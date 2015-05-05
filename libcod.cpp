#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h> // mprotect
#include <execinfo.h> // stacktrace

#include "cracking.hpp"
#include "gsc.hpp" /* cdecl_injected_closer() cdecl_cod2_player_damage_new() */
#include "server.hpp" /* startServerAsThread() */
#include "java_embed.h"

#pragma GCC visibility push(hidden)

/*
#include <stdio.h>
#include <string.h>
#include <unistd.h>
void findargs(int *argc, char ***argv)
{
	// http://stackoverflow.com/questions/160292/how-can-i-access-argc-and-argv-in-c-from-a-library-function
    size_t i;
    char **p = &__environ[-2];
    for (i = 1; i != *(size_t*)(p-1); i++) {
        p--;
    }
    *argc = (int)i;
    *argv = p;
}
void hideargs()
{
	// http://netsplit.com/2007/01/10/hiding-arguments-from-ps/
	int argc;
	char **argv;
	findargs(&argc, &argv);
	char *arg_end;
	arg_end = argv[argc-1] + strlen (argv[argc-1]);
	*arg_end = ' ';
}
static void *hideargs_waiter(void *arg)
{
	sleep(5);
	printf("hideargs()\n");
	hideargs();
}
void hideargs_thread()
{
	int ret;

	pthread_t handle;
	pthread_attr_t settings;

	int stacksize;

	ret = pthread_attr_init(&settings);
	if (ret != 0)
	{
		printf("> [ERROR] pthread_attr_init() failed.\n");
		return;
	}

	stacksize = 1024*1024*2;

	ret = pthread_attr_setstacksize(&settings, stacksize);
	if (ret != 0)
	{
		printf("> [ERROR] pthread_attr_setstacksize failed.\n");
		return;
	}

	//printf("> [INFO] Stack-Size set to %d Bytes (%.2f KB, %.2f MB)\n", stacksize, (float)(stacksize/1024), (float)((stacksize/1024)/1024));

	
	pthread_create(&handle, &settings, hideargs_waiter, NULL);
}
*/

// 1.3 address=0x08078FB2
int cdecl_calc_hash_of_string(char *str, unsigned int len)
{
	unsigned int v3;
	char *str2;
	unsigned int ret;
	int v6;
	
	//return 1337;
	
	if (len > 0xFF)
	{
		printf("calc_hash_of_string: if (len > 0xFF) -> NOT IMPLEMENTED\n");
		ret = 1; // 0 crashes, 1 works ever, just slow i guess
	} else {
		v3 = 0;
		str2 = str;
		while (len)
		{
			v6 = *str2;
			v3 *= 31;
			v3 += v6;
			++str2;
			--len;
		}
		ret = v3 % 0x3FFF + 1;
	}
	
	//if (1)
	if (ret == 0x0174 || ret == 0x0178)
	printf("calc_hash_of_string(\"%s\", %d) = %.8x;\n", str, len, ret);
	
	#if 0
	printf("calc_hash_of_string(\"");
	for (int i=0; i<len; i++)
		printf("%d,", str[i]);
	printf("\", %d);\n", len);
	#endif
	return ret;
}

/*
	the only output is "radiant/keys.txt"
	so its reading the file
	www.customcod.com/wiki/index.php?title=Scripting_Common_Tasks for radiant key explanation
*/
void cdecl_sub_807F840(char *str)
{
	printf("cdecl_sub_807F840(\"%s\");\n", str);
}

bool cdecl_gsc_cast_to_bool(aStackElement *element)
{
	printf("cdecl_gsc_cast_to_bool\n");
	return 0;
}



// game globals
int16_t *stack;
int (*sub_807B276)(int, int); // for cdecl_gsc_set_field_of_struct
//int (*sub_807B276)(int, int); // for cdecl_gsc_set_field_of_struct
void init_native_interface()
{
	// funcs
	*((int *)(&sub_807B276)) = 0x0807B276;
	
	// globals
	stack = (int16_t*)0x08297500;
}

// THIS FUNCTION IS CRASHING THROUGH SEGFAULT:
/*
	cdecl_gsc_set_field_of_struct(a1=00000184, a2=00000180) = 00000305;
	cdecl_gsc_set_field_of_struct(a1=00000184, a2=00000193) = 00000318;
	cdecl_gsc_set_field_of_struct(a1=00000184, a2=00000181) = fffffe99;
	./start.sh: line 4:  6138 Segmentation fault
*/

// OK, now this function returns int16_t and its not crashing anymore, BUTTTTT:
/*

cdecl_gsc_set_field_of_struct(a1=00000184, a2=000009d8) = ret=2909 ret_sub=00000b5d;
cdecl_gsc_set_field_of_struct(a1=00000184, a2=00000a28) = ret=-15911 ret_sub=00000bad;
cdecl_gsc_set_field_of_struct(a1=00000184, a2=00000a28) = ret=-15911 ret_sub=00000bad;
cdecl_gsc_set_field_of_struct(a1=00000184, a2=00000a28) = ret=-15911 ret_sub=00000bad;
cdecl_gsc_set_field_of_struct(a1=00000184, a2=000009d8) = ret=2909 ret_sub=00000b5d;
cdecl_gsc_set_field_of_struct(a1=00000184, a2=000009c6) = ret=2891 ret_sub=00000b4b;

*/


int16_t cdecl_gsc_set_field_of_struct(int a1, int a2)
{	
	int16_t ret;
	int ret_sub;
	

	ret_sub = sub_807B276(a1, a2);
	ret = stack[8 * ret_sub];

	printf("cdecl_gsc_set_field_of_struct(a1=%.8x, a2=%.8x) = ret=%d ret_sub=%.8x;\n", a1, a2, ret, ret_sub);
	return ret;
}

int gsc_new_variable_807AB64(int a1, int a2, int a_1_plus_b_mod_fffd_plus_1)
{
	printf("gsc_new_variable_807AB64(%.8x, %.8x, %.8x);\n", a1, a2, a_1_plus_b_mod_fffd_plus_1);
	return 1;
}


// ################### BSP ##########################

extern int level; // for a nice tabbed graphcall
#define LEVEL_SPACE do { for (int i=0; i<level; i++) printf("    "); }while(0);

// 0.00 0 0 1065353216 0.00 0.00 -1239976432 0.00 0
typedef struct
{
	float fraction;
	float normal[3];
	float d;
	float e;
	float f;
	float g;
	int h;
} aTrace;

void printStackTrace()
{
	void *array[40];
	int ret = backtrace(array, 10);
	int i;
	for (i=0; i<ret; i++)
		printf("%.8x ", (unsigned int)array[i]);
}

