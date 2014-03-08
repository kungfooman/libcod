#ifndef _GSC_HPP_
#define _GSC_HPP_

#ifdef __cplusplus
extern "C" {
#endif

// needs to be over the includes
// example of versioning:
/*
		#if COD2_VERSION == COD2_VERSION_1_2
		int *addressToCloserPointer = (int *)0x081872D0;
		#endif
		#if COD2_VERSION == COD2_VERSION_1_3
		int *addressToCloserPointer = (int *)0x081882F0;
		#endif
*/

#define COD2_VERSION_1_0 0 // depricated
#define COD2_VERSION_1_2 2 // depricated
#define COD2_VERSION_1_3 3 // depricated

#define COD1_1_5 115
#define COD2_1_0 210
#define COD2_1_2 212
#define COD2_1_3 213
#define COD4_1_7 417

//#define COD2_VERSION COD2_VERSION_1_2

// gonna define that per command-line now, to build from script every version + select the correct with inbuilt if()
//#define COD2_VERSION COD2_VERSION_1_3


/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <dlfcn.h> // closer 500: dlcall
#include <stdarg.h> // va_args

#include "config.hpp" // DEBUG_MYSQL etc.

#include "functions.hpp"

#include "gsc_player.hpp"
#include "gsc_astar.hpp"
#include "gsc_mysql.hpp"
#include "gsc_memory.hpp"
#include "gsc_car.hpp"
#include "gsc_tcc.hpp"
#include "gsc_utils.hpp"


#include "cracking.hpp" // closer 900: nop


static void printf_hide(const char *str, ...)
{
	#if DEBUG_PRINTF == 1
	va_list args;
	va_start(args, str);
	vprintf(str, args);
	va_end(args);
	#endif
}


/*
	just search in winhex for "localized string" in the binary and go-to-fileoffset in IDA
	
	in cod4 search for "developer codepos"
*/

#if COD2_VERSION == COD2_VERSION_1_2 || COD2_VERSION == COD2_VERSION_1_3 || COD_VERSION == COD2_1_0
	#define STACK_UNDEFINED 0
	#define STACK_OBJECT 1
	#define STACK_STRING 2
	#define STACK_LOCALIZED_STRING 3
	#define STACK_VECTOR 4
	#define STACK_FLOAT 5
	#define STACK_INT 6
	#define STACK_CODEPOS 7
	#define STACK_PRECODEPOS 8
	#define STACK_FUNCTION 9
	#define STACK_STACK 10
	#define STACK_ANIMATION 11
	#define STACK_DEVELOPER_CODEPOS 12
	#define STACK_INCLUDE_CODEPOS 13
	#define STACK_THREAD_LIST 14
	#define STACK_THREAD_1 15
	#define STACK_THREAD_2 16
	#define STACK_THREAD_3 17
	#define STACK_THREAD_4 18
	#define STACK_STRUCT 19
	#define STACK_REMOVED_ENTITY 20
	#define STACK_ENTITY 21
	#define STACK_ARRAY 22
	#define STACK_REMOVED_THREAD 23
	
#elif COD_VERSION == COD1_1_5

	#define STACK_UNDEFINED 0
	#define STACK_STRING 1
	#define STACK_LOCALIZED_STRING 2
	#define STACK_VECTOR 3
	#define STACK_FLOAT 4
	#define STACK_INT 5
	#define STACK_CODEPOS 6
	#define STACK_OBJECT 7
	#define STACK_KEY_VALUE 8
	#define STACK_FUNCTION 9
	#define STACK_STACK 10
	#define STACK_ANIMATION 11
	#define STACK_THREAD 12
	#define STACK_ENTITY 13
	#define STACK_STRUCT 14
	#define STACK_ARRAY 15
	#define STACK_DEAD_THREAD 16
	#define STACK_DEAD_ENTITY 17
	#define STACK_DEAD_OBJECT 18

#elif COD_VERSION == COD4_1_7
	/*
	THOSE ARE CoD2 ones!
	
	WinHex: 1E3AE3
	
	*/
	#define STACK_UNDEFINED 0
	#define STACK_OBJECT 1
	#define STACK_STRING 2
	#define STACK_LOCALIZED_STRING 3
	#define STACK_VECTOR 4
	#define STACK_FLOAT 5
	#define STACK_INT 6
	#define STACK_CODEPOS 7
	#define STACK_PRECODEPOS 8
	#define STACK_FUNCTION 9
	#define STACK_STACK 10
	#define STACK_ANIMATION 11
	#define STACK_DEVELOPER_CODEPOS 12
	#define STACK_INCLUDE_CODEPOS 13
	#define STACK_THREAD_LIST 14
	#define STACK_THREAD_1 15
	#define STACK_THREAD_2 16
	#define STACK_THREAD_3 17
	#define STACK_THREAD_4 18
	#define STACK_STRUCT 19
	#define STACK_REMOVED_ENTITY 20
	#define STACK_ENTITY 21
	#define STACK_ARRAY 22
	#define STACK_REMOVED_THREAD 23
#else
	// NO ERROR, because those are only available if code is compiled version-dependent!
	//#warning NO STACK TYPES
#endif

typedef struct
{
	void *offsetData;
	int type;
} aStackElement;

int getStack();
int stackNew();
int stackPushUndefined();

int stackGetParamInt(int param, int *value);
int stackGetParamString(int param, char **value);
int stackGetParamVector(int param, float value[3]);
int stackGetParamFloat(int param, float *value);
int stackGetNumberOfParams();
int stackGetParamType(int param);
char *stackGetParamTypeAsString(int param);
int stackGetParams(char *params, ...);

int cdecl_injected_closer_stack_debug();

int stackReturnInt(int ret); // obsolete
int stackPushInt(int ret);
int stackReturnVector(float *ret); // obsolete
int stackPushVector(float *ret);
int stackPushFloat(float ret);
int stackPushString(char *toPush);
int stackPushEntity(int arg);

int stackCallScriptFunction(int self, int scriptFunction, int numberOfArgs);
int alloc_object_and_push_to_array();
int stackSetKeyInArray(int precachedStringOffset);
int push_previous_var_in_array_sub();
int cdecl_injected_closer();


int sub_8101B40(int self, int eInflictor, int eAttacker, float *vDir, float *vPoint, int iDamage, int iDFlags, int iMeansOfDeath, int iHitLoc, int psOffsetTime);
int cdecl_cod2_player_damage_new(int self, int eInflictor, int eAttacker, float *vDir, float *vPoint, int iDamage, int iDFlags, int iMeansOfDeath, int iHitLoc, int psOffsetTime);

// might put it in an extra file later, but atm its just one FS function
// int FS_LoadDir(char *path, char *dir);


// real functions and methods

// functions
typedef void (*Scr_FunctionCall)();

typedef struct {
	const char *name;
	Scr_FunctionCall call;
	int developer;
} Scr_Function;

typedef Scr_FunctionCall (*Scr_GetFunction_t)(const char **fname, int *fdev);

Scr_FunctionCall Scr_GetCustomFunction(const char **fname, int *fdev);

// methods
typedef void (*Scr_MethodCall)(int);

typedef struct {
	const char* name;
	Scr_MethodCall call;
	int developer;
} Scr_Method;

typedef Scr_MethodCall (*Scr_GetMethod_t)(const char**, int*);

Scr_MethodCall Scr_GetCustomMethod(const char **fname, int *fdev);

#ifdef __cplusplus
}
#endif

#endif