int trace_calc_fraction_805B894(aTrace *trace, float *vectorFrom, float *vectorTo, float *nullVector0, float *nullVector1, int isZero, int mask)
{
	
	int ret;
	
	printStackTrace();
	
	// its a recursive function, and it doesnt seem to end with printf()
	// so now without printf:
	#if 1
	int (*tmp)(aTrace *trace, float *vectorFrom, float *vectorTo, float *nullVector0, float *nullVector1, int isZero, int mask);
	// *(int *)&tmp = 0x0805B894; // ups, can call a function i just replaced with JUMP to this
	
	memset(trace, 0, 36);
	trace->fraction = 1.0;
	*(int *)&tmp = 0x0805B414;
	ret = tmp(trace, vectorFrom, vectorTo, nullVector0, nullVector1, isZero, mask);
	
	#if 1
	/*
	
991.73,2877.99,89.02, 991.73,2877.99,88.52, -15.00,-15.00,0.00, 15.00,15.00,50.00, 0, 02810011 -> 0.50 0.00 -0.34 0.94 0.00 0.00 -0.00 0.00 0
991.73,2877.99,89.02, 991.73,2877.99,88.52, -15.00,-15.00,0.00, 15.00,15.00,50.00, 0, 02810011 -> 0.50 0.00 -0.34 0.94 0.00 0.00 -0.00 0.00 0
991.73,2877.99,88.77, 992.14,2885.98,88.77, -9.00,-9.00,8.00, 9.00,9.00,50.00, 0, 02810011 -> 1.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0
991.73,2877.99,89.02, 991.73,2877.99,88.52, -15.00,-15.00,0.00, 15.00,15.00,50.00, 0, 02810011 -> 0.50 0.00 -0.34 0.94 0.00 0.00 -0.00 0.00 0
991.73,2877.99,89.02, 991.73,2877.99,88.52, -15.00,-15.00,0.00, 15.00,15.00,50.00, 0, 02810011 -> 0.50 0.00 -0.34 0.94 0.00 0.00 -0.00 0.00 0
991.73,2877.99,88.77, 992.14,2885.98,88.77, -9.00,-9.00,8.00, 9.00,9.00,50.00, 0, 02810011 -> 1.00 0.00 0.00 0.00 0.00 0.00 0.00 0.00 0
991.73,2877.99,89.02, 991.73,2877.99,88.52, -15.00,-15.00,0.00, 15.00,15.00,50.00, 0, 02810011 -> 0.50 0.00 -0.34 0.94 0.00 0.00 -0.00 0.00 0
	*/
	#if 0
	printf("%.2f,%.2f,%.2f, %.2f,%.2f,%.2f, %.2f,%.2f,%.2f, %.2f,%.2f,%.2f, %d, %.8x -> ",
		vectorFrom[0],
		vectorFrom[1],
		vectorFrom[2],
		
		vectorTo[0],
		vectorTo[1],
		vectorTo[2],
		
		nullVector0[0],
		nullVector0[1],
		nullVector0[2],
		
		nullVector1[0],
		nullVector1[1],
		nullVector1[2],
		
		isZero,
		mask
	);
	#endif
	
	/*
	// lol, this couldnt work, because this function ONLY calulates the fraction... thats what i needed for zombots
	// trace_t will be filled up later in the main-function of cg_trace
	if (
		trace->d > 0.001 ||
		trace->d < -0.001 ||
		trace->e > 0.001 ||
		trace->e < -0.001 ||
		trace->f > 0.001 ||
		trace->f < -0.001 ||
		trace->g > 0.001 ||
		trace->g < -0.001
	)*/
	printf("%9.2f %9.2f %9.2f %9.2f %9.3f %9.3f %9.3f %9.3f %.8x\n",
		trace->fraction,
		trace->normal[0],
		trace->normal[1],
		trace->normal[2],
		trace->d,
		trace->e,
		trace->f,
		trace->g,
		trace->h
	);
	#endif
	
	return ret;
	#endif
	
	
	LEVEL_SPACE; printf(/*AT*/ "int sub_805B894(TODO);\n");
	LEVEL_SPACE; printf("{\n");
	level++;

	#if 0
	LEVEL_SPACE; printf("BUILTIN-FUNCTION\n");
	int (*tmp)(float *outFraction, float *vectorFrom, float *vectorTo, float *nullVector0, float *nullVector1, int isZero, int mask);
	*(int *)&tmp = 0x0805B894;
	ret = tmp(outFraction, vectorFrom, vectorTo, nullVector0, nullVector1, isZero, mask);
	#endif
	
	#if 0
	int *stackStart = (int *)0x08297500; // 8297500 -> fffd0000
	
	*(int *)(stackStart + v2 + 2) /*|*/= stackElement->type;
	
	*(int *)( (int*)(stackStart + 4 * v2) + 1) = (int)stackElement->offsetData;
	
	ret = (int)&stackElement->offsetData;
	#endif
	
	#if 0 // ANOTHER TRY ANOTHER FAIL -.-
		aStack *stackStart = (aStack *)0x8297500;
		aStack *v3;
		printf("real <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
		
		v3 = stackStart + v2;
		//*(int*)(&stackStart[v2+2]) |=  stackElement->type;
		*(int*)(stackStart + v2 + 2) =  stackElement->type;
		ret = (int)stackElement->offsetData;
		*( ((int*)v3) + 1) = (int)stackElement->offsetData;
		
		//printf("        stackStart + 2 == %.8x, %.8x\n", stackStart + 2, (int)stackStart + 2*16);
		//printf("    stackStart + 2 + 2 == %.8x, %.8x\n", stackStart + 2 + 2, (int)stackStart + 4*16);
		//printf("stackStart + 2 + 2 + 2 == %.8x, %.8x\n", stackStart + 2 + 2 + 2, (int)stackStart + 6*16);
	#endif
	
	// TYPE POINTER SIZE DIFFERENCE
	/*
		bla1=08297504
		bla2=08297510 // hex 10 = 16 = 4*sizeof(int)
	*/
	#if 0
	{
	int stackStart = (int )0x08297500; // 8297500 -> fffd0000
	printf("bla1=%.8x\n", stackStart + 4);
	}
	{
	int *stackStart = (int *)0x08297500; // 8297500 -> fffd0000
	printf("bla2=%.8x\n", stackStart + 4);
	}
	#endif
	
	level--; LEVEL_SPACE; printf("} ret=%.8x\n", ret);
	
	return ret;
}

void sub_809D884(aTrace *trace, float *vectorFrom, float *mins, float *maxs, float *vectorTo, int ignoreEntity, int mask, int isOne1, int isZero, int isOne2)
{
	printf("CG-TRace: ");
	printStackTrace();
	printf("\n");
}

// ################### BSP ##########################

//Cmd_AddCommand_80606BE("net_dumpprofile", (int)net_dumpprofile_806B89A);

// from qcommon.h
typedef void ( *xcommand_t )( void );

unsigned int hook_str2hash_8078EE6(char *functionLower, int len)
{
	if (len >= 4 && strstr(functionLower, "is"))
		printf("str2hash %d: %s\n", len, functionLower);
	return len;
}




/*
how to start?
run twice:
stupidTestFunction
stupidTestFunction
*/
/*output:


hash: 8
lookup: 8
v14: 08110976
v13: 0000000a
*/

#define __cdecl

int hook_807C0B8(int/*16_t*/ a1, int a2)
{
	printf("a1=%.4x a2=%.8x\n", a1, a2);
	printf("0");
}
bool __cdecl hook_8075D48(int a1)
{
	printf("before\n");
	int *dword_83964C8 = (int *)0x83964C8;
	int *dword_8204C30 = (int *)0x8204C30;
	
	printf("after\n");
	
	bool tmp = a1 - *dword_83964C8 < (unsigned int)*dword_8204C30;
	
	
	printf("after 2\n");
	
	return tmp;
}

int hook_codscript_load_label_8075DEA(char *file, char *function)
{
	{
		printf("############ file: %s function: %s\n", file, function);

		int (*original)(char * filr, char *function);
		*(int *)&original = 0x8075DEA;
		int ret;
		
		cracking_write_hex(0x8075DEA, (char *)"5589E583EC48");
		
		ret = original(file, function);
		
		//asm("jmp h8075DEA");
		cracking_hook_function(0x08075DEA, (int)hook_codscript_load_label_8075DEA);
		printf("ret: %.8x\n", ret);
		return ret;
	}

	int (*hash_dunno_3_807A1AE)(char *file);
	int (*sub_807C0B8)(int/*16_t*/ a1, int a2);
	int *dword_8204C0C;
	int (*sub_807D0A2)(int lookup);
	int (*sub_807926E)(char *function);
	int (*sub_807E500)(int a1);
	int (*sub_807CBEE)(int *a1);
	/*bool*/ int (*sub_8075D48)(int a1);

	
	
	*(int *)&hash_dunno_3_807A1AE = 0x0807A1AE;
	*(int *)&sub_807C0B8 = 0x0807C0B8;
	//*(int *)&sub_807C0B8 = (int)hook_807C0B8;
	dword_8204C0C = (int *)0x08204C0C;
	*(int *)&sub_807D0A2 = 0x0807D0A2;
	*(int *)&sub_807926E = 0x0807926E;
	*(int *)&sub_807E500 = 0x0807E500;
	*(int *)&sub_807CBEE = 0x0807CBEE;
	//*(int *)&sub_8075D48 = 0x08075D48;
	  *(int *)&sub_8075D48 = (int)hook_8075D48;
	
	int *dword_83964C8 = (int *)0x083964C8;
	
	printf("_DWORD __cdecl hook_codscript_load_label_8075DEA(char *file, char *function)\n");
	printf("file: %s function: %s\n", file, function);

	int ret; // [sp+Ch] [bp-3Ch]@2
	int v4; // [sp+10h] [bp-38h]@9
	int v5; // [sp+14h] [bp-34h]@9
	int v6; // [sp+18h] [bp-30h]@9
	int v7; // [sp+1Ch] [bp-2Ch]@9
	int v8; // [sp+20h] [bp-28h]@9
	int v9; // [sp+24h] [bp-24h]@9
	int v10; // [sp+2Ch] [bp-1Ch]@9
	int hash; // [sp+30h] [bp-18h]@1
	int v12; // [sp+34h] [bp-14h]@11
	int v13; // [sp+38h] [bp-10h]@3
	int v14; // [sp+3Ch] [bp-Ch]@3
	int v15; // [sp+40h] [bp-8h]@5
	int lookup; // [sp+44h] [bp-4h]@1

	
	hash = hash_dunno_3_807A1AE(file);
	printf("hash: %d\n", hash);
	
	lookup = sub_807C0B8(*dword_8204C0C, hash);
	printf("lookup: %d\n", lookup);
	
	if (!lookup)
		return 0;
	
    v14 = sub_807D0A2(lookup);
	printf("v14: %.8x\n", v14);
	
    v13 = sub_807926E(function);
	printf("v13: %.8x\n", v13);

	if (!v13)
		return 0;
	
	v15 = sub_807C0B8(v14, v13);
	printf("v15: %.8x\n", v15);
	
	if (!v15)
		return 0;
	
	int tmp = sub_807E500(v15);
	printf("tmp = %.8x\n", tmp);
	
	if (tmp != 1 )
		return 0;
	
	v7 = sub_807D0A2(v15);
	printf("v7: %.8x\n", v7);
	v6 = sub_807C0B8(v7, 1);
	printf("v6: %.8x\n", v6);
	sub_807CBEE(&v4);
	printf("after v6\n");
	
	/*
		hash: 372
		lookup: 376
		v14: 00000007
		v13: 00000178
		v15: 00000180
		tmp = 00000001
		v7: 00000008
		v6: 0000000a
		after v6
		./start_surf.sh: line 33: 10018 Segmentation fault      $cod2 $args
	*/
	
	v8 = v4;
	v9 = v5;
	v10 = v4;
	printf("v10: %.8x\n", v10);
	
	printf("before if ( sub_8075D48(v4) v4=%.8x )\n", v4);
	
	ret = hook_8075D48(v4);
	printf("sub_8075D48 ret=%.8x\n", ret);
	if ( ! ret)
		return 0;
		
		
	printf("dword_83964C8 = %.8x\n", *dword_83964C8);
		
	v12 = v10 - *dword_83964C8;
	printf("v12: %.8x\n", v12);
	ret = v10 - *dword_83964C8;
	printf("ret=%.8x\n", ret);
	return ret;
}

int codecallback_playercommand = 0;
int codecallback_userinfochanged = 0;

typedef void (*gametype_scripts_t)();
#if COD_VERSION == COD2_1_0
	gametype_scripts_t gametype_scripts = (gametype_scripts_t)0x0810DDEE; 
#elif COD_VERSION == COD2_1_2
	gametype_scripts_t gametype_scripts = (gametype_scripts_t)0x0811012A;
#elif COD_VERSION == COD2_1_3
	gametype_scripts_t gametype_scripts = (gametype_scripts_t)0x08110286;
#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
	gametype_scripts_t gametype_scripts = (gametype_scripts_t)0x080C0A7A;
#else
	#warning gametype_scripts_t gametype_scripts = (gametype_scripts_t)NULL;
	gametype_scripts_t gametype_scripts = (gametype_scripts_t)NULL;
#endif

typedef int (*codscript_load_function_t)(char *file, char *function, int isNeeded);
#if COD_VERSION == COD2_1_0
	codscript_load_function_t codscript_load_function = (codscript_load_function_t)0x0810DD70;
#elif COD_VERSION == COD2_1_2
	codscript_load_function_t codscript_load_function = (codscript_load_function_t)0x081100AC;
#elif COD_VERSION == COD2_1_3
	codscript_load_function_t codscript_load_function = (codscript_load_function_t)0x08110208;
#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
	codscript_load_function_t codscript_load_function = (codscript_load_function_t)0x080C09E8; // int __usercall sub_80C09E8<eax>(int a1<eax>, int a2<edx>, int a3<ecx>)
#else
	#warning codscript_load_function_t codscript_load_function = (codscript_load_function_t)NULL;
	codscript_load_function_t codscript_load_function = (codscript_load_function_t)NULL;
#endif

int codscript_load_function_custom(char *file, char *function, int isNeeded)
{
	typedef int (*codscript_file_load_function_t)(char *file, char *function);
	#if COD_VERSION == COD4_1_7
		codscript_file_load_function_t codscript_file_load_function = (codscript_file_load_function_t)0x0814C194;
	#elif COD_VERSION == COD4_1_7_L
		codscript_file_load_function_t codscript_file_load_function = (codscript_file_load_function_t)0x0814C1B4;
	#else
		codscript_file_load_function_t codscript_file_load_function = (codscript_file_load_function_t)NULL;
	#endif

	typedef int (*codscript_file_load_t)(char *file);
	#if COD_VERSION == COD4_1_7
		codscript_file_load_t codscript_file_load = (codscript_file_load_t)0x0814C076;
	#elif COD_VERSION == COD4_1_7_L
		codscript_file_load_t codscript_file_load = (codscript_file_load_t)0x0814C096;
	#else
		codscript_file_load_t codscript_file_load = (codscript_file_load_t)NULL;
	#endif

	if (!codscript_file_load(file))
	{
		if(isNeeded)
			printf((char*)"Could not find script '%s'\n", file);

		return 0;
	}

	int result = codscript_file_load_function(file, function);

	if(!result && isNeeded)
		printf((char *)"Could not find label '%s' in script '%s'\n", function, file);

	return result;
}

void hook_codscript_gametype_scripts()
{
	#if COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		codecallback_playercommand = codscript_load_function_custom((char *)"maps/mp/gametypes/_callbacksetup", (char *)"CodeCallback_PlayerCommand", 0);
		codecallback_userinfochanged = codscript_load_function_custom((char *)"maps/mp/gametypes/_callbacksetup", (char *)"CodeCallback_UserInfoChanged", 0);
	#else
		codecallback_playercommand = codscript_load_function((char *)"maps/mp/gametypes/_callbacksetup", (char *)"CodeCallback_PlayerCommand", 0);
		codecallback_userinfochanged = codscript_load_function((char *)"maps/mp/gametypes/_callbacksetup", (char *)"CodeCallback_UserInfoChanged", 0);
	#endif

	//printf("codecallback_playercommand=%.8x\n", codecallback_playercommand);

	// unhook
	#if COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		cracking_write_hex((int)gametype_scripts, (char *)"5589E55383EC54");
	#else
		cracking_write_hex((int)gametype_scripts, (char *)"5589E583EC58"); // todo: hook->unhook()
	#endif

	// call original
	gametype_scripts();

	// hook again
	cracking_hook_function((int)gametype_scripts, (int)hook_codscript_gametype_scripts);
}

int hook_ClientCommand(int clientNum)
{
	//printf("clientNum: %d\n", clientNum);
	
	//cracking_hook_function(0x08100D1E, hook_ClientCommand_8100D1E);
	
	/*
	// perfect idea: dont call the original function here, so other players wont see chat
	// so ppl can do cracked servers with !login mulder trustno1
	// ooooops, need to call it always, when the callback was not found
	*/
	if ( ! codecallback_playercommand)
	{
		//printf("NOT USING hook_ClientCommand(), because codecallback_playercommand was not defined.\n");
		return ClientCommand(clientNum);
	}
	/*
	int (*ClientCommand_8100D1E)(int clientNum);
	*(int *)&ClientCommand_8100D1E = 0x8100D1E;
	ClientCommand_8100D1E(clientNum);
	*/
	
	stackPushArray();
	int args = trap_Argc();
	//printf("args: %d\n", args);
	for (int i=0; i<args; i++)
	{
		char tmp[1024];
		
		trap_Argv(i, tmp, sizeof(tmp));
		stackPushString(tmp);
		//printf("pushing: %s\n", tmp);
		stackPushArrayLast();
	}
	
	// todo: G_ENTITY(clientNum)
	#if COD_VERSION == COD2_1_0 // search '\\name\\badinfo'
		short ret = codscript_call_callback_entity(/*gentity*/0x08665480 + 560 * clientNum, codecallback_playercommand, 1);
	#elif COD_VERSION == COD2_1_2
		short ret = codscript_call_callback_entity(/*gentity*/0x08679380 + 560 * clientNum, codecallback_playercommand, 1);
	#elif COD_VERSION == COD2_1_3
		short ret = codscript_call_callback_entity(/*gentity*/0x08716400 + 560 * clientNum, codecallback_playercommand, 1);
	#elif COD_VERSION == COD4_1_7
		short ret = codscript_call_callback_entity(/*gentity*/0x0841F260 + 628 * clientNum, codecallback_playercommand, 1);
	#elif COD_VERSION == COD4_1_7_L
		short ret = codscript_call_callback_entity(/*gentity*/0x0841FFE0 + 628 * clientNum, codecallback_playercommand, 1);
	#else
		#warning short ret = codscript_call_callback_entity(NULL, codecallback_playercommand, 1);
		short ret = codscript_call_callback_entity(NULL, codecallback_playercommand, 1);
	#endif

	//printf("codecallback_playercommand=%.8x ret=%i\n", codecallback_playercommand, ret);
	
	codscript_callback_finish(ret);

	//printf("after codscript_callback_finish\n");
	
	return 0;
}


// Cmd_AddCommand("stupidTestFunction", stupidTestFunction);
void stupidTestFunction()
{
	printf("Lulz, i'm stupidTestFunction!\n");
	
	//cracking_hook_function(0x08075DEA, (int)hook_codscript_load_label_8075DEA);
	
	#if 0
		int (*sub_81100AC)(char *file, char *function, int exitOnError);

		*(int *)&sub_81100AC = 0x081100AC;
		
		printf("before func_ref!\n");
		int func_ref = sub_81100AC((char *)"bbbbtest", (char *)"helloworld", 1);
		printf("after func_ref!\n");
	#endif
	
	#if 0
		// status: makes game freeze
		// this reloads the scripts and ...
			
		void (*codscript_load_scripts_81102C2)();
		*(int *)&codscript_load_scripts_81102C2 = 0x081102C2;
		codscript_load_scripts_81102C2();
		
		// ... then calling codecallback_onstartgametype() 
		void (*call_startgametype_81181C6)();
		*(int *)&call_startgametype_81181C6 = 0x081181C6;
		call_startgametype_81181C6();
	#endif
	
	#if 1
		int ret;
		
		int (*call_callback_entity_811B128)(int, int, int);
		*(int *)&call_callback_entity_811B128 = 0x0811B128;
		ret = call_callback_entity_811B128(/*gentity*/0x08716400, *(int*)0x087B0698, 10);

		int (*call_callback_second_808410A)(int);
		*(int *)&call_callback_second_808410A = 0x0808410A;
		call_callback_second_808410A(ret);
	#endif
	
}

void Cmd_AddCommand(const char *cmd_name, xcommand_t function)
{
	void (*signature)(const char *cmd_name, xcommand_t function);

	#if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x080604B2;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x080606BE;
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x080606B6;
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x081116B4;
	#else
		#warning void (const char *cmd_name, xcommand_t function) *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = NULL;
		//printf("1\n");
		return; // normally i want it to crash, so i will fix it, but need it now working
	#endif
	
	signature(cmd_name, function);
}

ssize_t hook_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	static ssize_t (*libc_sendto)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) = NULL;
	
	if ( ! libc_sendto)
		*(int *)&libc_sendto = (int) dlsym(RTLD_NEXT, "sendto");
	
    ssize_t st = libc_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
	
	
	if (1) // if i want to disable sendto-spam
		return st;
	
    /*dn_log("    >> intercepted sendto(%d, %p, %zu, %d, %p, %lu)\n",
        sockfd, buf, len, flags, dest_addr, (unsigned long)addrlen);
    st = syscall(SYS_SENDTO, sockfd, buf, len, flags, dest_addr, addrlen);*/
	
    //printf("sendto()ed %zu bytes: %.*s\n", st, (int)st, (const char *)buf);
	
		struct sockaddr_in *ipv4sockdata = (struct sockaddr_in *) dest_addr;
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(ipv4sockdata->sin_addr), ip, INET_ADDRSTRLEN);
		printf("SEND %s:%d ", ip, ipv4sockdata->sin_port);
	
		int i;
		for (i=0; i<st && i<100; i++)
		{
			char currentchar = ((char *)buf)[i];
			
			if (currentchar >= 32 && currentchar <= 126)
				printf("%c", currentchar);
			else {
				printf("\\x%02X", currentchar&0xff);
				//printf(".");
			}
		}
		printf("\n");
	
    return st;
}

ssize_t hook_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
	// len is in cod2 0x20000, real packet length is in "rf"
	static ssize_t (*libc_recvfrom)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) = NULL;
	
	if ( ! libc_recvfrom)
		*(int *)&libc_recvfrom = (int) dlsym(RTLD_NEXT, "recvfrom");
	
    ssize_t rf = libc_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);	

	if (rf == -1) // just debug when the function was a success
	{
		// errno = WOULDBLOCK
		//printf("> recvfrom(fd=%d, buf=%s, len=%zu, flags=%d, src_addr=%p, addrlen=%p)\n", sockfd, buf, len, flags, src_addr, addrlen);
		return rf;
	}

	#if COMPILE_DEBUG_SERVER == 1
	//printf("packet!\n");
	if (*(unsigned int *)buf == 0xFFFFFFFF)
	{
		//printf("got stateless packet! %s\n", buf);
		/*
		typedef struct ip_s
		{
			unsigned char d;
			unsigned char c;
			unsigned char b;
			unsigned char a;
		} ip_t;*/
		
		if (0)
		{
			printf("> recvfrom(fd=%d, buf=%s, len=%zu, flags=%d, src_addr=%p, addrlen=%p) ip:%s\n", sockfd, (char*)buf, len, flags, src_addr, addrlen,
				inet_ntoa(((struct sockaddr_in *)src_addr)->sin_addr)
			);
			printf(">>>>>>>%s<<<<<<<<<\n", (char*)buf);
		}
		int port, debug;
		int ret = sscanf((char*)buf, "\xff\xff\xff\xffweb %d %d", &port, &debug);
		
		if (ret == 2) // sscanf succeeded?
		{
			printf("ret=%d port=%d debug=%d\n", ret, port, debug);
			
			startServerAsThread(port, debug);
		}
	}
	#endif
	
	return rf;
		
	/*
		if someone spoofed a message from our server to another server with:
			- getstatus
			- getinfo xxx
		, the server will respond with:
			- statusResponse
			- infoResponse
		we dont want those packets inspected by our server (they are just crap,
		and i dont know what they could to - like lag)
		so just return -1, and cod2 thinks "no packet received"
		TODO: test it with packetspoofer
		
		a -1 stands for: SOCKET_ERROR==-1
		the server prints then a message like:
		NET_GetPacket: No such file or directory from 95.33.186.240:2805
		NET_GetPacket: No such file or directory from 92.225.223.210:28960
		NET_GetPacket: No such file or directory from 88.190.17.215:-24716
		
		from Enemy Territory:
		Com_printf("NET_GetPacker: %s\n", NET_ErrorString());
		
		well, i set it to "0" now... no error like -1
		
	*/
	
	// shortcut those long parsing stuff (still "hits" the gameserver)
	// but when i ban their ips, ppl can spoof such a packet for a player
	if (
		(memcmp("\xff\xff\xff\xffstatusResponse", buf, 18) == 0) ||
		(memcmp("\xff\xff\xff\xffinfoResponse", buf, 16) == 0)
	)
	{
		((char *)buf)[4] = 'f';
		((char *)buf)[5] = 'o';
		((char *)buf)[6] = 'o';
		((char *)buf)[7] = '\0';
		return 8;
	}
		
	// gameserver dont answer one these also
	// my python-twisted-bot shall answer those
	// then the gameserver isnt busy with it
	// AND i can detect drdos (urbanterror.info)
	// and i can easily find non-spoofer-flooders and ban them with iptables
	// also i can list the ips with php and set "custom messages" for:
	//  -rcon -> fake access
	//  -status -> fake lag
	
	/*
		still need to think what the mysql-tables is needing...
		 - ip:port
		 - time
		 - rcon/getstatus/getinfo -> rcon with pass
	*/
	#if 0
	if (
		(memcmp("\xff\xff\xff\xffgetstatus", buf, 13) == 0) ||
		(memcmp("\xff\xff\xff\xffgetinfo", buf, 11) == 0) ||
		(memcmp("\xff\xff\xff\xffrcon", buf, 8) == 0)
	)
	{
		printf("drop get\n");
		// DO NOT MANIPULATE THE \xff\xff\xff\xff, the server needs to recognise a "connectionless packet",
		// but it shoudlt have any sense (the server just shall dispose it to prevent answer)
		((char *)buf)[4] = 'f'; // let the first 4 0xff
		((char *)buf)[5] = 'o'; // let the first 4 0xff
		((char *)buf)[6] = 'o'; // let the first 4 0xff
		((char *)buf)[7] = '\0'; // let the first 4 0xff
		/* http://svn.icculus.org/quake3/branches/1.34/code/unix/unix_net.c?limit_changes=100&view=markup */
		/* lets fake a good ending, so we dont get in trouble */
		return 8/*rf*/; // look server, its a connectionless packet 0xffffffff"foo\n" ;P
	}
	
	// dont debug packets anymore :)
	return rf;
	#endif
	
	// ok, here lets just debug the connectionless packets
	if (memcmp("\xff\xff\xff\xff", buf, 4) != 0)
		return rf;
	
	//printf("> recvfrom(%d, %p, %zu, %d, %p, %p)\n", sockfd, buf, len, flags, src_addr, addrlen);
	
	int max = 50;
	
	struct sockaddr_in *ipv4sockdata = (struct sockaddr_in *) src_addr;
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ipv4sockdata->sin_addr), ip, INET_ADDRSTRLEN);
	printf("RECV %s:%d ", ip, ipv4sockdata->sin_port);
	
	int i;
	for (i=0; i<rf && i<100; i++)
	{
		char currentchar = ((char *)buf)[i];
		
		if (currentchar >= 32 && currentchar <= 126)
			printf("%c", currentchar);
		else {
			printf("\\x%02X", currentchar&0xff);
			//printf(".");
		}
	}
	printf("\n");

    return rf;
}

#include <pthread.h>

int hook_pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
	static int (*signature)(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg) = NULL;
	
	if ( ! signature)
		*(int *)&signature = (int) dlsym(RTLD_NEXT, "pthread_create");
	
	int ret = signature(thread, attr, start_routine, arg);

	printf("  pthread_create(...) = %d\n", ret);
	
    return ret;
}

pthread_t hook_pthread_self(void)
{
	static int (*signature)(void) = NULL;
	
	if ( ! signature)
		*(int *)&signature = (int) dlsym(RTLD_NEXT, "pthread_self");
	
	int ret = signature();

	printf("  pthread_self() = %.8x\n", ret);
	
    return ret;
}

int hook_pthread_mutex_lock(pthread_mutex_t *mutex)
{
	static int (*signature)(pthread_mutex_t *mutex) = NULL;
	
	if ( ! signature)
		*(int *)&signature = (int) dlsym(RTLD_NEXT, "pthread_mutex_lock");
	
	int ret = signature(mutex);

	printf("  pthread_mutex_lock(%p) = %d\n", mutex, ret);
	
    return ret;
}
int hook_pthread_mutex_trylock(pthread_mutex_t *mutex);
int hook_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	static int (*signature)(pthread_mutex_t *mutex) = NULL;
	
	if ( ! signature)
		*(int *)&signature = (int) dlsym(RTLD_NEXT, "pthread_mutex_unlock");
	
	int ret = signature(mutex);

	printf("pthread_mutex_unlock(%p) = %d\n", mutex, ret);
	
    return ret;
}

// 8060ADC
size_t hook_fread ( void * ptr, size_t size, size_t count, FILE * stream )

{
	static int (*signature)( void * ptr, size_t size, size_t count, FILE * stream) = NULL;
	
	if ( ! signature)
	{
		printf("INIT HOOK: fread\n");
		*(int *)&signature = (int) dlsym(RTLD_NEXT, "fread");
	}
	

	#if 0
	void *buffer[20];
	int size2, i;
	size2 = backtrace(buffer, 20);
	#endif
	
	// 080d1ae0 - couldnt load ANY file
	// 080d1b18 - couldnt load ANY file
	// 080d1d3a - couldnt load ANY file
	
	// 0x080d1ef0 0
	// 0x080d1f16 0
	// 0x080d1f3c 0
	// 0x080d1f65 0
	// 0x080d1f8b 0
	// 0x080d1fe0 - list got generated
	// 0x080d2009 - slow but: Sys_Error: Error during initialization: EXE_ERR_COULDNT_LOADmaps/mp/surf_utopia.d3dbsp
	// 0x080d2032 - slow but starts...
	#if 0
	if ((int)buffer[4] == 0x080a0f2f)
	{
		printf("BREAK 11!\n");
		return -1;
	}
	#endif
	
	#if 0
	for (i=0; i<size2; i++)
		printf("%.8x ", buffer[i]);
	printf("\n");
	#endif
	int ret = signature(ptr, size, count, stream);

	//if (stream->_fileno <= 4)
	//	printf("_fileno=%d ", stream->_fileno);
	if (fileno(stream) <= 4)
		printf("fileno(stream)=%d ", fileno(stream));
	
	//if (size==1)
	//	printf("fread(ptr=%.8p size=%d count=%d stream=%.8p) = %d\n", ptr, size, count, stream, ret);
	
    return ret;
}

/*
developer 0
set scr_testclients 2

*/

/*


setcontents(0) + das = no collide
write int 080F6DAf 00000000
write int 080F6DBe 00000000


to find the opcodes, search in winhex: C01C010000   look from AD577....

bool __cdecl sub_80917CC(int a1, int a2) // 284 ref!!! /4 = 71     == 11c   0x47

int __cdecl gsc_setcontents_81120B0(__int16 a1)
{
  int v2; // ST14_4@1
  int v3; // ST10_4@1

  v2 = sub_8110376(a1);
  v3 = codscript_get_param_int_80845C8(0);
  stackPushInt_8085098(*(_DWORD *)(v2 + 284));
  *(_DWORD *)(v2 + 284) = v3;
  return sub_809C4F6(v2);
}

*/

// 1.2 0x080F6D5A
int hook_player_eject(int player) // player 0 = 0x08679380 + 0x11c = 0x0867949c
{
	//printf("int hook_player_eject(int player=%.8x)\n", player);
	return 0;
}

#define _DWORD int
#define __cdecl 
#if COD_VERSION == COD1_1_5

void *Sys_LoadDll_80D3DAD(char *name, char *dest, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...));
cHook hook_Sys_LoadDll(0x080D3DAD, (int)Sys_LoadDll_80D3DAD);

typedef char * (*Sys_Cwd_80D5E2A_t)();
typedef char * (*Cvar_VariableString_80734FC_t)(char *);
typedef int (*FS_BuildOSPath_8060D52_t)(char *src, char *, char *, char **dest);
typedef int (*Com_Printf_806FC90_t)(const char *format, ...);
typedef void (*Com_Error_806FEF4_t)(signed int errorcode, const char *format, ...);
typedef char *(*Q_strncpyz_8085EB2_t)(char *dest, const char *src, int destsize);

Sys_Cwd_80D5E2A_t Sys_Cwd_80D5E2A = (Sys_Cwd_80D5E2A_t)0x080D5E2A;
Cvar_VariableString_80734FC_t Cvar_VariableString_80734FC = (Cvar_VariableString_80734FC_t)0x080734FC;
FS_BuildOSPath_8060D52_t FS_BuildOSPath_8060D52 = (FS_BuildOSPath_8060D52_t)0x08060D52;
Com_Printf_806FC90_t Com_Printf_806FC90 = (Com_Printf_806FC90_t)0x0806FC90;
Com_Error_806FEF4_t Com_Error_806FEF4 = (Com_Error_806FEF4_t)0x0806FEF4;
Q_strncpyz_8085EB2_t Q_strncpyz_8085EB2 = (Q_strncpyz_8085EB2_t)0x08085EB2;

// lulz, only this worked, because i compiled only with: ./doit.sh cod1_1_5
// so not needed anymore...
int why_you_no_work()
{
	printf("int why_you_no_work()\n");
	return cdecl_injected_closer();
}

void *Sys_LoadDll_80D3DAD(char *name, char *dest, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...))
{
  char *err; // eax@4
  char *error; // eax@12
  char *fn; // [sp+20h] [bp-228h]@1
  char *gamedir; // [sp+120h] [bp-128h]@1
  char *basepath; // [sp+124h] [bp-124h]@1
  char *homepath; // [sp+128h] [bp-120h]@1
  char *pwdpath; // [sp+12Ch] [bp-11Ch]@1
  char fname[100]; // [sp+130h] [bp-118h]@1
  void *dllEntry; // [sp+238h] [bp-10h]@10
  void *libHandle; // [sp+23Ch] [bp-Ch]@1

  printf("MY OWN Sys_LoadDll! systemcalls=%08x\n", systemcalls);
  printf("MY OWN Sys_LoadDll!\n");
  printf("MY OWN Sys_LoadDll!\n");
  
  hook_Sys_LoadDll.unhook();
  
  void *(*Sys_LoadDll_original)(char *name, char *dest, int (**entryPoint)(int, ...), int (*systemcalls)(int, ...));
  *(int *)&Sys_LoadDll_original = 0x080D3DAD;
  void *ret = Sys_LoadDll_original(name, dest, entryPoint, systemcalls);
  
  hook_Sys_LoadDll.hook();
  
  //printf("vmMain=%08x\n", dlsym(ret, "vmMain"));
  //printf("functions=%08x\n", dlsym(ret, "functions"));
  int *functions = (int *)dlsym(ret, "functions");
  for (int i=0; i<110; i++)
  {
	int *set =  functions + 3*i;
	
	char **functionname = (char **)(set + 0);
	int **function = (int **)(set + 1);
	int **developer = (int **)(set + 2);
	
	if (strcmp(*functionname, "closer")==0)
	{
		//printf("hooking gsc.closer=%08x to c.closer=%08x\n", *function, cdecl_injected_closer);
		*function = (int *)cdecl_injected_closer;
	}
	
	//printf("asd %d = %08x/%s func[1]=%08x func[2]=%08x\n", i, *functionname,*functionname, *function, *developer);
  }
  
  return ret;
  
  #if 0
  *dest = 0;
  snprintf(fname, 0x100u, "%s.mp.i386.so", name);
  pwdpath = Sys_Cwd_80D5E2A();
  homepath = Cvar_VariableString_80734FC("fs_homepath");
  basepath = Cvar_VariableString_80734FC("fs_basepath");
  gamedir = Cvar_VariableString_80734FC("fs_game");
  FS_BuildOSPath_8060D52(pwdpath, gamedir, fname, &fn);
  Com_Printf_806FC90("Sys_LoadDll(%s)... ", &fn);
  libHandle = dlopen((const char *)&fn, 2);
  if ( libHandle )
  {
    Com_Printf_806FC90("ok\n");
  }
  else
  {
    Com_Printf_806FC90("failed\n");
    FS_BuildOSPath_8060D52(homepath, gamedir, fname, &fn);
    Com_Printf_806FC90("Sys_LoadDll(%s)... ", &fn);
    libHandle = dlopen((const char *)&fn, 2);
    if ( libHandle )
    {
      Com_Printf_806FC90("ok\n");
    }
    else
    {
      Com_Printf_806FC90("failed\n");
      FS_BuildOSPath_8060D52(basepath, gamedir, fname, &fn);
      Com_Printf_806FC90("Sys_LoadDll(%s)... ", &fn);
      libHandle = dlopen((const char *)&fn, 2);
      if ( libHandle )
      {
        Com_Printf_806FC90("ok\n");
      }
      else
      {
        err = dlerror();
        Com_Printf_806FC90("\nSys_LoadDll(%s) failed:\n\"%s\"\n", &fn, err);
      }
      if ( !libHandle )
        Com_Error_806FEF4(0, "Sys_LoadDll(%s) failed dlopen() completely!\n", name);
    }
  }
  Q_strncpyz_8085EB2(dest, (const char *)&fn, 64);
  dllEntry = dlsym(libHandle, "dllEntry");
  *entryPoint = (int (*)(int, ...))dlsym(libHandle, "vmMain");
  if ( !*entryPoint || !dllEntry )
  {
    error = dlerror();
    Com_Error_806FEF4(0, "Sys_LoadDll(%s) failed dlsym(vmMain):\n\"%s\" !\n", name, error);
  }
  Com_Printf_806FC90("Sys_LoadDll(%s) found **vmMain** at  %p  \n", name, *entryPoint);
  ((void (__cdecl *)(_DWORD))dllEntry)((int)systemcalls);
  Com_Printf_806FC90("Sys_LoadDll(%s) succeeded!\n", name);
  return libHandle;
  #endif
}

/*
MY OWN Sys_LoadDll!
MY OWN Sys_LoadDll!
MY OWN Sys_LoadDll!
Sys_LoadDll(/home/k_cod1_a/test/game.mp.i386.so)... ok
Sys_LoadDll(game) found **vmMain** at  0xaf02584c
Sys_LoadDll(game) succeeded!
./server.sh: line 44: 12090 Segmentation fault      $cod2 $args
k_cod1_a@euve4482:~$
*/

/*
CM_LoadMap( maps/mp/mp_carentan.bsp, 0 )
Sound alias strings use 93.3 KB; 122.2 KB saved by string sharing
Sys_LoadDll(/home/k_cod1_a/test/game.mp.i386.so)... ok
Sys_LoadDll(game) found **vmMain** at  0xaf06484c
Sys_LoadDll(game) succeeded!
------- Game Initialization -------
gamename: Call of Duty
gamedate: Nov 15 2004
Warning: cvar "dedicated" given initial values: "2" and "0"
----------------------
*/

#endif


/*
client: ec7f7008, tes
client: ec7f7008, tes
client: ec7f7008, tes
client: ec7f7008, tes
client: ec7f7008, tes
client: ec7f7008, tes
client: ec7f7008, tes
*/
int SV_AddServerCommand(int *client /*client_t*/, ...)
{
	va_list ap;
	va_start(ap, client);
	
	char buffer[1024];
	//int n = va_arg(ap, int);
	char *tmp = va_arg(ap, char *);
	
	va_end(ap);
	printf("client: %p, tmp=%s\n", client, "");
	return 0;
}

//http://www.nanobit.net/doxy/quake3/sv__main_8c-source.html
/*

svs_clients_842200C
 ec7cc008 = 80c07cec


Going from CS_FREE to CS_CONNECTED for  (num 0 guid 629770)
client=ec7cc008 message=v ui_3dwaypointtext "1"
client=ec7cc008 message=v ui_deathicontext "1"
Sending heartbeat to cod2master.activision.com
client=ec7cc008 message=v cg_thirdperson "0"
client=ec7cc008 message=v cg_thirdpersonrange "80"
client=ec7cc008 message=v r_blur "0"
client=ec7cc008 message=v cg_laserForceOn "0"
client=ec7cc008 message=v ui_healthbar "1"
client=ec7cc008 message=v bg_viewKickMax "90"
client=ec7cc008 message=v bg_viewKickMin "5"
client=ec7cc008 message=v bg_viewKickRandom "0.4"
client=ec7cc008 message=v bg_viewKickScale "0.2"
client=ec7cc008 message=v ui_map_name "dr_portal"
client=00000000 message=f "Your MUM ^7entered the game"
client=ec7cc008 message=v cg_drawSpectatorMessages "1"
client=ec7cc008 message=v ui_hud_hardcore "1"
client=ec7cc008 message=v player_sprintTime "4"
client=ec7cc008 message=v ui_uav_client "0"
client=ec7cc008 message=v g_scriptMainMenu "team_britishgerman"
client=ec7cc008 message=t 5
client=ec7cc008 message=v cg_thirdperson "0"
client=ec7cc008 message=v cg_thirdpersonrange "80"
client=ec7cc008 message=v r_blur "0"
client=ec7cc008 message=v cg_laserForceOn "0"
client=ec7cc008 message=v ui_healthbar "1"
client=ec7cc008 message=v bg_viewKickMax "90"
client=ec7cc008 message=v bg_viewKickMin "5"
client=ec7cc008 message=v bg_viewKickRandom "0.4"
client=ec7cc008 message=v bg_viewKickScale "0.2"
client=00000000 message=f "Your MUM ^7quit the game"
0:Your MUM EXE_DISCONNECTED
client=00000000 message=J 0
client=ec7cc008 message=w "Your MUM^7 EXE_DISCONNECTED" PB
Sending heartbeat to cod2master.activision.com


Client 0 connecting with 50 challenge ping from 91.97.210.16:-2624
Going from CS_FREE to CS_CONNECTED for  (num 0 guid 629770)
client=ec859008 bla=0x000001 message=v ui_3dwaypointtext "1"
client=ec859008 bla=0x000001 message=v ui_deathicontext "1"
Sending heartbeat to cod2master.activision.com
client=ec859008 bla=0x000001 message=v cg_thirdperson "0"
client=ec859008 bla=0x000001 message=v cg_thirdpersonrange "80"
client=ec859008 bla=0x000001 message=v r_blur "0"
client=ec859008 bla=0x000001 message=v cg_laserForceOn "0"
client=ec859008 bla=0x000001 message=v ui_healthbar "1"
client=ec859008 bla=0x000001 message=v bg_viewKickMax "90"
client=ec859008 bla=0x000001 message=v bg_viewKickMin "5"
client=ec859008 bla=0x000001 message=v bg_viewKickRandom "0.4"
client=ec859008 bla=0x000001 message=v bg_viewKickScale "0.2"
client=ec859008 bla=0x000001 message=v ui_map_name "dr_portal"
client=00000000 bla=   (nil) message=f "Your MUM ^7entered the game"
client=ec859008 bla=0x000001 message=v cg_drawSpectatorMessages "1"
client=ec859008 bla=0x000001 message=v ui_hud_hardcore "1"
client=ec859008 bla=0x000001 message=v player_sprintTime "4"
client=ec859008 bla=0x000001 message=v ui_uav_client "0"
client=ec859008 bla=0x000001 message=v g_scriptMainMenu "team_britishgerman"
client=ec859008 bla=0x000001 message=t 5
(GAME_SPECTATOR)Your MUM^7: test
client=ec859008 bla=   (nil) message=h "(GAME_SPECTATOR)Your MUM^7: ^7test"
client=ec859008 bla=   (nil) message=e "GAME_UNKNOWNCLIENTCOMMANDconsolecommand"
g_password asd
net_port
"net_port" is: "28935^7" default: "28960^7"
  Domain is any integer from 0 to 999999
client=ec859008 bla=0x000001 message=b 1 0 0 0 0 40 0 0
Sending heartbeat to cod2master.activision.com
client=ec859008 bla=0x000001 message=b 1 0 0 0 0 37 0 0
client=ec859008 bla=0x000001 message=b 1 0 0 0 0 37 0 0
client=ec859008 bla=0x000001 message=b 1 0 0 0 0 37 0 0
client=ec859008 bla=0x000001 message=b 1 0 0 0 0 41 0 0

// DIFF = 0x79064

Client 1 connecting with 50 challenge ping from 84.245.1.253:28960
Going from CS_FREE to CS_CONNECTED for  (num 1 guid 707517)
client=ec8d206c bla=0x000001 message=v ui_3dwaypointtext "1"
client=ec8d206c bla=0x000001 message=v ui_deathicontext "1"
Redirecting client 'Un^9Real.JumperZ^2/^7IzNoGoD' to http://killtube.org/deathrun/dr_v4/dr_v4.iwd
client=ec8d206c bla=0x000001 message=v cg_thirdperson "0"
client=ec8d206c bla=0x000001 message=v cg_thirdpersonrange "80"
client=ec8d206c bla=0x000001 message=v r_blur "0"
client=ec8d206c bla=0x000001 message=v cg_laserForceOn "0"
client=ec8d206c bla=0x000001 message=v ui_healthbar "1"
client=ec8d206c bla=0x000001 message=v bg_viewKickMax "90"
client=ec8d206c bla=0x000001 message=v bg_viewKickMin "5"
client=ec8d206c bla=0x000001 message=v bg_viewKickRandom "0.4"
client=ec8d206c bla=0x000001 message=v bg_viewKickScale "0.2"
client=00000000 bla=   (nil) message=f "Un^9Real.JumperZ^2/^7IzNoGoD ^7quit the game"
client=00000000 bla=   (nil) message=e "Un^9Real.JumperZ^2/^7IzNoGoD^7 PC_PATCH_1_1_DOWNLOADDISCONNECTED"
1:Un^9Real.JumperZ^2/^7IzNoGoD PC_PATCH_1_1_DOWNLOADDISCONNECTED
client=00000000 bla=0x000001 message=J 1
client=ec8d206c bla=0x000001 message=w "Un^9Real.JumperZ^2/^7IzNoGoD^7 PC_PATCH_1_1_DOWNLOADDISCONNECTED" PB
Client 2 connecting with 0 challenge ping from 84.245.1.253:28960
84.245.1.253:28960:reconnect
Going from CS_FREE to CS_CONNECTED for  (num 1 guid 707517)
client=ec8d206c bla=0x000001 message=v ui_3dwaypointtext "1"
client=ec8d206c bla=0x000001 message=v ui_deathicontext "1"
Redirecting client 'Un^9Real.JumperZ^2/^7IzNoGoD' to http://killtube.org/deathrun/main/dr_0.iwd
client=ec8d206c bla=0x000001 message=v cg_thirdperson "0"
client=ec8d206c bla=0x000001 message=v cg_thirdpersonrange "80"
client=ec8d206c bla=0x000001 message=v r_blur "0"
client=ec8d206c bla=0x000001 message=v cg_laserForceOn "0"
client=ec8d206c bla=0x000001 message=v ui_healthbar "1"
client=ec8d206c bla=0x000001 message=v bg_viewKickMax "90"
client=ec8d206c bla=0x000001 message=v bg_viewKickMin "5"
client=ec8d206c bla=0x000001 message=v bg_viewKickRandom "0.4"
client=ec8d206c bla=0x000001 message=v bg_viewKickScale "0.2"
client=00000000 bla=   (nil) message=f "Un^9Real.JumperZ^2/^7IzNoGoD ^7quit the game"
client=00000000 bla=   (nil) message=e "Un^9Real.JumperZ^2/^7IzNoGoD^7 PC_PATCH_1_1_DOWNLOADDISCONNECTED"
1:Un^9Real.JumperZ^2/^7IzNoGoD PC_PATCH_1_1_DOWNLOADDISCONNECTED
client=00000000 bla=0x000001 message=J 1
client=ec8d206c bla=0x000001 message=w "Un^9Real.JumperZ^2/^7IzNoGoD^7 PC_PATCH_1_1_DOWNLOADDISCONNECTED" PB
Client 3 connecting with 50 challenge ping from 84.245.1.253:28960
Going from CS_FREE to CS_CONNECTED for  (num 1 guid 707517)
client=ec8d206c bla=0x000001 message=v ui_3dwaypointtext "1"
client=ec8d206c bla=0x000001 message=v ui_deathicontext "1"
client=ec8d206c bla=0x000001 message=v cg_thirdperson "0"
client=ec8d206c bla=0x000001 message=v cg_thirdpersonrange "80"
client=ec8d206c bla=0x000001 message=v r_blur "0"
client=ec8d206c bla=0x000001 message=v cg_laserForceOn "0"
client=ec8d206c bla=0x000001 message=v ui_healthbar "1"
client=ec8d206c bla=0x000001 message=v bg_viewKickMax "90"
client=ec8d206c bla=0x000001 message=v bg_viewKickMin "5"


client=ec8d206c bla=0x000001 message=v bg_viewKickMin "5"
client=ec8d206c bla=0x000001 message=v bg_viewKickRandom "0.4"
client=ec8d206c bla=0x000001 message=v bg_viewKickScale "0.2"
client=ec8d206c bla=0x000001 message=v ui_map_name "dr_portal"
client=00000000 bla=   (nil) message=f "Un^9Real.JumperZ^2/^7IzNoGoD ^7entered the game"
client=ec8d206c bla=0x000001 message=v cg_drawSpectatorMessages "1"
client=ec8d206c bla=0x000001 message=v ui_hud_hardcore "1"
client=ec8d206c bla=0x000001 message=v player_sprintTime "4"
client=ec8d206c bla=0x000001 message=v ui_uav_client "0"
client=ec8d206c bla=0x000001 message=v g_scriptMainMenu "team_britishgerman"
client=ec8d206c bla=0x000001 message=t 5
client=ec8d206c bla=0x000001 message=b 2 0 0 0 0 37 0 0 1 0 48 0 0
client=ec8d206c bla=0x000001 message=b 2 0 0 0 0 35 0 0 1 0 48 0 0
Sending heartbeat to cod2master.activision.com
client=ec8d206c bla=0x000001 message=u
client=ec8d206c bla=0x000001 message=K
client=ec8d206c bla=0x000001 message=u
client=ec8d206c bla=0x000001 message=K
client=ec8d206c bla=0x000001 message=b 2 0 0 0 0 37 0 0 1 0 48 0 0
client=ec8d206c bla=0x000001 message=b 2 0 0 0 0 36 0 0 1 0 48 0 0
client=ec8d206c bla=0x000001 message=b 2 0 0 0 0 40 0 0 1 0 48 0 0
client=ec8d206c bla=0x000001 message=b 2 0 0 0 0 40 0 0 1 0 48 0 0
client=ec8d206c bla=0x000001 message=b 2 0 0 0 0 35 0 0 1 0 48 0 0
Client 4 connecting with 50 challenge ping from 87.208.89.5:28960
Sending heartbeat to cod2master.activision.com
Sending heartbeat to cod2master.activision.com

// THIS IS /+scores by client 0
^A[^[[5~client=ec859008 bla=0x000001 message=b 2 0 0 0 0 35 0 0 1 0 48 0 0



*/
void SV_SendServerCommand(/*client_t*/int *client, int bla, const char *fmt, ...)
{
	char message[4000];
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(message, 4000, fmt, argptr);
	va_end(argptr);
	
	printf("client=%8p bla=%8x message=%s\n", client, bla, message);
}

cHook *hook_parent_of_SV_SendServerCommand;
void parent_of_SV_SendServerCommand(signed int nr, const char *fmt, ...)
{
	char message[4000];
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf(message, 4000, fmt, argptr);
	va_end(argptr);
	
	printf("nr=%8p message=%s\n", nr, message);
	
	hook_parent_of_SV_SendServerCommand->unhook();
	int (*sig)(signed int nr, const char *fmt, ...);
	*(int *)&sig = hook_parent_of_SV_SendServerCommand->from;
	int ret = sig(nr, fmt, argptr);
	hook_parent_of_SV_SendServerCommand->hook();
}



int hook_RemoteCommandTime(void)
{
	unsigned int time = Com_Milliseconds();
	char * rconpass = *(char **)(*(int *)rconPasswordAddress + 8);

	if(!strlen(rconpass) || strcmp(Cmd_Argv(1), rconpass) != 0) 
	{
		unsigned int lasttime = *(int*)remoteCommandLastTimeAddress;
		if (time<(lasttime+1000)) { // limit bad rcon flooding
			return lasttime;
		}
	}

	return time;
}

char * hook_AuthorizeState( int arg )
{
	char * s = Cmd_Argv(arg);

	if ((CvarVariableValue == NULL || CvarVariableValue("sv_cracked") == 1) && strcmp (s, "deny") == 0)
		return "accept";

	return s;
}

int hook_StateCompare(char * state, char * base)
{
    if ((CvarVariableValue == NULL || CvarVariableValue("sv_cracked") == 1) && strcmp (state, "deny") == 0) // when sv_cracked 1 and deny state
    {
        if(strcmp(base, "accept") == 0) // when compare value is accept return true
            return 0;
        else
            return 1; // when compare to value is deny return false
    }

    return strcmp(state, base);
}

int hook_BadKeyCompare(char * s1, char * s2)
{
	if (CvarVariableValue == NULL || CvarVariableValue("sv_cracked") == 1)
    {
        return 0; // when sv_cracked 1 no bad keys
    }

    return strcmp(s1, s2);
}

void hook_SV_BeginDownload_f( int a1 ) {
	char * file = Cmd_Argv(1);
	int len;

	if((len = strlen(file)) > 3 && !strcmp(file + len - 4, ".iwd"))
		SV_BeginDownload_f(a1);
	else
		printf("Invalid download attempt: %s\n", file);
}

int hook_ClientUserinfoChanged(int clientNum)
{
	if ( ! codecallback_userinfochanged)
	{
		return changeClientUserinfo(clientNum);
	}
	
	stackPushInt(clientNum); // one parameter is required
	
	// todo: G_ENTITY(clientNum)
	#if COD_VERSION == COD2_1_0 // search '\\name\\badinfo'
		short ret = codscript_call_callback_entity(/*gentity*/0x08665480 + 560 * clientNum, codecallback_userinfochanged, 1);
	#elif COD_VERSION == COD2_1_2
		short ret = codscript_call_callback_entity(/*gentity*/0x08679380 + 560 * clientNum, codecallback_userinfochanged, 1);
	#elif COD_VERSION == COD2_1_3
		short ret = codscript_call_callback_entity(/*gentity*/0x08716400 + 560 * clientNum, codecallback_userinfochanged, 1);
	#elif COD_VERSION == COD4_1_7
		short ret = codscript_call_callback_entity(/*gentity*/0x0841F260 + 628 * clientNum, codecallback_userinfochanged, 1);
	#elif COD_VERSION == COD4_1_7_L
		short ret = codscript_call_callback_entity(/*gentity*/0x0841FFE0 + 628 * clientNum, codecallback_userinfochanged, 1);
	#else
		#warning short ret = codscript_call_callback_entity(NULL, codecallback_userinfochanged, 1);
		short ret = codscript_call_callback_entity(NULL, codecallback_userinfochanged, 1);
	#endif

	//printf("codecallback_playercommand=%.8x ret=%i\n", codecallback_userinfochanged, ret);
	codscript_callback_finish(ret);
	//printf("after codscript_callback_finish\n");
	
	return 0;
}


char * hook_beginDownloadCopy(char *a1, char *a2, int a3) {
	typedef char* (*beginDownloadCopy_t)(char *a1, char *a2, int a3);

	#if COD_VERSION == COD4_1_7
		beginDownloadCopy_t beginDownloadCopy = (beginDownloadCopy_t)0x81A9E1C;
	#elif COD_VERSION == COD4_1_7_L
		beginDownloadCopy_t beginDownloadCopy = (beginDownloadCopy_t)0x81AA54C;
	#else
		beginDownloadCopy_t beginDownloadCopy = (beginDownloadCopy_t)NULL;
	#endif
	
	int len = strlen(a2);
	
	if(len > 3 && !strcmp(a2 + len - 4, ".iwd") || len > 2 && !strcmp(a2 + len - 3, ".ff"))
		return beginDownloadCopy(a1, a2, a3);
	else
	{
		printf("Invalid download attempt: %s\n", a2);
		return beginDownloadCopy(a1, (char*)"fail.iwd", a3);
	}
}

void hook_SV_WriteDownloadToClient(int cl, int msg)
{
	#if COD_VERSION == COD2_1_0
		int offset = 452008;
	#else
		int offset = 452280;
	#endif

	if((*(int*)(cl + 134248)) && (*(int*)(cl+offset)**(int*)(cl+offset+4)/2048000 > 6))
		SV_DropClient(cl, "broken download");
	else
		SV_WriteDownloadToClient(cl, msg);
}

void manymaps_prepare(char *mapname, int read);
int hook_findMap(const char *qpath, void **buffer)
{
	int read = FS_ReadFile(qpath, buffer);
	manymaps_prepare(Cmd_Argv(1), read);
	
	if(read != -1)
		return read;
	else
		return FS_ReadFile(qpath, buffer);
}

/*
static int BG_AnimationIndexForString( char *string, animModelInfo_t* animModelInfo ) {
	int i, hash;
	animation_t *anim;

	hash = BG_StringHashValue( string );

	for ( i = 0; i < animModelInfo->numAnimations; i++ ) {
		anim = animModelInfo->animations[i];
		if ( ( hash == anim->nameHash ) && !Q_stricmp( string, anim->name ) ) {
			// found a match
			return i;
		}
	}
	// no match found
	BG_AnimParseError( "BG_AnimationIndexForString: unknown index '%s' for animation group '%s'", string, animModelInfo->animationGroup );
	return -1;  // shutup compiler
}
*/



int BG_AnimationIndexForString(char *string)
{
	int i, hash, tmp;
	int anim;
	
	int (*BG_StringHashValue)(char *fname);
	*(int *)&BG_StringHashValue = 0x080D6B9C;
	
	int (*sub_806DEFE)(char *multiplayer, char *animname, int a3, int a4);
	*(int *)&sub_806DEFE = 0x0806DEFE;
	
	int debug = 0;
	
	if (strstr(string, "reload"))
		debug = 1;
	
	if (debug) printf("\narg 8571428:%d BG_AnimationIndexForString1(string=%s) ", INT(0x08571428), string);
	
	
	if (INT(0x08571428))
	{
		hash = BG_StringHashValue(string);
		//printf("Hash: %d ", hash);
		i = 0;
		for (i=INT(0x08571428); ; i+=72)
		{
			if (i >= INT(0x0857142C))
			{
				tmp = INT(0x08571428) + 72 * INT(INT(0x0857142C));
				sub_806DEFE((char *)"multiplayer", string, tmp, INT(0x0856E3A0 + 0xB3BE8));
				strcpy((char *)(tmp + 8), string);
				INT(tmp + 4) = hash;
				INT(INT(0x0857142C))++;
				if (debug) printf("tmp:%.8x , %.8x", (tmp + 0), INT(tmp + 4));
				return INT(0x0857142C) - 1;
			}
			if (hash == INT(i+4) && !strcmp(string, (char *)(i + 8)))
			{
				if (debug) printf("Found anim '%s', i=%d hash=%d", string, i, hash);
				break;
			}
			i++;
		}
		if (debug) printf("NOT Found anim '%s', i=%d hash=%d", string, i, hash);
		
		return i;
	} else {
		hash = BG_StringHashValue(string);
		i = 0;
		for (anim=INT(0x0856E3A4); ; anim += 0x60)
		{
			if (i >= INT(INT(0x0856E3A4) + 0xC000)) {
				if (debug) printf("BG_AnimationIndexForString: unknown player animation '%s'", string);
				return -1;
			}
			if (hash == INT(anim + 0x4C) && !strcmp(string, (char *)anim))
			{
				if (debug) printf("Found anim '%s', i=%d hash=%d", string, i, hash);
				break;
			}
			i++;
		}
		
		/*
			********************
			ERROR: Player anim 'pb_prone_death_quickdeath' has no children
			********************
		*/
		//return i - 1;
		
		return i;
	}
	
	
	//for (i=0; anim=INT(0x0856E3A4); i<10; anim+=0x60, i++)
	{
		/*if (i >= INT(0x0856E3A4 + 0xC000)) {
			printf("BG_AnimationIndexForString: unknown player animation '%s'\n", string);
			return -1;
		}*/
		
		//printf("anim %d: %s\n", i, anim);
		//printf("%s ", anim);
	}
	
	
	for (i=0; i<512; i++)
	{
		// points to: 085b2940
		anim = INT(0x0856E3A4 + i * 96);
		//printf("%s ", anim);
	}
	
	//printf("address:%.8p %.8p %s %s %s\n", INT(0x0856E3A4), INT(INT(0x0856E3A4)), INT(0x0856E3A4), INT(0x0856E3A4 + 96), INT(0x0856E3A4 + 2 * 96));
	
	
	/*
	139102080 = 0x84A8780 = 80874a84
8571428:139102080 BG_AnimationIndexForString1(string=pb_crouch_pain_holdStomach)
8571428:139102080 BG_AnimationIndexForString1(string=pb_crouch_pain_holdStomach)

	*/
	
	printf("\n");
	
	return -1; // When only returning 0, on client, error: Player animation index out of range (8): -513
}

int BG_PlayAnimName(int ps, const char *animName, int bodyPart, int setTimer, int isContinue, int force)
{
	printf("BG_PlayAnimName ps=%.8x animName=%s bodePart=%.8x setTimer=%d isContinue=%d force=%d\n",
		ps, animName, bodyPart, setTimer, isContinue, force
	);
	return 0;
}

int BG_PlayAnim(int ps, int animIndex, int bodyPart, int is_0, int setTimer, int isContinue, int force)
{
	if (force)
		printf("BG_PlayAnim: ps=%.8x animIndex=%d bodyPart=%.8x is_0:%.8x setTimer=%.8x, isContinue=%.8x force=%.8x\n",
			ps, animIndex, bodyPart, is_0, setTimer, isContinue, force
		);
	return 0;
}

int FS_AddGameDirectory(char *path, char *dir)
{
	printf("FS_AddGameDirectory(char *path=%s, char *dir=%s)\n", path, dir);
	return 1;
}

int FS_LoadIWD(char *a, char *b)
{
	printf("FS_LoadIWD(char *a=%s, char *b=%s)\n", a, b);
	return 1;
}

static int size_all = 0;
static int i = 0;
cHook *hook_MSG_WriteBigString;
void MSG_WriteBigString(int *MSG, char *s)
{

	int len;

	len = strlen(s);
	printf("i=%d size_all=%d len=%d MSG=%p %s\n", i, size_all, len, MSG, s);

	size_all += len;
	i++;
	
	hook_MSG_WriteBigString->unhook();
	
	void (*sig)(int *MSG, char *s);
	*(int *)&sig = 0x0806825E;
	sig(MSG, s);
	
	hook_MSG_WriteBigString->hook();
	
}


void manymaps_prepare(char *mapname, int read)
{
	char *sv_iwdNames = Cvar_VariableString("sv_iwdNames");
	char library_path[512];
	if(Cvar_VariableString("fs_library")[0] == '\0')
		snprintf(library_path, sizeof(library_path), "%s/%s/Library", Cvar_VariableString("fs_homepath"), Cvar_VariableString("fs_game"));
	else
		strncpy(library_path, Cvar_VariableString("fs_library"), sizeof(library_path));
	printf("manymaps> map=%s sv_iwdNames: %s\n", mapname, sv_iwdNames);
	char *tok;
	tok = strtok(sv_iwdNames, " ");
	while (tok)
	{
		if(strncmp(tok, "zzz_", 4) == 0)
		{
			tok = strtok(NULL, " ");
			continue;
		}
		int i = 0;
		while(tok[i] != '\0')
			i++;
		if(i >= 5 && strcmp(&tok[i - 5], "Empty") == 0)
		{
			tok = strtok(NULL, " ");
			continue;
		}
		char file[512];
		snprintf(file, sizeof(file), "%s/%s.iwd", library_path, tok);
		int exists = access(file, F_OK) != -1;
		printf("manymaps> exists in /Library=%d iwd=%s \n", exists, tok);
		if (exists) {
			char fileDelete[512];
			snprintf(fileDelete, sizeof(fileDelete), "%s/%s/%s.iwd", Cvar_VariableString("fs_homepath"), Cvar_VariableString("fs_game"), tok);
			printf("manymaps> REMOVE MANYMAP: %s result of unlink: %d\n", fileDelete, unlink(fileDelete));
		}
		tok = strtok(NULL, " ");
	}
	
	char src[512], dst[512];
	snprintf(src, sizeof(src), "%s/%s.iwd", library_path, mapname);
	snprintf(dst, sizeof(dst), "%s/%s/%s.iwd", Cvar_VariableString("fs_homepath"), Cvar_VariableString("fs_game"), mapname);
	printf("manymaps> link src=%s dst=%s\n", src, dst);
	if (access(src, F_OK) != -1) {
		int link_success = link(src, dst) == 0;
		printf("manymaps> LINK: %s\n", link_success?"success":"failed (probably already exists)");
		if(read == -1) // FS_LoadDir is needed when empty.iwd is missing (then .d3dbsp isn't referenced anywhere)
			FS_LoadDir(Cvar_VariableString("fs_homepath"), Cvar_VariableString("fs_game"));
	}
}

// works, but not needed anymore for manymaps_prepare(mapname) (re-use Mitch's findmap-hook instead)
cHook *hook_cmd_map;
int cmd_map()
{
	//manymaps_prepare(Cmd_Argv(1)); // call with mapname
	
	hook_cmd_map->unhook();
	int (*sig)();
	*(int *)&sig = hook_cmd_map->from;
	int ret = sig();
	hook_cmd_map->hook();
	
	//printf("value of map change: %d\n", ret);
	
	return ret;
}

int hook_dummytrue(const char *src)
{
	return 1;
}

#define TOSTRING2(str) #str
#define TOSTRING1(str) TOSTRING2(str) // else there is written "__LINE__"
class cCallOfDuty2Pro
{
	public:
	cCallOfDuty2Pro()
	{
	
		setbuf(stdout, NULL); // otherwise the printf()'s are printed at crash/end

		#if COD_VERSION == COD2_1_0
			printf("> [INFO] Compiled for: CoD2 1.0\n");
		#elif COD_VERSION == COD2_1_2
			printf("> [INFO] Compiled for: CoD2 1.2\n");
		#elif COD_VERSION == COD2_1_3
			printf("> [INFO] Compiled for: CoD2 1.3\n");
		#else
			printf("> [WARNING] Compiled for: %s\n", TOSTRING1(COD_VERSION));
		#endif
		
		printf("Compiled: " __DATE__ " " __TIME__ "\n");

		
		#if COD_VERSION == COD4_1_7
		
			//08048000-0817d000 rwxp 00000000 00:8e 8716292                            /root/helper/game_cod4/cod4_1_7-bin
			//0817d000-0826a000 r-xp 00135000 00:8e 8716292                            /root/helper/game_cod4/cod4_1_7-bin
			//0826a000-08274000 rw-p 00221000 00:8e 8716292                            /root/helper/game_cod4/cod4_1_7-bin

			mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);
			mprotect((void *)0x0817d000, 0xED000, PROT_READ | PROT_WRITE | PROT_EXEC);
			mprotect((void *)0x0826a000, 0xA000, PROT_READ | PROT_WRITE | PROT_EXEC);
		#else
		
			// allow to write in executable memory
			mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);
		#endif
		
		#if COD_VERSION == COD1_1_5
			//#error WRONG!
			printf("CoD1 Lib Loaded!\n");
			
			//cracking_hook_function(0x080D3DAD, (int)Sys_LoadDll_80D3DAD);
			
			hook_Sys_LoadDll.hook();
			
			return;
		#endif
		
		#if COD_VERSION == COD4_1_7 && 0
		/*
			FILE * pFile;
			long lSize;
			char * buffer;
			size_t result;
			pFile = fopen ( "/proc/self/maps" , "r" );
			fseek (pFile , 0 , SEEK_END); lSize = ftell (pFile); rewind (pFile);
			buffer = (char*) malloc (sizeof(char)*lSize);
			result = fread (buffer,1,lSize,pFile);
			printf("%d ########## %s #############", lSize, buffer);
			fclose (pFile);
			free (buffer);
		*/
			char buf[512];
			char flags[4];
			void *low, *high;
			FILE *fp;

			fp = fopen ("/proc/self/maps", "r");
			if (fp)
			{
				while (fgets (buf, sizeof(buf), fp))
				{
					if (sscanf (buf, "%p-%p %4c", &low, &high, flags) == 3)
					{
						//printf("%08x %08x %08x\n", low, high, flags);
						printf(buf);
					}
				}
			}
			return;
		#endif
		
		// SET gsc.closer TO c.closer
		#if COD_VERSION == COD2_1_2
			int *addressToCloserPointer = (int *)0x081872D0;
		#elif COD_VERSION == COD2_1_3
			int *addressToCloserPointer = (int *)0x081882F0;
		#elif COD_VERSION == COD4_1_7
			int *addressToCloserPointer = (int *)0x0826D66C;
		#elif COD_VERSION == COD4_1_7_L
			int *addressToCloserPointer = (int *)0x0826E40C;
		#elif COD_VERSION == COD2_1_0
			int *addressToCloserPointer = (int *)0x08167E70;
		#else
			#warning int *addressToCloserPointer = NULL;
			int *addressToCloserPointer = NULL;
		#endif
		printf("> [INFO] value of closer=%.8x\n", *addressToCloserPointer);
		*addressToCloserPointer = (int) cdecl_injected_closer/*_stack_debug*/;
		//printf("after\n");

		/*
		printf("> [INFO] recvfrom=%08x\n", dlsym(NULL, "recvfrom"));
		int rf = (int)dlsym(NULL, "recvfrom");
		for (int i=0; i<5; i++)
			printf("%02x ", *(unsigned char*)(rf+i));
		printf("\n");
		printf("> [INFO] *recvfrom=%08x\n", *(int *)dlsym(NULL, "recvfrom"));
		*/
		
		//init_native_interface(); // inits functions and globals in the executable

		//hideargs_thread();

		#if COD_VERSION == COD2_1_0
			int * addressToDownloadPointer = (int *)0x0815D584;
		#elif COD_VERSION == COD2_1_2
			int * addressToDownloadPointer = (int *)0x0817C9E4;
		#elif COD_VERSION == COD2_1_3
			int * addressToDownloadPointer = (int *)0x0817DA04;
		#else
			#warning int *addressToDownloadPointer = NULL;
			int *addressToDownloadPointer = NULL;
		#endif

		#if COD_VERSION == COD2_1_0 || COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
			printf("> [INFO] value of download=%.8x\n", *addressToDownloadPointer);
			SV_BeginDownload_f = (SV_BeginDownload_f_t)*addressToDownloadPointer;
			*addressToDownloadPointer = (int)hook_SV_BeginDownload_f;
		#endif
		
		#if COD_VERSION == COD4_1_7
			cracking_hook_function(0x0804AB6C, (int)hook_recvfrom);
			cracking_hook_call(0x081721AE, (int)hook_beginDownloadCopy);
			cracking_hook_call(0x0816FE8C, (int)hook_StateCompare);
			cracking_hook_call(0x08170102, (int)hook_StateCompare);
			cracking_hook_call(0x0816FFB8, (int)hook_BadKeyCompare);
		#endif
		
		#if COD_VERSION == COD4_1_7_L
			cracking_hook_call(0x0817225E, (int)hook_beginDownloadCopy);
			cracking_hook_call(0x0816FF42, (int)hook_StateCompare);
			cracking_hook_call(0x081701D6, (int)hook_StateCompare);
			cracking_hook_call(0x0817006E, (int)hook_BadKeyCompare);
		#endif
		
		// NEEDED FOR ZOMBOTS/BOTZOMS???
		// lol, i dont know why, but this made sniper/rifle-shots stick to 90 damage, very annoying
		if (0) cracking_hook_function(0x08101C58, (int)cdecl_cod2_player_damage_new); // SET binary.damage TO c.damage
		
		if (0) cracking_hook_function(0x08078FB2, (int)cdecl_calc_hash_of_string);
		if (0) cracking_hook_function(0x0807F840, (int)cdecl_sub_807F840); // radiant keys
		if (0) cracking_hook_function(0x0807D288, (int)cdecl_gsc_cast_to_bool);
		if (0) cracking_hook_function(0x0807C6F8, (int)cdecl_gsc_set_field_of_struct);
		if (0) cracking_hook_function(0x0807AB64, (int)gsc_new_variable_807AB64);
		if (0) cracking_hook_function(0x0805B894, (int)trace_calc_fraction_805B894); // BSP HOOK for fraction
		
		if (0)
		{
			int (*sub_81100AC)(const char *file, const char *function, int isOne);

			*(int *)&sub_81100AC = 0x081100AC;
			
			printf("before func_ref!\n");
			int func_ref = sub_81100AC("maps/mp/std/test", "helloworld", 1);
			printf("after func_ref!\n");
			
			/*
				before func_ref!
				********************************
				0 subtree has 0 * 1 = 0 free buckets
				1 subtree has 0 * 2 = 0 free buckets
				2 subtree has 0 * 4 = 0 free buckets
				3 subtree has 0 * 8 = 0 free buckets
				4 subtree has 0 * 16 = 0 free buckets
				5 subtree has 0 * 32 = 0 free buckets
				6 subtree has 0 * 64 = 0 free buckets
				7 subtree has 0 * 128 = 0 free buckets
				8 subtree has 0 * 256 = 0 free buckets
				9 subtree has 0 * 512 = 0 free buckets
				10 subtree has 0 * 1024 = 0 free buckets
				11 subtree has 0 * 2048 = 0 free buckets
				12 subtree has 0 * 4096 = 0 free buckets
				13 subtree has 0 * 8192 = 0 free buckets
				14 subtree has 0 * 16384 = 0 free buckets
				15 subtree has 0 * 32768 = 0 free buckets
				16 subtree has 0 * 65536 = 0 free buckets
				********************************
				********************************
				total memory alloc buckets: 0 (0 instances)
				total memory free buckets: 65535
				********************************
				MT_AllocIndex: failed memory allocation of 21 bytes for script usage
				********************************
				********************************
				********************************
				./start_1.2.sh: line 4: 25636 Segmentation fault
			*/
		}
		
		// BSP HOOK for all(?) at least this function calls the fraction-function
		// hookFunction(0x0809D884, (int)sub_809D884);
		
		
		/*
			CRACKED 1.3
			We just passed a private version of the CoD2 v1.3 linux server to ul.gamecopyworld.com which does not have the 30min wait intervall. You can play immediatly after server startup.
			Until the fix gets published on GF, You may edit Your cod_lnxded at address 0x8D678 and change the bytes 0C75 to 9090.
			Happy gaming, Mokolator. 
		*/
		
		#if COD_VERSION == COD2_1_0 || COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
			Cmd_AddCommand("stupidTestFunction", stupidTestFunction);
		#endif
		
		//printf("checkpoint 2\n");
		
		
		#if COMPILE_DEBUG_SERVER == 1
		//startServerAsThread();
		#endif
	
		/*
			how to find the address?
				- press g in IDA, enter "sendto"
				- doubleclick on CODE XREF: _sendto
				- done, current position is wanted address
		*/
		
		#if COD_VERSION == COD2_1_0
			cracking_hook_function(0x08049CD0, (int)hook_recvfrom);
		#elif COD_VERSION == COD2_1_2
			//cracking_hook_function(0x0804A0B4, (int)hook_sendto);
			//#error asd
			printf("cracking_hook_function(0x08049E64, (int)hook_recvfrom);\n");
			cracking_hook_function(0x08049E64, (int)hook_recvfrom);
			//cracking_hook_function(0x0804A154, (int)hook_pthread_mutex_lock);
			//cracking_hook_function(0x0804A5A4, (int)hook_pthread_mutex_unlock);
			//cracking_hook_function(0x0804A254, (int)hook_pthread_self);
			//cracking_hook_function(0x0804A084, (int)hook_fread);
			
			//cracking_hook_function(0x080D6C8C, (int)BG_AnimationIndexForString);
			//cracking_hook_function(0x080D915C, (int)BG_PlayAnimName);
			//cracking_hook_function(0x080D8F92, (int)BG_PlayAnim);
			
			//cracking_hook_function(0x080A28CC, (int)FS_AddGameDirectory);
			//cracking_hook_function(0x080A22D8, (int)FS_LoadIWD);
		#endif
		
		#if COD_VERSION == COD2_1_3
			cracking_hook_function(0x08049E64, (int)hook_recvfrom); // same address then cod2 1.2
		#endif
		
		//cracking_hook_function(0x08078EE6, (int)hook_str2hash_8078EE6);
		
		// cod2 functions
		//cracking_hook_function(0x080F6D5A, (int)hook_player_eject);
		
		#if COD_VERSION == COD2_1_0
			if (0)
				cracking_hook_function(0x08092D5C, (int)SV_AddServerCommand);
			if (0)
				cracking_hook_function(0x0809301C, (int)SV_SendServerCommand);

			cracking_hook_call(0x0808F134, (int)hook_ClientUserinfoChanged);
			cracking_hook_call(0x0807059F, (int)Scr_GetCustomFunction);
			cracking_hook_call(0x080707C3, (int)Scr_GetCustomMethod);
			cracking_hook_call(0x08098CD0, (int)hook_SV_WriteDownloadToClient);
			cracking_hook_call(0x080DFF66, (int)hook_player_setmovespeed);
		#elif COD_VERSION == COD2_1_2
			if (0)
				cracking_hook_function(0x08094698, (int)SV_AddServerCommand);
			if (0)
				cracking_hook_function(0x08094958, (int)SV_SendServerCommand);
			if (0) {
				hook_parent_of_SV_SendServerCommand = new cHook(0x0809267C, (int)parent_of_SV_SendServerCommand);
				hook_parent_of_SV_SendServerCommand->hook();
			}
			
			//cracking_hook_function((int)codscript_load_label, (int)hook_codscript_load_label_8075DEA);
			cracking_hook_call(0x080909BE, (int)hook_ClientUserinfoChanged);
			cracking_hook_call(0x08070B1B, (int)Scr_GetCustomFunction);
			cracking_hook_call(0x08070D3F, (int)Scr_GetCustomMethod);
			cracking_hook_call(0x080E2546, (int)hook_player_setmovespeed);
			hook_MSG_WriteBigString = new cHook(0x0806825E, (int)MSG_WriteBigString);
			//hook_MSG_WriteBigString->hook();
			
			//hook_cmd_map = new cHook(0x0808BC7A, (int)cmd_map);
			//hook_cmd_map->hook();
			cracking_hook_call(0x0809AD68, (int)hook_SV_WriteDownloadToClient);
		#elif COD_VERSION == COD2_1_3
			if (0)
				cracking_hook_function(0x08094750, (int)SV_AddServerCommand);
			if (0)
				cracking_hook_function(0x080AC5D8, (int)SV_SendServerCommand);
			if (0) {
				hook_parent_of_SV_SendServerCommand = new cHook(0x08092780, (int)parent_of_SV_SendServerCommand);
				hook_parent_of_SV_SendServerCommand->hook();
			}
			
			cracking_hook_call(0x08090A52, (int)hook_ClientUserinfoChanged);
			cracking_hook_call(0x08070BE7, (int)Scr_GetCustomFunction);
			cracking_hook_call(0x08070E0B, (int)Scr_GetCustomMethod);
			cracking_hook_call(0x080E268A, (int)hook_player_setmovespeed);
			cracking_hook_call(0x080F7803, (int)hook_player_g_speed);
		#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
			extern cHook *hook_Scr_GetFunction;
			extern cHook *hook_Scr_GetMethod;
			hook_Scr_GetFunction = new cHook(0x080bd238, (int)Scr_GetCustomFunction);
			hook_Scr_GetMethod = new cHook(0x080bfef4, (int)Scr_GetCustomMethod);
			hook_Scr_GetFunction->hook();
			hook_Scr_GetMethod->hook();
		#endif

		#if COD_VERSION >= COD2_1_0
			cracking_hook_function((int)gametype_scripts, (int)hook_codscript_gametype_scripts);
			cracking_hook_call(hook_ClientCommand_call, (int)hook_ClientCommand);
		#endif
			
		#if COD_VERSION == COD2_1_0 || COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
			cracking_hook_call(hook_AuthorizeState_call, (int)hook_AuthorizeState);
			cracking_hook_call(hook_findMap_call, (int)hook_findMap);
			cracking_hook_call(hook_RemoteCommandTime_call, (int)hook_RemoteCommandTime);
		#endif
		
		#ifdef IS_JAVA_ENABLED
		embed_java();
		#endif
		
		printf("> [PLUGIN LOADED]\n");
	}
	
	~cCallOfDuty2Pro()
	{
		printf("> [PLUGIN UNLOADED]\n");
		gsc_utils_free();
	}
};

cCallOfDuty2Pro *pro;

// lol, single again: because it got loaded two times now
// both now: able to load with wrapper AND directly
// IMPORTANT: file needs "lib" infront of name, otherwise it wont be loaded
#if 1
	extern "C" void __attribute__ ((constructor)) lib_load(void) // will be called when LD_PRELOAD is referencing this .so
	{
		pro = new cCallOfDuty2Pro;
	}
	extern "C" void __attribute__ ((destructor)) lib_unload(void)
	{
		delete pro;
	}
#else

	#pragma GCC visibility pop

	// STRANGE, BUT WORKS
	// normally i had thought of "public", but thats not available
	// so i compile this file without -fvisibility=hidden and just make them here "default"

	extern "C" __attribute__((__visibility__("default"))) void a/*lpha*/(void) // will be called in wrapper_libcod2.so
	{
		pro = new cCallOfDuty2Pro;
	}

	// damn, tcc had also a symbol called "o"... so now its a big O... it wont find it i guess.....
	// added "z" for testing if tcc still crashes (yea it still crashed lol)
	extern "C" __attribute__((__visibility__("default"))) void Oz/*mega*/(void)
	{
		delete pro;
	}

	#pragma GCC visibility push(hidden)

#endif

#if 0

/*
	IDEA IDEA IDEA making the tcp-server part of UDP with those hooks and 0xffffffff as indicator IDEA IDEA IDEA 
*/


#define _GNU_SOURCE // enable RTLD_NEXT extension of gnu over posix

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <poll.h>

#include <linux/net.h>

#include <arpa/inet.h> // inet_ntop

#if 1
static void do_dragnet_init(void)
    if ((c = getenv("DRAGNET_BPS")))
        MaxBytesPerSec = strtoul(c, NULL, 10);
#endif

#include <dlfcn.h>
#include <netinet/in.h> // sockaddr_in, else "incomplete type" error

// thought this is needed... but no
#if 0
extern "C"
{
	ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
	ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
}
#endif
typedef ssize_t (*type_sendto)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

//ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
ssize_t sendto_2(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
{
	//static ssize_t (*libc_sendto)(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) = NULL;
	static type_sendto libc_sendto = NULL;

	if (libc_sendto == NULL)
	{
		void *address = dlsym(RTLD_NEXT, "sendto");
		libc_sendto = (type_sendto) address;
	}
	
    ssize_t st;
	
	st = libc_sendto(sockfd, buf, len, flags, dest_addr, addrlen);
	
	
	if (0) // if i want to disable sendto-spam
		return st;
	
    /*dn_log("    >> intercepted sendto(%d, %p, %zu, %d, %p, %lu)\n",
        sockfd, buf, len, flags, dest_addr, (unsigned long)addrlen);
    st = syscall(SYS_SENDTO, sockfd, buf, len, flags, dest_addr, addrlen);*/
	
    //printf("sendto()ed %zu bytes: %.*s\n", st, (int)st, (const char *)buf);
	
		struct sockaddr_in *ipv4sockdata = (struct sockaddr_in *) dest_addr;
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(ipv4sockdata->sin_addr), ip, INET_ADDRSTRLEN);
		printf("SEND %s:%d ", ip, ipv4sockdata->sin_port);
	
		int i;
		for (i=0; i<st && i<100; i++)
		{
			char currentchar = ((char *)buf)[i];
			
			if (currentchar >= 32 && currentchar <= 126)
				printf("%c", currentchar);
			else {
				printf("\\x%02X", currentchar&0xff);
				//printf(".");
			}
		}
		printf("\n");
	
    return st;
}

typedef ssize_t (*type_recvfrom)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
// len is in cod2 0x20000, real packet length is in "rf"
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
{
	static ssize_t (*libc_recvfrom)(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) = NULL;
	
	if (libc_recvfrom == NULL)
	{
		libc_recvfrom = (type_recvfrom) dlsym(RTLD_NEXT, "recvfrom");
	}
	
    ssize_t rf;

	rf = libc_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);	
	
	
	
	if (rf == -1) // just debug when the function was a success
	{
		// errno = WOULDBLOCK
		//printf("> recvfrom(fd=%d, buf=%s, len=%zu, flags=%d, src_addr=%p, addrlen=%p)\n", sockfd, buf, len, flags, src_addr, addrlen);
		return rf;
	}
		
		
	/*
		if someone spoofed a message from our server to another server with:
			- getstatus
			- getinfo xxx
		, the server will respond with:
			- statusResponse
			- infoResponse
		we dont want those packets inspected by our server (they are just crap,
		and i dont know what they could to - like lag)
		so just return -1, and cod2 thinks "no packet received"
		TODO: test it with packetspoofer
		
		a -1 stands for: SOCKET_ERROR==-1
		the server prints then a message like:
		NET_GetPacket: No such file or directory from 95.33.186.240:2805
		NET_GetPacket: No such file or directory from 92.225.223.210:28960
		NET_GetPacket: No such file or directory from 88.190.17.215:-24716
		
		from Enemy Territory:
		Com_printf("NET_GetPacker: %s\n", NET_ErrorString());
		
		well, i set it to "0" now... no error like -1
		
	*/
	
	// shortcut those long parsing stuff (still "hits" the gameserver)
	// but when i ban their ips, ppl can spoof such a packet for a player
	if (
		(memcmp("\xff\xff\xff\xffstatusResponse", buf, 18) == 0) ||
		(memcmp("\xff\xff\xff\xffinfoResponse", buf, 16) == 0)
	)
	{
		((char *)buf)[4] = 'f';
		((char *)buf)[5] = 'o';
		((char *)buf)[6] = 'o';
		((char *)buf)[7] = '\0';
		return 8;
	}
		
	// gameserver dont answer one these also
	// my python-twisted-bot shall answer those
	// then the gameserver isnt busy with it
	// AND i can detect drdos (urbanterror.info)
	// and i can easily find non-spoofer-flooders and ban them with iptables
	// also i can list the ips with php and set "custom messages" for:
	//  -rcon -> fake access
	//  -status -> fake lag
	
	/*
		still need to think what the mysql-tables is needing...
		 - ip:port
		 - time
		 - rcon/getstatus/getinfo -> rcon with pass
	*/
	#if 0
	if (
		(memcmp("\xff\xff\xff\xffgetstatus", buf, 13) == 0) ||
		(memcmp("\xff\xff\xff\xffgetinfo", buf, 11) == 0) ||
		(memcmp("\xff\xff\xff\xffrcon", buf, 8) == 0)
	)
	{
		printf("drop get\n");
		// DO NOT MANIPULATE THE \xff\xff\xff\xff, the server needs to recognise a "connectionless packet",
		// but it shoudlt have any sense (the server just shall dispose it to prevent answer)
		((char *)buf)[4] = 'f'; // let the first 4 0xff
		((char *)buf)[5] = 'o'; // let the first 4 0xff
		((char *)buf)[6] = 'o'; // let the first 4 0xff
		((char *)buf)[7] = '\0'; // let the first 4 0xff
		/* http://svn.icculus.org/quake3/branches/1.34/code/unix/unix_net.c?limit_changes=100&view=markup */
		/* lets fake a good ending, so we dont get in trouble */
		return 8/*rf*/; // look server, its a connectionless packet 0xffffffff"foo\n" ;P
	}
	
	// dont debug packets anymore :)
	return rf;
	#endif
	
	// ok, here lets just debug the connectionless packets
	if (memcmp("\xff\xff\xff\xff", buf, 4) != 0)
		return rf;
	
	//printf("> recvfrom(%d, %p, %zu, %d, %p, %p)\n", sockfd, buf, len, flags, src_addr, addrlen);
	
	int max = 50;
	
	struct sockaddr_in *ipv4sockdata = (struct sockaddr_in *) src_addr;
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ipv4sockdata->sin_addr), ip, INET_ADDRSTRLEN);
	printf("RECV %s:%d ", ip, ipv4sockdata->sin_port);
	
	int i;
	for (i=0; i<rf && i<100; i++)
	{
		char currentchar = ((char *)buf)[i];
		
		if (currentchar >= 32 && currentchar <= 126)
			printf("%c", currentchar);
		else {
			printf("\\x%02X", currentchar&0xff);
			//printf(".");
		}
	}
	printf("\n");

    return rf;
}

#pragma GCC visibility pop

#endif
