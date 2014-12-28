#include "gsc.hpp"

/*
	CoD2 search for "parameter count exceeds 256" and go upwards
	CoD1 search for "parameter count exceeds 256" or "unknown (builtin) function '%s'" and go upwards
*/

#if COD_VERSION == COD2_1_0
	Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08115824;
	Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x0811595C;
#elif COD_VERSION == COD2_1_2
	Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117B56;
	Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117C8E;
#elif COD_VERSION == COD2_1_3
	Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x08117CB2;
	Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x08117DEA;
#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
	//Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x080BD238;
	//Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x080BFEF4;

	cHook *hook_Scr_GetFunction;
	cHook *hook_Scr_GetMethod;
	
	Scr_FunctionCall Scr_GetFunction(const char **fname, int *fdev) {
		//printf("CoD4 Scr_GetFunction: fdev=%d fname=%s\n", *fdev, *fname);
		
		hook_Scr_GetFunction->unhook();
		int (*sig)(const char **fname, int *fdev);
		*(int *)&sig = hook_Scr_GetFunction->from;
		int m = sig(fname, fdev);
		hook_Scr_GetFunction->hook();
	/*
		when I use the real return-type instead of int (*sig), this errors occurs:
			##### LINK libcod4_1_7.so #####
			objects_cod4_1_7/libcod.opp: In function `Scr_GetFunction_CoD4(char const**, int*)':
			libcod.cpp:(.text+0x132b): undefined reference to `sig(char const**, int*)'
			/usr/bin/ld: objects_cod4_1_7/libcod.opp: relocation R_386_GOTOFF against undefined hidden symbol `_Z3sigPPKcPi' can not be used when making a shared object
			/usr/bin/ld: final link failed: Bad value
			collect2: error: ld returned 1 exit status
			k_cod4_nodlzom@Debian-70-wheezy-64-LAMP:~/libcod$ rm objects_cod4_1_7/libcod.opp
	*/
		return (Scr_FunctionCall)m;
	}
	Scr_MethodCall Scr_GetMethod(const char **fname, int *fdev) {
		//printf("CoD4 Scr_GetMethod: fdev=%d fname=%s\n", *fdev, *fname);
		hook_Scr_GetMethod->unhook();
		int (*sig)(const char **fname, int *fdev);
		*(int *)&sig = hook_Scr_GetMethod->from;
		int m = sig(fname, fdev);
		hook_Scr_GetMethod->hook();
		return (Scr_MethodCall)m;
	}
#else
	#warning Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)NULL;
	#warning Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)NULL;
	Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)NULL;
	Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)NULL;
#endif

char *stackGetParamTypeAsString(int param) {
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;
	
	char *type = "UNKNOWN TYPE!";
	switch (arg->type) {
		case  0: type = "UNDEFINED"; break;
		case  1: type = "OBJECT"; break;
		case  2: type = "STRING"; break;
		case  3: type = "LOCALIZED_STRING"; break;
		case  4: type = "VECTOR"; break;
		case  5: type = "FLOAT"; break;
		case  6: type = "INT"; break;
		case  7: type = "CODEPOS"; break;
		case  8: type = "PRECODEPOS"; break;
		case  9: type = "FUNCTION"; break;
		case 10: type = "STACK"; break;
		case 11: type = "ANIMATION"; break;
		case 12: type = "DEVELOPER_CODEPOS"; break;
		case 13: type = "INCLUDE_CODEPOS"; break;
		case 14: type = "THREAD_LIST"; break;
		case 15: type = "THREAD_1"; break;
		case 16: type = "THREAD_2"; break;
		case 17: type = "THREAD_3"; break;
		case 18: type = "THREAD_4"; break;
		case 19: type = "STRUCT"; break;
		case 20: type = "REMOVED_ENTITY"; break;
		case 21: type = "ENTITY"; break;
		case 22: type = "ARRAY"; break;
		case 23: type = "REMOVED_THREAD"; break;
	}
	return type;
}

int stackPrintParam(int param) {
	if (param >= stackGetNumberOfParams())
		return 0;
		
	switch (stackGetParamType(param)) {
		case STACK_STRING:
			char *str;
			stackGetParamString(param, &str); // no error checking, since we know it's a string
			printf("%s", str);
			return 1;
		case STACK_VECTOR:
			float vec[3];
			stackGetParamVector(param, vec);
			printf("(%.2f, %.2f, %.2f)", vec[0], vec[1], vec[2]);
			return 1;
		case STACK_FLOAT:
			float tmp_float;
			stackGetParamFloat(param, &tmp_float);
			printf("%.3f", tmp_float); // need a way to define precision
			return 1;
		case STACK_INT:
			int tmp_int;
			stackGetParamInt(param, &tmp_int);
			printf("%d", tmp_int);
			return 1;
	}
	printf("(%s)", stackGetParamTypeAsString(param));
	return 0;
}

void gsc_utils_printf() {
	char *str;
	if ( ! stackGetParams("s", &str)) {
		printf("scriptengine> WARNING: printf undefined argument!\n");
		stackPushUndefined();
		return;
	}
	
	int param = 1; // maps to first %
	int len = strlen(str);

	for (int i=0; i<len; i++) {
		if (str[i] == '%')
		{
			if(str[i + 1] == '%')
				putchar('%');
			else
				stackPrintParam(param++);
		}
		else
			putchar(str[i]);
	}
		
	stackPushInt(1);
}
void gsc_utils_printfline() {
	gsc_utils_printf();
	printf("\n");
}

void gsc_utils_com_printf() {
	char *str;
	if ( ! stackGetParams("s", &str)) {
		printf("scriptengine> WARNING: com_printf undefined argument!\n");
		stackPushUndefined();
		return;
	}
	
	#if COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		Com_Printf(0, "%s", str);
	#else
		Com_Printf("%s", str);
	#endif
	
	stackPushInt(1);
}

void gsc_utils_redirectprintf() {
	char *str;
	if ( ! stackGetParams("s", &str)) {
		printf("scriptengine> WARNING: gsc_utils_redirectprintf undefined argument!\n");
		stackPushUndefined();
		return;
	}
	
	SV_FlushRedirect(str);
	stackPushInt(1);
}

Scr_Function scriptFunctions[] = {
	{"printf", gsc_utils_printf, 0},
	{"printfline", gsc_utils_printfline, 0}, // adds \n at end
	{"com_printf", gsc_utils_com_printf, 0},
	{"redirectprintf", gsc_utils_redirectprintf, 0},
	
	#if COMPILE_MYSQL == 1
	{"mysql_init"              , gsc_mysql_init              , 0},
	{"mysql_real_connect"      , gsc_mysql_real_connect      , 0},
	{"mysql_close"             , gsc_mysql_close             , 0},
	{"mysql_query"             , gsc_mysql_query             , 0},
	{"mysql_errno"             , gsc_mysql_errno             , 0},
	{"mysql_error"             , gsc_mysql_error             , 0},
	{"mysql_affected_rows"     , gsc_mysql_affected_rows     , 0},
	{"mysql_store_result"      , gsc_mysql_store_result      , 0},
	{"mysql_num_rows"          , gsc_mysql_num_rows          , 0},
	{"mysql_num_fields"        , gsc_mysql_num_fields        , 0},
	{"mysql_field_seek"        , gsc_mysql_field_seek        , 0},
	{"mysql_fetch_field"       , gsc_mysql_fetch_field       , 0},
	{"mysql_fetch_row"         , gsc_mysql_fetch_row         , 0},
	{"mysql_free_result"       , gsc_mysql_free_result       , 0},
	{"mysql_real_escape_string", gsc_mysql_real_escape_string, 0},
	{"mysql_async_create_query", gsc_mysql_async_create_query, 0},
	{"mysql_async_create_query_nosave", gsc_mysql_async_create_query_nosave, 0},
	{"mysql_async_getdone_list", gsc_mysql_async_getdone_list, 0},
	{"mysql_async_getresult_and_free", gsc_mysql_async_getresult_and_free, 0},
	{"mysql_async_initializer" , gsc_mysql_async_initializer , 0},
	{"mysql_reuse_connection"  , gsc_mysql_reuse_connection  , 0},
	#endif
	
	#if COMPILE_PLAYER == 1
	{"free_slot"                   , gsc_free_slot                         , 0},
	#endif
	
	#if COMPILE_MEMORY == 1
	{"memory_malloc" , gsc_memory_malloc , 0},
	{"memory_free"   , gsc_memory_free   , 0},
	{"memory_int_get", gsc_memory_int_get, 0},
	{"memory_int_set", gsc_memory_int_set, 0},
	{"memory_memset" , gsc_memory_memset , 0},
	#endif

	#if COMPILE_UTILS == 1
	{"disableGlobalPlayerCollision", gsc_utils_disableGlobalPlayerCollision, 0},
	{"getAscii"                    , gsc_utils_getAscii                    , 0},
	{"system"                      , gsc_utils_system                      , 0},
	{"file_link"                   , gsc_utils_file_link                   , 0},
	{"file_unlink"                 , gsc_utils_file_unlink                 , 0},
	{"file_exists"                 , gsc_utils_file_exists                 , 0},
	{"FS_LoadDir"                  , gsc_utils_FS_LoadDir                  , 0},
	{"getType"                     , gsc_utils_getType                     , 0},
	{"stringToFloat"               , gsc_utils_stringToFloat               , 0},
	{"rundll"                      , gsc_utils_rundll                      , 0},
	{"Cmd_ExecuteString"           , gsc_utils_ExecuteString               , 0},
	{"sendGameServerCommand"       , gsc_utils_sendgameservercommand       , 0},
	{"scandir"                     , gsc_utils_scandir                     , 0},
	{"fopen"                       , gsc_utils_fopen                       , 0},
	{"fread"                       , gsc_utils_fread                       , 0},
	{"fwrite"                      , gsc_utils_fwrite                      , 0},
	{"fclose"                      , gsc_utils_fclose                      , 0},
	{"sprintf"                     , gsc_utils_sprintf                     , 0},
	{"add_language"                , gsc_add_language                      , 0},
	{"load_languages"              , gsc_load_languages                    , 0},
	{"get_language_item"           , gsc_get_language_item                 , 0},
	{"G_FindConfigstringIndex"     , gsc_G_FindConfigstringIndex           , 0},
	#endif
	
	#if COMPILE_TCC == 1
	{"tcc_new"             , gsc_tcc_new             , 0},
	{"tcc_add_include_path", gsc_tcc_add_include_path, 0},
	{"tcc_add_file"        , gsc_tcc_add_file        , 0},
	{"tcc_run"             , gsc_tcc_run             , 0},
	{"tcc_delete"          , gsc_tcc_delete          , 0},
	#endif
	
	{"sqrt"   , gsc_math_sqrt,    0},
	{"sqrtInv", gsc_math_sqrtInv, 0},
	
	{NULL, NULL, 0}
};

Scr_FunctionCall Scr_GetCustomFunction(const char **fname, int *fdev) {
	//printf("Scr_GetCustomFunction: fdev=%d fname=%s\n", *fdev, *fname);
	Scr_FunctionCall m = Scr_GetFunction(fname, fdev);
	if (m)
		return m;
	
	for (int i=0; scriptFunctions[i].name; i++) {
		if (strcasecmp(*fname, scriptFunctions[i].name))
			continue;
		Scr_Function func = scriptFunctions[i];
		*fname = func.name;
		*fdev = func.developer;
		return func.call;		
	}
    
	return NULL;
}

void gsc_player_printf(int id) {
	printf("id: %.8x\n", id);
}

Scr_Method scriptMethods[] = {
	{"printf", gsc_player_printf, 0}, // rather use sprintf() to re-use iprintlnbold() etc.?
	
	#if COMPILE_PLAYER == 1
	{"getStance"             , gsc_player_stance_get         , 0},
	{"setVelocity"           , gsc_player_velocity_set       , 0},
	{"addVelocity"           , gsc_player_velocity_add       , 0},
	{"getVelocity"           , gsc_player_velocity_get       , 0},
	{      "aimButtonPressed", gsc_player_button_ads         , 0},
	{     "leftButtonPressed", gsc_player_button_left        , 0},
	{    "rightButtonPressed", gsc_player_button_right       , 0},
	{  "forwardButtonPressed", gsc_player_button_forward     , 0},
	{     "backButtonPressed", gsc_player_button_back        , 0},
	{ "leanleftButtonPressed", gsc_player_button_leanleft    , 0},
	{"leanrightButtonPressed", gsc_player_button_leanright   , 0},
	{     "jumpButtonPressed", gsc_player_button_jump        , 0},
	{"getIP"                 , gsc_player_getip              , 0},
	{"getPing"               , gsc_player_getping            , 0},
	{"getSpectatorClient"    , gsc_player_spectatorclient_get, 0},
	{"ClientCommand"         , gsc_player_ClientCommand      , 0},
	{"getLastConnectTime"    , gsc_player_getLastConnectTime , 0},
	{"getLastMSG"            , gsc_player_getLastMSG         , 0},
	{"getAddressType"        , gsc_player_addresstype        , 0},
	{"getClientState"        , gsc_player_getclientstate     , 0},
	{"renameBot"             , gsc_player_renamebot          , 0},
	{"setAlive"              , gsc_entity_setalive           , 0},
	{"setBounds"             , gsc_entity_setbounds          , 0},
	{"get_userinfo"          , gsc_get_userinfo              , 0},
	{"set_userinfo"          , gsc_set_userinfo              , 0},
	{"printOutOfBand"        , gsc_player_outofbandprint     , 0},
	{"connectionlessPacket"  , gsc_player_connectionlesspacket, 0},
	#endif
	
	{NULL, NULL, 0}
};

Scr_MethodCall Scr_GetCustomMethod(const char **fname, int *fdev) {
	//printf("Scr_GetCustomMethod: fdev=%d fname=%s\n", *fdev, *fname);
	Scr_MethodCall m = Scr_GetMethod(fname, fdev);
	if (m)
		return m;
	
	for (int i=0; scriptMethods[i].name; i++) {
		if (strcasecmp(*fname, scriptMethods[i].name))
			continue;
		Scr_Method func = scriptMethods[i];
		*fname = func.name;
		*fdev = func.developer;
		return func.call;		
	}
	
	return NULL;
}



/*
	In CoD2 this address can be found in every get-param-function
	In CoD1 its a bit harder, search for: "cannot cast %s to int" and go the function upwards:
		the stack-address is in a context like: dword_830AE88 - 8 * a1
*/
int getStack()
{
	#  if COD_VERSION == COD2_1_0
		return 0x083D7610;
	#elif COD_VERSION == COD2_1_2
		return 0x083D7A10; // diff to 1.3: 1080
	#elif COD_VERSION == COD2_1_3
		return 0x083D8A90;
	#elif COD_VERSION == COD1_1_5
		return 0x0830AE88;
	#elif COD_VERSION == COD4_1_7
		return 0x08c055b0;
	#elif COD_VERSION == COD4_1_7_L	
		return 0x08C06330;
	#else
		#warning int getStack() return NULL;
		return (int)NULL;
	#endif
}

int stackGetParamType(int param)
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;
	return arg->type;
}

int stackGetParams(char *params, ...)
{
	va_list args;
	va_start(args, params);
	
	int errors = 0;
	
	int len = strlen(params);
	int i;
	for (i=0; i<len; i++)
	{
		switch (params[i])
		{
			case ' ': // ignore param (e.g. to ignore the function-id from closer()-wrapper)
				break;
			case 'i': {
				int *tmp = va_arg(args, int *);
				if ( ! stackGetParamInt(i, tmp)) {
					printf("Param %d needs to be an int, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
					errors++;
				}
				break;
			}
			case 'v': {
				float *tmp = va_arg(args, float *);
				if ( ! stackGetParamVector(i, tmp)) {
					printf("Param %d needs to be a vector, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
					errors++;
				}
				break;
			}
			case 'f': {
				float *tmp = va_arg(args, float *);
				if ( ! stackGetParamFloat(i, tmp)) {
					printf("Param %d needs to be a float, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
					errors++;
				}
				break;
			}
			case 's': {
				char **tmp = va_arg(args, char **);
				if ( ! stackGetParamString(i, tmp)) {
					printf("Param %d needs to be a string, %s=%d given! NumParams=%d\n", i, ">make function for this<", stackGetParamType(i), stackGetNumberOfParams());
					errors++;
				}
				break;
			}
			
			default:
				errors++;
				printf("[WARNING] stackGetParams: errors=%d Identifier '%c' is not implemented!\n", errors, params[i]);
		}
	}
	
	va_end(args);
	return errors == 0; // success if no errors
}

// function can be found in same context as getStack()
int getNumberOfParams() // as in stackNew()
{
	#  if COD_VERSION == COD2_1_0
		return 0x083D761C;
	#elif COD_VERSION == COD2_1_2
		return 0x083D7A1C; // diff to 1.3: 1080
	#elif COD_VERSION == COD2_1_3
		return 0x083D8A9C;
	#elif COD_VERSION == COD1_1_5
		return 0x0830AE84;
	#elif COD_VERSION == COD4_1_7
		return 0x08c055bc;
	#elif COD_VERSION == COD4_1_7_L
		return 0x08C0633C;
	#else
		#warning int getNumberOfParams() return NULL;
		return (int)NULL;
	#endif
}

// todo: check if the parameter really exists (number_of_params)
int stackGetParamInt(int param, int *value)
{
	//printf("stackGetParamInt() start...");
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;
	if (arg->type != STACK_INT)
		return 0;
	*value = (int)arg->offsetData;
	//printf("... end\n");
	return 1;
}

/*
CoD2: just look in getent() e.g.
int __cdecl sub_8078DFC(int a1)
{
  return stringtable_8205E80 + 8 * a1;
}


kinda hard to find this for CoD1, because i cant trace the get-param-functions... 
but search for: "parameter %d does not exist"
 - then make xrefs-to to it
 - all those functions shall have ONE parent-function
 - go into that parent-function and now just click through till it looks like str-function
*/
int stackGetParamString(int param, char **value) // as in the sub-functions in getentarray (hard one, use the graph to find it)
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;
	if (arg->type != STACK_STRING)
		return 0;
	//*value = "bla";
	//printf("offsetData=%.8x ", arg->offsetData);
	// lnxded 1.3: get_string_stack_element_by_id_sub_8078ec8(int id)
	//printf("datatable + 8*%d = %.8x \"%s\"\n", arg->offsetData, *(int *)0x08206F00 + 8*(int)arg->offsetData, *(int *)0x08206F00 + 8*(int)arg->offsetData + 4);
	
	#  if COD_VERSION == COD2_1_0
		*value = (char *)(*(int *)0x08283E80 + 8*(int)arg->offsetData + 4);
	#elif COD_VERSION == COD2_1_2
		*value = (char *)(*(int *)0x08205E80 + 8*(int)arg->offsetData + 4);
	#elif COD_VERSION == COD2_1_3
		*value = (char *)(*(int *)0x08206F00 + 8*(int)arg->offsetData + 4);
	#elif COD_VERSION == COD1_1_5
		*value = (char *)(*(int *)0x081F6940 + 8*(int)arg->offsetData + 4);
	#elif COD_VERSION == COD4_1_7
		*value = (char *)(*(int *)0x0897ca00 + 12 * (int)arg->offsetData + 4);
	#elif COD_VERSION == COD4_1_7_L
		*value = (char *)(*(int *)0x0897D780 + 12 * (int)arg->offsetData + 4);
	#else
		#warning stackGetParamString(int param, char **value) *value = (char *)(*(int *)NULL + 8*(int)arg->offsetData + 4);
		*value = (char *)(*(int *)NULL + 8*(int)arg->offsetData + 4);
	#endif
	
	return 1;
}
int stackGetParamVector(int param, float value[3])
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;
	if (arg->type != STACK_VECTOR)
		return 0;
	value[0] = *(float *)((int)(arg->offsetData) + 0);
	value[1] = *(float *)((int)(arg->offsetData) + 4);
	value[2] = *(float *)((int)(arg->offsetData) + 8);
	return 1;
}

int stackGetParamFloat(int param, float *value)
{
	aStackElement *scriptStack = *(aStackElement**)getStack();
	aStackElement *arg = scriptStack - param;
	
	if (arg->type == STACK_INT)
	{
		int asInteger;
		int ret = stackGetParamInt(param, &asInteger);
		if (!ret)
			return 0;
		*value = (float) asInteger;
		return 1;
	}
	
	// *value = (float)arg->offsetData;
	// gcc: error: pointer value used where a floating point value was expected
	// so i use the tmp for casting
	float tmp;
	if (arg->type != STACK_FLOAT)
		return 0;
	//swapEndian(&arg->offsetData); // DOESEN WORK EVEN WITH SWAP
	//*value = (float)(int)arg->offsetData; // DOESNT WORK
	// jeah gcc, you fucked me off!
	memcpy(&tmp, &arg->offsetData, 4); // cast to float xD
	*value = tmp;	
	return 1;
}
int stackGetNumberOfParams()
{
	int numberOfParams = *(int *)getNumberOfParams();
	return numberOfParams;
}


int cdecl_injected_closer_stack_debug()
{
	int i;
	int numberOfParams = *(int *)0x83D8A9C;
	aStackElement *scriptStack = *(aStackElement**)0x83D8A90;
	
	printf("numberOfParams=%d\n", numberOfParams);
	for (i=0; i<numberOfParams; i++)
	{
		aStackElement *stackPtr = scriptStack - i;
		printf("i=%d offsetData=%p type=%d\n", i, stackPtr->offsetData, stackPtr->type);
		
		switch (stackPtr->type)
		{
			case STACK_OBJECT:
			{
				printf("type=object with: ");
				
				int object_table = 0x8297500; // int16
				
				int offsetData = *(int *)(object_table + 2*((int)stackPtr->offsetData * 8) + 0);
				
				int type = *(int *)(object_table + 2*((int)stackPtr->offsetData * 8) + 4) % 31;
				
				printf("offsetData:%.8x type:%.8x\n", offsetData, type);
				
				break;
			}
		}
		
	}
	return 1;
}

/* THE BEGINNING of generalizing the push-value-functions! */
// pushing a new stack-element on stack
// available through getStack()
// 11.03.2013, Sido|Meine Jordans Instrumental, Generalisation is unstable!

/* search for "Internal script stack overflow", thats stackNew() */
/* can also be found in the next stackPush-functions */
int stackNew()
{
	int (*signature)();
	
	#if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x080837B0;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x08083D2C;
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x08083DF8;
	#elif COD_VERSION == COD1_1_5
		*((int *)(&signature)) = 0x080AE084;
	#elif COD_VERSION == COD4_1_7
		*((int *)(&signature)) = 0x0815EC48;
	#elif COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x0815EC68;
	#else
		#warning int stackNew() *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = (int)NULL;
	#endif
	
	return signature();
}
int stackPushUndefined()
{
	aStackElement *scriptStack;
	
	scriptStack = *(aStackElement**)getStack();
	#if DEBUG_GSC
	printf("stackPushUndefined(): type=%d value=%.8x\n", scriptStack->type, scriptStack->offsetData);
	#endif
	int ret = stackNew();
	
	scriptStack = *(aStackElement**)getStack();
	#if DEBUG_GSC
	printf("stackPushUndefined(): type=%d value=%.8x\n", scriptStack->type, scriptStack->offsetData);
	#endif
	
	//aStackElement *scriptStack = *(aStackElement**)getStack();
	//aStackElement *scriptStack = *(aStackElement**)ret;
	scriptStack->type = STACK_UNDEFINED;
	scriptStack->offsetData = NULL; // never tested anything else for UNDEFINED
	//return (int) *(aStackElement**) getStack(); // dunno...
	return 123; // dunno... works also lol. so no need to return some specific stackelement
}


/*

	HOWTO: how to find the addresses of a binary?
	1) find the function-string in winhex
	2) go to file offset in IDA -> then copy the real address
	3) then search for the SWAPPED value in winhex again
	4) go for it in ida 
	5) convert the crazy numbers to str/func-pair
	6) go into function and get the offset of the internal function
*/
int stackReturnInt(int ret) /*obsolete - not guaranteed to be available later*/
{
	return stackPushInt(ret);

	/*
	int (*signature)(int);	
	*((int *)(&signature)) = 0x08085164;
	return signature(ret);
	*/
}
int stackPushInt(int ret) // as in isalive
{
	#if 1
		int (*signature)(int);
		
		#  if COD_VERSION == COD2_1_0
			*((int *)(&signature)) = 0x08084B1C;
		#elif COD_VERSION == COD2_1_2
			*((int *)(&signature)) = 0x08085098; // difference to 1.3: CC
		#elif COD_VERSION == COD2_1_3
			*((int *)(&signature)) = 0x08085164;
		#elif COD_VERSION == COD4_1_7
			*((int *)(&signature)) = 0x0815EFFA;
		#elif COD_VERSION == COD4_1_7_L
			*((int *)(&signature)) = 0x0815F01A;
		#else
			#warning int stackPushInt(int ret)
			*((int *)(&signature)) = (int)NULL;
		#endif
		
		return signature(ret);
	#endif
	
	// this shit dont work! seg fault
	#if 0
		aStackElement *scriptStack;
		stackNew();
		scriptStack = *(aStackElement**)getStack();
		scriptStack->type = STACK_INT;
		scriptStack->offsetData = (void *)ret;
		
		printf("still working int??\n");
		
		return ret;
		return -1234; // crap, just testin if i can return void
	#endif
}
int stackReturnVector(float *ret) // obsolete
{
	return stackPushVector(ret);
}

/*
	for CoD1:
		search MT_AllocIndex
		go 2 functions up or so and compare structure with CoD2
	for other CoD's:
		just go into getOrigin() or so and look
*/
int stackPushVector(float *ret) // as in vectornormalize
{
	
	int (*signature)(float *);
	
	#  if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x08084CBE;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x0808523A; // difference to 1.3: CC
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x08085306;
	#elif COD_VERSION == COD1_1_5
		printf("*((int *)(&signature)) = 0x080AF464;\n");
		*((int *)(&signature)) = 0x080AF464;
	#elif COD_VERSION == COD4_1_7
		*((int *)(&signature)) = 0x0815EDF2;
	#elif COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x0815EE12;
	#else
		#warning int stackPushVector(float *ret) *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = (int)NULL;
	#endif
	
	return signature(ret);
	
	
	#if 0
		int (*alloc_vector)(float *);
		*((int *)(&alloc_vector)) = 0x080A5D14;
		
		aStackElement *scriptStack;
		stackNew();
		scriptStack = *(aStackElement**)getStack();
		scriptStack->type = STACK_VECTOR;
		printf("1\n");
		scriptStack->offsetData = (void *)alloc_vector(ret);
		printf("2\n");
	#endif
}
int stackPushFloat(float ret) // as in distance
{
	int (*signature)(float);
	
	#  if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x08084B40;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x080850BC; // difference to 1.3: CC
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x08085188;
	#elif COD_VERSION == COD4_1_7
		*((int *)(&signature)) = 0x0815EF7A;
	#elif COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x0815EF9A;
	#else
		#warning int stackPushFloat(float ret) *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = (int)NULL;
	#endif
	
	return signature(ret);
}
int stackPushString(char *toPush) // as in getcvar()
{
	int (*signature)(char *);
	
	#  if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x08084C1A;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x08085196; // difference to 1.3: CC
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x08085262;
	#elif COD_VERSION == COD4_1_7
		*((int *)(&signature)) = 0x0815EC48;	
	#elif COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x0815EC68;
	#else
		#warning int stackPushString(char *toPush) *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = (int)NULL;
	#endif
	
	return signature(toPush);
}
int stackPushEntity(int arg) // as in getent() // todo: find out how to represent an entity
{
	int (*signature)(int);
	
	#  if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x08118CC0;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x0811AFF4; // difference OTHER then CC
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x08117F50;
	#elif COD_VERSION == COD4_1_7 || COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x080C7770;
	#else
		#warning int stackPushEntity(int arg) *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = (int)NULL;
	#endif
	
	return signature(arg);
}
int stackCallScriptFunction(int self, int scriptFunction, int numberOfArgs)
{
	int (*signature)(int, int, int);
	*((int *)(&signature)) = 0x0811B284;
	return signature(self, scriptFunction, numberOfArgs);
}
int stackCallbackPlayerDamage = 0x0884D718;

// as in bullettrace
int stackPushArray() {
	int (*signature)();
	
	#  if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x08084CF0;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x0808526C;
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x08085338;
	#elif COD_VERSION == COD4_1_7
		*((int *)(&signature)) = 0x0815ED6A;
	#elif COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x0815ED8A;
	#else
		#warning int stackPushArray() *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = (int)NULL;
	#endif
	
	return signature();
}

int stackSetKeyInArray(int precachedStringOffset) // TODOOOOOO
{
	int (*signature)(int);
	*((int *)(&signature)) = 0x080853B6;
	return signature(precachedStringOffset);
}

int stackPushArrayLast() { // as in getentarray
	int (*signature)();
	
	#if COD_VERSION == COD2_1_0
		*((int *)(&signature)) = 0x08084D1C;
	#elif COD_VERSION == COD2_1_2
		*((int *)(&signature)) = 0x08085298;
	#elif COD_VERSION == COD2_1_3
		*((int *)(&signature)) = 0x08085364;
	#elif COD_VERSION == COD4_1_7
		*((int *)(&signature)) = 0x0815D5A0;	
	#elif COD_VERSION == COD4_1_7_L
		*((int *)(&signature)) = 0x0815D5C0;
	#else
		#warning int stackPushArrayLast() *((int *)(&signature)) = NULL;
		*((int *)(&signature)) = (int)NULL;
	#endif
	
	return signature();
}

int sub_807BCA8()
{
	int16_t (*sub_807B9F8)();
	*(int *)(&sub_807B9F8) = 0x0807B9F8;
	
	int *startStack = (int*)0x08297500;
	
	int16_t var;
	int *ptr;
	
	var = sub_807B9F8(); // make new variable
	
	printf("var=%d\n", var);
	ptr = &startStack[4*var];
	startStack[4 * var + 2] = 96;
	*(ptr + 2) |= 22;
	*((int16_t*)ptr + 2) = 0;
	*((int16_t*)ptr + 3) = 0;
	printf("(int16_t*)ptr + 2) = %hn\n", (int16_t*)ptr + 2);
	printf("(int16_t*)ptr + 3) = %hn\n", (int16_t*)ptr + 3);
	
	/*
	var=640
	(int16_t*)ptr + 2) = 08299d04
	(int16_t*)ptr + 3) = 08299d06
	*/
	
	return var;
}

#define TOSTRING2(str) #str
#define TOSTRING1(str) TOSTRING2(str) // else there is written "__LINE__"
#define AT __FILE__ ":" TOSTRING1(__LINE__)

// i would like to make nice php-scripts, which are building something like a "cracking-ide" with many informations
// all infos easy SEARCHABLE, like... enter 4 letters of function -> find all saved graphs of it
// doesnt even need to be "normalized", just TEXT-seach in DB
int level = 0; // for a nice tabbed graphcall
#define LEVEL_SPACE do { for (int i=0; i<level; i++) printf("    "); }while(0);

// gsc_new_variable
int sub_807AB64(void *offsetData, int key, int a_plus_b_mod_fffd_plus_1)
{
	LEVEL_SPACE; printf("setKeyInArray: int sub_807AB64(void *offsetData=%p, int key=%.8x, int a_plus_b_mod_fffd_plus_1=%.8x)\n", offsetData, key, a_plus_b_mod_fffd_plus_1);
	LEVEL_SPACE; printf("{\n");
	level++;
	
	int ret;
	
	#if 1
	LEVEL_SPACE; printf("BUILTIN-FUNCTION\n");
	int (*tmp)(void *offsetData, int key, int a_plus_b_mod_fffd_plus_1);
	*(int *)&tmp = 0x0807AB64;
	ret = tmp(offsetData, key, a_plus_b_mod_fffd_plus_1);
	#endif
	

	//LEVEL_SPACE; printf("setKeyInArray: int sub_807AB64 #####>>>>> ret=%.8x\n", ret); level--;
	level--; LEVEL_SPACE; printf("} ret=%.8x\n", ret);
	return ret;
}


	typedef struct
	{
		int16_t a_0; // 0
		int16_t a_1; // 2
		int16_t b_0; // 4
		int16_t b_1; // 6
		int c; // 8
		int16_t d_0; // 12
		int16_t d_1; // 14
	} aStack;
	
	
	void print_aStack(aStack *stack)
	{
		printf("a_0=%.4x a_1=%.4x b_0=%.4x b_1=%.4x c=%.8x d_0=%.4x d_1=%.4x ",
			stack->a_0, stack->a_1, stack->b_0, stack->b_1, stack->c, stack->d_0, stack->d_1
		);
	}
	
int sub_807B064(void *offsetData, int key, int a_plus_b_mod_fffd_plus_1)
{
	LEVEL_SPACE; printf("int sub_807B064(void *offsetData=%p, int key=%.8x, int a_plus_b_mod_fffd_plus_1=%.8x)\n", offsetData, key, a_plus_b_mod_fffd_plus_1);
	LEVEL_SPACE; printf("{\n");
	level++;
	
	int ret;
	
	#if 0
	LEVEL_SPACE; printf("BUILTIN-FUNCTION\n");
	int (*tmp)(void *offsetData, int key, int a_plus_b_mod_fffd_plus_1);
	*(int *)&tmp = 0x0807B064;
	ret = tmp(offsetData, key, a_plus_b_mod_fffd_plus_1);
	#endif
	
	#if 0 // BACKUP THAT WORKS LIKE IN IDA, NO CHANGE!
	
	int *stackStart = (int*) 0x08297500;
	
	int v4, v5, *v6, *v8, *v9;
	unsigned int/*16_t*/ v7;
	char *v10;
	
	//a_plus_b_mod_fffd_plus_1 = (int16_t)a_plus_b_mod_fffd_plus_1;
	
	v4 = sub_807AB64(offsetData, key, a_plus_b_mod_fffd_plus_1);
	a_plus_b_mod_fffd_plus_1 = v4;
	v6 = stackStart + 4 * (int)offsetData;
	v7 = *((int16_t*)stackStart + 2 * (int)offsetData + 7);
	v8 = stackStart + 4 * v7;
	v9 = stackStart + 4 * v4;
	v5 = 16 * *((int16_t*)stackStart + 8 * v4);
	v10 = (char *)stackStart + v5;
	*(int16_t*)((char *)stackStart + v5 + 14) = v7;
	*((int16_t*)v8 + 1) = a_plus_b_mod_fffd_plus_1;
	*((int16_t*)v9 + 1) = *((int16_t*)v6 + 6);
	*((int16_t*)v6 + 7) = a_plus_b_mod_fffd_plus_1;	
	
	ret = a_plus_b_mod_fffd_plus_1;
	#endif
	
	#if 0
	
	int *stackStart = (int*) 0x08297500;
	

	aStack *stackStart_2 = (aStack*) 0x08297500;
	
	int v4, v5, *v6, *v8, *v9;
	unsigned int/*16_t*/ v7;
	char *v10;
	
	//a_plus_b_mod_fffd_plus_1 = (int16_t)a_plus_b_mod_fffd_plus_1;
	LEVEL_SPACE; printf(">>> 2\n");
	int ret_807AB64;
	/*v4*/ ret_807AB64 = sub_807AB64(offsetData, key, a_plus_b_mod_fffd_plus_1);
	//a_plus_b_mod_fffd_plus_1 = v4;
	v6 = stackStart + 4 * (int)offsetData;
	aStack *v6_2 = &stackStart_2[(int)offsetData];
	LEVEL_SPACE; printf("v6=%.8x v6_2=%.8x\n", v6, v6_2);
	v7 = *((int16_t*)stackStart + 2 * (int)offsetData + 7);
	
	v8 = stackStart + 4 * v7;
	aStack *v8_2 = &stackStart_2[v7];
	LEVEL_SPACE; printf("v8=%.8x v8_2=%.8x\n", v8, v8_2);
	
	v9 = stackStart + 4 * ret_807AB64;
	aStack *v9_2 = &stackStart_2[ret_807AB64];
	LEVEL_SPACE; printf("v9=%.8x v9_2=%.8x\n", v9, v9_2);
	
	v5 = 16 * *((int16_t*)stackStart + 8 * ret_807AB64);
	int v5_2 = 16 * *(int16_t*) (stackStart + 4 * ret_807AB64);
	int v5_3 = 16 * *(int16_t*) (&stackStart_2[ret_807AB64]);
	int v5_4 = 16 * stackStart_2[ret_807AB64].a_0;
	
	LEVEL_SPACE; printf("v5=%.8x v5_2=%.8x v5_3=%.8x v5_4=%.8x\n", v5, v5_2, v5_3, v5_4);
	
	v10 = (char *)stackStart + v5;
	int *v10_2 = &stackStart[4 * *((int16_t*)stackStart + 8 * ret_807AB64)];
	int v10_3 = (int)&stackStart_2[*((int16_t*)stackStart + 8 * ret_807AB64)];
	int v10_4 = (int)&stackStart_2[stackStart_2[ret_807AB64].a_0];
	aStack *v10_5 = stackStart_2 + stackStart_2[ret_807AB64].a_0;
	LEVEL_SPACE; printf("v10=%.8x v10_2=%.8x v10_3=%.8x v10_4=%.8x v10_5=%.8x\n", v10, v10_2, v10_3, v10_4, v10_5);
	
	*(int16_t*)((char *)stackStart + v5 + 14) = v7;
	// this:
	int bla = (int)((char *)stackStart + v5 + 14);
	int bla_1 = (int)   &(stackStart_2 + stackStart_2[ret_807AB64].a_0)->d_1;
	int bla_2 = (int)   &((stackStart_2 [stackStart_2[ret_807AB64].a_0]).d_1);
	int bla_3 = (int)   &(v10_5->d_1);
	LEVEL_SPACE; printf("bla=%.8x bla_1=%.8x bla_2=%.8x bla_3=%.8x\n", bla, bla_1, bla_2, bla_3);
	
	*((int16_t*)v8 + 1) = ret_807AB64; //a_plus_b_mod_fffd_plus_1;
	int foo = (int) (v8+1);
	int foo_1 = (int) &v8_2->b_0;
	LEVEL_SPACE; printf("foo=%.8x foo_1=%.8x\n", foo, foo_1);
	LEVEL_SPACE; printf("*((int16_t*)v8 + 1==%.8x) = ret_807AB64==%.8x;\n", ((int16_t*)v8 + 1), ret_807AB64);
	LEVEL_SPACE; printf("*((int16_t*)v8 + 1==%.8x) = ret_807AB64==%.8x;\n", &v8_2->a_1, ret_807AB64);
	
	*((int16_t*)v9 + 1) = *((int16_t*)v6 + 6);
	LEVEL_SPACE; printf("*((int16_t*)v9 + 1==%.8x) = *((int16_t*)v6 + 6==%.8x);\n", ((int16_t*)v9 + 1), ((int16_t*)v6 + 6));
	LEVEL_SPACE; printf("*((int16_t*)v9 + 1==%.8x) = *((int16_t*)v6 + 6==%.8x);\n", &v9_2->a_1, &v6_2->d_0);
	
	*((int16_t*)v6 + 7) = ret_807AB64; //a_plus_b_mod_fffd_plus_1;	
	LEVEL_SPACE; printf("*((int16_t*)v6 + 7==%.8x) = ret_807AB64==%.8x;\n", ((int16_t*)v6 + 7), ret_807AB64);
	LEVEL_SPACE; printf("*((int16_t*)v6 + 7==%.8x) = ret_807AB64==%.8x;\n", &v6_2->d_1, ret_807AB64);
	
	ret = ret_807AB64/*a_plus_b_mod_fffd_plus_1*/;
	#endif
	
	#if 0

	int *stackStart = (int*) 0x08297500;
	aStack *stackStart_2 = (aStack *) 0x08297500;
	
	int v4;
	int v5;
	aStack *v6;
	aStack *v8;
	aStack *v9;
	unsigned int/*16_t*/ v7;
	aStack *v10;
	
	a_plus_b_mod_fffd_plus_1 = (int16_t)a_plus_b_mod_fffd_plus_1;
	
	v4 = sub_807AB64(offsetData, key, a_plus_b_mod_fffd_plus_1);
	a_plus_b_mod_fffd_plus_1 = v4;
	v6 = stackStart_2 + (int)offsetData;
	v7 = *((int16_t*)stackStart + 2 * (int)offsetData + 7);
	v8 = stackStart_2 + v7;
	v9 = stackStart_2 + v4;
	v5 = stackStart_2[v4].a_0;
	v10 = stackStart_2 + v5;
	stackStart_2[v5].d_1 = v7;
	
	//*((int16_t*)v8 + 1) = a_plus_b_mod_fffd_plus_1;
	v8->a_1 = a_plus_b_mod_fffd_plus_1;
	
	//*((int16_t*)v9 + 1) = *((int16_t*)v6 + 6);
	v9->a_1 = v6->d_0;
	
	//*((int16_t*)v6 + 7) = a_plus_b_mod_fffd_plus_1;
	v6->d_1 = a_plus_b_mod_fffd_plus_1;
	
	ret = a_plus_b_mod_fffd_plus_1;
	
	#endif
	
	

	
	#if 1

	int *stackStart = (int*) 0x08297500;
	aStack *stackStart_2 = (aStack *) 0x08297500;
	
	int v4;
	int v5;
	aStack *v6;
	aStack *v8;
	aStack *v9;
	int v7;
	aStack *v10;
	
	//a_plus_b_mod_fffd_plus_1 = (int16_t)a_plus_b_mod_fffd_plus_1;
	
	int ret_807AB64 = sub_807AB64(offsetData, key, a_plus_b_mod_fffd_plus_1);

	v6 = stackStart_2 + (int)offsetData;
	v7 = *((int16_t*)stackStart + 2 * (int)offsetData + 7);
	v8 = stackStart_2 + v7;
	v9 = stackStart_2 + ret_807AB64;
	v5 = stackStart_2[ret_807AB64].a_0;
	v10 = stackStart_2 + v5;
	stackStart_2[v5].d_1 = v7;
	
	v8->a_1 = ret_807AB64;
	
	v9->a_1 = v6->d_0;
	
	v6->d_1 = ret_807AB64;
	
	ret = ret_807AB64;
	
	
	LEVEL_SPACE; printf("v8: "); print_aStack(v8); printf("\n");
	LEVEL_SPACE; printf("v9: "); print_aStack(v9); printf("\n");
	LEVEL_SPACE; printf("v6: "); print_aStack(v6); printf("\n");
	
	#endif
	
	//LEVEL_SPACE; printf("setKeyInArray: int sub_807B064 #####>>>>> ret=%.8x\n", ret); level--;
	level--; LEVEL_SPACE; printf("} ret=%.8x\n", ret);
	
	return ret;
}
int sub_807B1E6(void *offsetData, int key)
{
	LEVEL_SPACE; printf("int sub_807B1E6(void *offsetData=%p, int key=%.8x)\n", offsetData, key);
	LEVEL_SPACE; printf("{\n");
	level++;
	
	int ret;
	
	#if 0
	int (*tmp)(void *offsetData, int key);
	*(int *)&tmp = 0x0807B1E6;
	ret = tmp(offsetData, key);
	#endif
	
	#if 1
	ret = sub_807B064(offsetData, key, ((int)offsetData+key) % 0xFFFD + 1);
	#endif
	
	//LEVEL_SPACE; printf("setKeyInArray: int sub_807B1E6 #####>>>>> ret=%.8x\n", ret);
	level--; LEVEL_SPACE; printf("} ret=%.8x\n", ret);
	return ret;
}

// THE ADDRESS IS TOLD BY THE LINKER AT LINK-TIME!
// THAT FAILED!!! GCC, im gonna hate you!
// gcc: -Xlinker --defsym -Xlinker stackStart=0x08297500
// output of stackStart-address: stackStart=c0185500
#if 0
	extern int stackStart;
#endif

int sub_807C71C(void *offsetData, int key)
{
	LEVEL_SPACE; printf("int sub_807C71C(void *offsetData=%p, int key=%.8x)\n", offsetData, key);
	LEVEL_SPACE; printf("{\n");
	level++;
	
	int ret;
	
	#if 0
	int (*tmp)(void *offsetData, int key);
	*(int *)&tmp = 0x0807C71C;
	return tmp(offsetData, key);
	#endif
	
	#if 0
	// doesnt work, the code has relative elements (function-call)
	unsigned char data[36] = {
		0x55, 0x89, 0xE5, 0x83, 0xEC, 0x08, 0x8B, 0x45, 0x0C, 0x89, 0x44, 0x24, 0x04, 0x8B, 0x45, 0x08, 
		0x89, 0x04, 0x24, 0xE8, 0xB2, 0xEA, 0xFF, 0xFF, 0xC1, 0xE0, 0x04, 0x0F, 0xB7, 0x80, 0x00, 0x75, 
		0x29, 0x08, 0xC9, 0xC3
	};
	int (*tmp)(void *offsetData, int key);
	*(int *)&tmp = (int)&data;
	return tmp(offsetData, key);
	#endif
	
	// THIS IS JUST CRASHING THE WHOLE SERVER
	// IT NEEDS TO BE COMPILED WITH -masm=intel and it compiles, but crashing without execute THESE lines...
	// BUG IN COMPILER
	//printf("VOR\n");
	/*asm(".intel_syntax noprefix\n");
	asm("ret\n");
	asm("ret\n");*/
	//printf("DANACH\n");
	
	#if 0
		int *stackStart = (int*) 0x08297500;
		ret = *(int16_t*)(stackStart + 4 * sub_807B1E6(offsetData, key)); // works... kind of the nicest
		//ret = *(int*)((int)stackStart + 16 * sub_807B1E6(offsetData, key)); // fails
		//ret = *(int16_t*)((int)stackStart + 16 * sub_807B1E6(offsetData, key)); // works also
		//ret = *((int16_t*)0x08297500 + 8 * sub_807B1E6(offsetData, key)); // works
	#endif

	#if 1
		//LEVEL_SPACE; printf("NEXT TRY\n"); // seen, works, commented out!
		aStack *stackStart = (aStack*) 0x08297500;
		ret = stackStart[sub_807B1E6(offsetData, key)].a_0;
	#endif
	
	#if 0
		int sub = sub_807B1E6(offsetData, key);
		printf("sub_807B1E6(offsetData, key) = %.8x\n", sub);
		
		printf("(int *)0x08297500 = %.8x\n", (int *)0x08297500);
		printf("*(int *)0x08297500 = %.8x\n", *(int *)0x08297500);
		
		int ret = *(int*)((char *)0x08297500 + 16 * sub);
		printf("int sub_807C71C(void *offsetData, int key) = %.8x\n", ret);
		
		return ret;
	#endif
	
	#if 0
		aStackElement **stackStart = (aStackElement **)0x08297500; // 8297500 -> fffd0000
		return (int)(*stackStart)[sub_807B1E6(offsetData, key)].offsetData;
	#endif
	
	level--; LEVEL_SPACE; printf("} ret=%.8x\n", ret);
	return ret;
}



int sub_807CB12(int v2, aStackElement *stackElement)
{
	LEVEL_SPACE; printf(/*AT*/ "int sub_807CB12(int v2=%.8x, aStackElement *stackElement=%p(type=%d))\n", v2, stackElement, stackElement->type);
	LEVEL_SPACE; printf("{\n");
	level++;
	
	int ret;
	
	// CANT IT IT SELF TO WORK!
	// SO IM USING THE BUILT-IN-FUNCTION
	#if 1
	LEVEL_SPACE; printf("BUILTIN-FUNCTION\n");
	int (*tmp)(int v2, aStackElement *stack);
	*(int *)&tmp = 0x0807CB12;
	ret = tmp(v2, stackElement);
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

int sub_80853B6(int key) // setKeyInArray
{
	LEVEL_SPACE; printf("int sub_80853B6(int key=%.8x)\n", key);
	LEVEL_SPACE; printf("{\n");
	level++;

	// BEST BEST BEST BEST
	// this comes the "native" variables the nearest
	// they are pointers to the places, and and array is then pointer-on-pointer
	// each access must be dereferened, to get the access of the real-mem-location

	int *dword_83D8A98 = (int *)0x83D8A98;
	aStackElement **scriptStack = (aStackElement**)0x83D8A90; // 83D8A90 -> 083d8da0
	
	//printf("scriptStack = %.8x\n", scriptStack);
	(*scriptStack)-=1;
	
	/*{
	
	aStackElement *scriptStack = *(aStackElement**)0x83D8A90;
	printf("scriptStack = %.8x\n", scriptStack);
	}*/
	
	(*dword_83D8A98)--;
	
	//printf("key = %.8d\n", key);
	//printf("(*scriptStack)->offsetData = %.8d\n", (*scriptStack)->offsetData);
	int v2, ret;
	v2 = sub_807C71C((*scriptStack)->offsetData, key);
	ret = sub_807CB12(v2, (*scriptStack)+1); // +1=FLOAT
	
	level--; LEVEL_SPACE; printf("} ret=%.8x\n", ret);
	return ret;
	
	
	//printf("blub\n");
}

/*
// e.g. FS_LoadDir("/home/ns_test", "NsZombiesV4.3");
int FS_LoadDir(char *path, char *dir)
{
	int (*signature)(char *path, char *dir);
	*(int *)&signature = 0x080A22D8;
	return signature(path, dir);
}
*/

int cdecl_injected_closer()
{
	int function;
	int playerid;
	
	// print in debug-modus dynamically the parameters
	//printf("scriptengine> closer(function=%d, %d, %.2f, %.2f, %.2f)\n", function, playerid, velocity_x, velocity_y, velocity_z);

	
	#if 0
		printf("Welcome in my closer()! The first step for a new CoD-Version is to make stackGetNumberOfParams() work!\n");
		printf("argc=%d\n", stackGetNumberOfParams());
		//exit(1);
		
		if (0)
		{
			int testInt;
			if (stackGetParamInt(0, &testInt))
				printf("testInt: %d\n", testInt);
			else
				printf("No INT given!\n");
			return stackPushInt(111);
		}
		if (0)
		{
			char *testString;
			if (stackGetParamString(0, &testString))
				printf("testString: \"%s\"\n", testString);
			else
				printf("No STRING given!\n");
			return stackPushString("aaaaaaaaaaaa");
		}
		if (0)
		{
			float testVector[3];
			if (stackGetParamVector(0, testVector))
				printf("testVector: (%.2f, %.2f, %.2f)\n", testVector[0], testVector[1], testVector[2]);
			else
				printf("No VECTOR given!\n");
			float tmp[3] = {0,1,2};
			return stackPushVector(tmp);
		}
	#endif
	
	float reference[3], point_a[3], point_b[3];
	if (
		stackGetNumberOfParams() == 3 &&
		stackGetParamVector(0, reference) &&
		stackGetParamVector(1, point_a) &&
		stackGetParamVector(2, point_b)
	) {
		// > Tests which of two points is the closest. Returns true if point a is closer to the reference than point b
		float delta_a[3] = {
			point_a[0] - reference[0],
			point_a[1] - reference[1],
			point_a[2] - reference[2]
		};
		float delta_b[3] = {
			point_b[0] - reference[0],
			point_b[1] - reference[1],
			point_b[2] - reference[2]
		};
		float distancesquared_a = delta_a[0]*delta_a[0] + delta_a[1]*delta_a[1] + delta_a[2]*delta_a[2];
		float distancesquared_b = delta_b[0]*delta_b[0] + delta_b[1]*delta_b[1] + delta_b[2]*delta_b[2];

		if (distancesquared_a < distancesquared_b)
			return stackPushInt(1);
		else
			return stackPushInt(0);
	}
	

	
	
	if (!stackGetParamInt(0, &function))
	{
		printf("scriptengine> ERROR: closer(): param \"function\"[0] has to be an integer!\n");
		return 0;
	}

	// test-functions to perform a bug-test (find non-working functions)
	/*
		closer((1,1,1),(1,1,1),(1,1,1)); // read params test
		//closer(200, "shall be 666:" + closer(4001) + "\n"); // int test
		closer(200, "shall be 33:" + closer(4002)[1] + "\n"); // vector test
		closer(200, "shall be 3rd:" + closer(4003)[2] + "\n"); // 3rd
		closer(200, "shall be 3:" + closer(4004 )[2] + "\n"); // 3rd
	*/
	switch (function) {
		case 4001:
			printf("4001!\n");
			return stackPushInt(666);
		case 4002: {
			printf("4002!\n");
			float vec[3] = {1, 33, 7};
			return stackPushVector(vec);
		}
		case 4003: {
			printf("4003!\n");
			int ret = stackPushArray();
			stackPushString((char *)"1st");
			stackPushArrayLast();
			stackPushString((char *)"2nd");
			stackPushArrayLast();
			stackPushString((char *)"3rd");
			stackPushArrayLast();	
			return ret;
		}
		case 4004: {
			printf("4004!\n");
			int ret = stackPushArray();
			stackPushFloat(1);
			stackPushArrayLast();
			stackPushFloat(2);
			stackPushArrayLast();
			stackPushFloat(3);
			stackPushArrayLast();
			return ret;
		}
	}
	
	// had to fix STACK_-defines for CoD1
	//printf("after stackGetParamInt\n");
	
	// 8716558 pointed on that!! and that i found in setorigin() with ida decompiler
	int playerStates = 0x087a2500;
	int sizeOfPlayer = 0x28A4;
	
	#if COMPILE_ASTAR == 1
	switch (function) {
		case 8: return gsc_graph_new();
		case 9: return gsc_graph_add_edge();
		case 10: return gsc_graph_add_vertex();
		case 11: return gsc_graph_delete();
		case 12: return gsc_graph_build();
		case 13: return gsc_graph_do_astar();
		//if (function == 14) { return gsc_tracefraction...;
		case 15: return gsc_graph_get_nearest_edge();
		//if (function == 16) { return gsc_math_nearest_point_on_linesegment();
	}
	#endif

	#if COMPILE_ASTAR == 1
	if (function == 16)
	{
		float from[3];
		float to[3];
		float toPoint[3];
		int helper = 0;
		helper += stackGetParamVector(1, from);
		helper += stackGetParamVector(2, to);
		helper += stackGetParamVector(3, toPoint);
		if (helper != 3)
		{
			printf("scriptengine> wrongs args for: int MATH_nearest_point_on_linesegment(from, to, toPoint, out_point)\n");
			return stackPushInt(0);
		}
		float out_point[3];
		MATH_nearest_point_on_linesegment(from, to, toPoint, out_point);
		
		return stackReturnVector(out_point);
	}
	#endif
	
	// like bullettrace just with position and fraction
	if (function == 14)
	{
		float fraction;
		float from[3];
		float to[3];
		float nullVector[3] = {0,0,0};
	
		int helper = 0;
		helper += stackGetParamVector(1, from);
		helper += stackGetParamVector(2, to);
		if (helper != 2)
		{
			printf("scriptengine> wrongs args for: traceFraction(from, to)\n");
			return stackPushInt(0);
		}
	
		int (*signature)(float *outFraction, float *vectorFrom, float *vectorTo, float *nullVector0, float *nullVector1, int isZero, int mask);
		*((int *)(&signature)) = 0x0805B894;
		signature(&fraction, from, to, nullVector, nullVector, 0, 1/*MASK=CONTENTS_SOLID*/);
		//printf("traceFraction((%.2f, %.2f, %.2f), (%.2f, %.2f, %.2f)) = fraction=%.2f\n", from[0],from[1],from[2], to[0],to[1],to[2], fraction);
		return stackPushFloat(fraction);
	}
	
	#if COMPILE_MYSQL == 1
	switch (function) {
		case 150: return gsc_mysql_stmt_init();
		case 151: return gsc_mysql_stmt_close();
		case 152: return gsc_mysql_stmt_get_stmt_id();
		case 153: return gsc_mysql_stmt_get_prefetch_rows();
		case 154: return gsc_mysql_stmt_get_param_count();
		case 155: return gsc_mysql_stmt_get_field_count();
		case 156: return gsc_mysql_stmt_prepare();
		case 157: return gsc_mysql_stmt_bind_param();
		case 158: return gsc_mysql_stmt_bind_result();
		case 159: return gsc_mysql_stmt_execute();
		case 160: return gsc_mysql_stmt_store_result();
		case 161: return gsc_mysql_stmt_fetch();
		
		#if COMPILE_MYSQL_TESTS == 1
		case 180: return gsc_mysql_test_0(); // simple connect, query, disconnect
		case 181: return gsc_mysql_test_1(); // same like _0, with stored procedures
		#endif
	}
	#endif
	
	/*
		201: int stackPushArray()
		202: int stackSetKeyInArray(int precachedStringOffset)
	*/
	switch (function)
	{
		case 201:
		{
			printf("int stackPushArray()\n");
			
			//aStackElement *currentStack = (aStackElement *) 0x08297500;
			
			aStackElement *currentStack = *(aStackElement**)getStack();
			
			// got a real function now
			//int (*sub_807BCA8)();
			//*(int *)(&sub_807BCA8) = 0x0807BCA8;
			
			
			stackNew();
			currentStack->type = STACK_OBJECT;
			currentStack->offsetData = (void*)sub_807BCA8();
			
	int key1 = 0; // precached string "key1"
	int key2 = 0;
	memcpy(&key1, (void *)0x08853240, 2); // jeah it workes!
	memcpy(&key2, (void *)0x08853242, 2);

	stackPushFloat(1336.00);
	sub_80853B6(key1);
	//stackSetKeyInArray(key1);
	
	
	stackPushFloat(1337.00);
	sub_80853B6(key2);
	//stackSetKeyInArray(key2);
			
			
			return (int)currentStack->offsetData; // seems that i can return every shit... but this is like in ida
		}
		case 202:
		{
			printf("int stackSetKeyInArray(int precachedStringOffset)\n");
			return stackReturnInt(1);
		}
		case 204:
		{
			printf("script_spawnstruct_808523c()\n");
			int ret;
			
			int (*tmp)();
			*(int *)&tmp = 0x0808523c;
			ret = tmp();
			
			
			//return stackReturnInt(1);
			return ret;
		}
	}

	#if COMPILE_CAR == 1
	switch (function)
	{
		case 600: return gsc_car_new();
		case 601: return gsc_car_update();
	}
	#endif
	
	//#if COMPILE_CODSCRIPT == 1
	switch (function)
	{
		case 700:
			int (*sub_81100AC)(const char *file, const char *function, int isOne);

			*(int *)&sub_81100AC = 0x081100AC;
			
			printf("before func_ref!\n");
			int func_ref;
			func_ref = sub_81100AC("std/test", "helloworld", 1);
			printf("after func_ref!\n");
			return stackReturnInt(func_ref);
			
		case 701:
		{
			printf("701\n");
#if 1
#define MAX_OSPATH 256
		
#define MAX_ZPATH           256
#define MAX_SEARCH_PATHS    4096
#define MAX_FILEHASH_SIZE   1024

typedef struct fileInPack_s {
	char                    *name;      // name of the file
	unsigned long pos;                  // file info position in zip
	struct  fileInPack_s*   next;       // next file in the hash
} fileInPack_t;

typedef struct {
	char pakFilename[MAX_OSPATH];               // c:\quake3\baseq3\pak0.pk3
	char pakBasename[MAX_OSPATH];               // pak0
	char pakGamename[MAX_OSPATH];               // baseq3
	/*unzFile*/ void* handle;                             // handle to zip file
	int checksum;                               // regular checksum
	int pure_checksum;                          // checksum for pure
	int numfiles;                               // number of files in pk3
	int referenced;                             // referenced file flags
	int hashSize;                               // hash table size (power of 2)
	fileInPack_t*   *hashTable;                 // hash table
	fileInPack_t*   buildBuffer;                // buffer with the filenames etc.
} pack_t;

typedef struct {
	char path[MAX_OSPATH];              // c:\quake3
	char gamedir[MAX_OSPATH];           // baseq3
} directory_t;

typedef struct searchpath_s {
	struct searchpath_s *next;

	pack_t      *pack;      // only one of pack / dir will be non NULL
	directory_t *dir;
} searchpath_t;

typedef struct aSearchPath_t{
	struct aSearchPath_t *next;
	pack_t *pack;
	directory_t *dir;
	int *d;
} aSearchPath;
#endif
			searchpath_t *fs_searchpaths = (searchpath_t *)0x0849ECE8;
			searchpath_t *search;
			
			for (search=fs_searchpaths->next/*first element is fucked*/; search; search=search->next)
			{
				//printf("searhpath %.8p\n", search);
				//printf("searhpath %.8p %s\n", search, search->dir->path);
				printf("next:%p pack:%p dir:%p", search->next, search->pack, search->dir);
				if ((int)search->pack /*& 0xffff0000*/)
				{
					printf(" pack:");
					printf(" filename:%s", search->pack->pakFilename);
					printf(" basename:%s", search->pack->pakBasename);
				}
				if ((int)search->dir)
				{
					printf(" dir:");
					printf(" path:%s", search->dir->path);
					printf(" gamedir:%s", search->dir->gamedir);
				}
				printf("\n");
			}
			
			return stackReturnInt(1);
		}
			
		case 702:
		
			printf("trying to call codecallback_startgametype():\n");
			void (*call_startgametype_81181C6)();
			*(int *)&call_startgametype_81181C6 = 0x081181C6;
			call_startgametype_81181C6();
		
			return stackPushUndefined();
		case 703:
		{
			short ret;
			
			printf("trying to call codecallback_playerdamage():\n");
			
			/*stackPushString("0");
			stackPushString("1");
			stackPushString("2");
			stackPushString("3");
			stackPushString("4");
			stackPushString("5");
			stackPushString("6");
			stackPushString("7");
			stackPushString("8");
			stackPushString("9");*/
			
			short (*call_callback_entity_811B128)(int, int, int);
			*(int *)&call_callback_entity_811B128 = 0x0811B128;
			ret = call_callback_entity_811B128(0x08716400, *(int*)0x087B0698, 0);

			int (*call_callback_second_808410A)(short);
			*(int *)&call_callback_second_808410A = 0x0808410A;
			call_callback_second_808410A(ret);
		
			return stackPushUndefined();
		}
	}
	//#endif

	//#if COMPILE_UTILS == 1
	switch (function)
	{
		case 906: {
			/*
				svs_clients = svs_clients_842200C;
				while ( v4 < *(_DWORD *)(dword_849E6CC + 8) )
				{
				  if ( *(_DWORD *)svs_clients > 2 )
					SV_AddServerCommand_8094698(svs_clients, (char *)fmt);
				  ++v4;
				  svs_clients += 495716;
				}
			*/
		
		/*
			void (*SV_AddServerCommand)(int client, char *msg, char *msg2);
			(*(int *)&SV_AddServerCommand) = 0x08094698;
		
			SV_AddServerCommand(0x0842200C, "print yoooo", "print yoooo");
			//SV_AddServerCommand(0, "print yoooo");
		*/
		
			void (*SV_SendServerCommand)(/*client_t*/unsigned int *client, int bla, const char *fmt, ...);
			(*(int *)&SV_SendServerCommand) = 0x08094958;
			
			//SV_SendServerCommand(NULL, 0, "f \"Un^9Real.JumperZ^2/^7IzNoGoD ^7quit the game\"");
			SV_SendServerCommand((unsigned int *)0xec859008, 0, "h \"some chat msg\"");
		
			return stackPushInt(1);
		}

		case 1204:
		{
			char *msg;
			int helper = 0;
			helper += stackGetParamString(1, &msg); // todo: is string?

			if (helper < 1)
			{
				printf("scriptengine> wrongs args for: say(): at least 1 arg\n");
				return stackReturnInt(0);
			}

			Cmd_ExecuteString(msg);

			return stackPushInt(1);
		}
	}

	int (*BG_PlayAnim)(int ps, int animIndex, int bodyPart, int is_0, int setTimer, int isContinue, int force);
	*(int *)&BG_PlayAnim = 0x080D8F92;
	if (0) printf("newAnim=%.8x %.8x %.8x\n",
		INT(0x0856E3A4) + 96 * 0x118 + 72,
		INT((0x0856E3A4) + 96 * 0x118 + 72),
		INT((0x0856E3A4) + 96 * 0x118 + 72) + 50
	);
	switch (function)
	{
	
		// *(_DWORD *)(dword_856E3A4 + 96 * animIndex + 72) + 50;
		// ==  *(085b2940 + 96*280 + 72) + 50 // reload
		// ==    140192064 + 26880 
		// ==        85B9240        +72
		// ==          *(85B9288) + 50
		
		
		// ==  *(085b2940 + 96*280 + 72) + 50 // reload
		// ==    140192064 + 26880 
		// ==        85B9240        +72
		// ==          *(85B9288) + 50
		// console.log("Address: " + (0x085b2940 + 96*(animIndex=0x118)).toString(16));
		// Address: 85b9240 
		
		// dword_856E3A4 = 0x085b2940;
		// animIndex = 0x118;
		// address = dword_856E3A4 + 96 * animIndex + 72
		// address.toString(16);
		// = 85b9288
		//                                         ps          animIndex   bodyPart    is_0/time   setTimer, isContinue, force
		case 1100: return stackPushInt(BG_PlayAnim(0x08705480, 0x00000118, 0x00000002, 0x00/*000567*/, 1, 0, 1)); // reload
		case 1101: return stackPushInt(BG_PlayAnim(0x08705480, 0x00000082, 0x00000003, 0x00/*000037*/, 1, /*0*/1, 1)); // strafejump
		case 1102: return stackPushInt(BG_PlayAnim(0x08705480, 0x00000083, 0x00000003, 0x00/*000037*/, 1, 0, 1)); // oO 1
		case 1103: return stackPushInt(BG_PlayAnim(0x08705480, 0x00000080, 0x00000003, 0x00/*000037*/, 1, 0, 1)); // oO 2
		case 1104: return stackPushInt(BG_PlayAnim(0x08705480, 0x00000000, 0x00000002, 0x00/*001000*/, 1, 1, 1)); // nothing
		case 1105: return stackPushInt(BG_PlayAnim(0x08705480, 0x00000085, 0x00000003, 0x00/*000600*/, 1, 1, 1)); // highjump
		case 1106: return stackPushInt(BG_PlayAnim(0x08705480, 0x00000081, 0x00000003, 0x00/*000037*/, 1, 1, 1)); // highjump
		case 1107: return stackPushInt(BG_PlayAnim(0x08705480,          6, 0x00000003, 0x00/*000037*/, 1, 1, 1));
	}
	
	/*
BG_PlayAnim: ps=0x08705480 animIndex=0x00000085 bodyPart=0x00000003 is_0:0x00000037 setTimer=0x00000001, isContinue=(nil) force=0x00000001
Bad rcon from 127.0.0.1:-12058:
status
Bad rcon from 127.0.0.1:-12058:
say
Your MUM^7: jump
BG_PlayAnim: ps=0x08705480 animIndex=0x00000081 bodyPart=0x00000003 is_0:0x00000037 setTimer=0x00000001, isContinue=(nil) force=0x00000001
BG_PlayAnim: ps=0x08705480 animIndex=0x00000080 bodyPart=0x00000003 is_0:0x00000037 setTimer=0x00000001, isContinue=(nil) force=0x00000001
	*/
	
	switch (function)
	{
		case 1300:
			// %s.iwd fileoffset:F9122
			signed int (*FS_FOpenFileRead)(char *qpath, int *filehandle, int isOne);
			*(int *)&FS_FOpenFileRead = 0x080A0122;
			signed int size;
			int filehandle;
			size = FS_FOpenFileRead((char *)"mp_3TOW3Rs.iwd", &filehandle, 1);
			printf("filesize:%d filehandle:%.8x\n", size, filehandle);
			return stackReturnInt(1);
			
		case 1301: {
			int (*FS_AddGameDirectory)(char *path, char *dir);
			*(int *)&FS_AddGameDirectory = 0x080A28CC;
			int ret = FS_AddGameDirectory("/home/ns_test", "NsZombiesV4.3");
			printf("FS_AddGameDirectory=%d\n", ret);
			return stackReturnInt(1);
		}
	}
	
	// todo: analyse getentarray():
	// - to learn where strings are saved
	// - to learn how to return an array
	// - maybe make getarraykeys()-function
	
	// then learn about getEye() how the skeleton is organized
	// make function that returns real eyes for aiming the portals!
	
	// then make mysql-functions!
	
	
	
	
	// bytes 12 + 13 == 40 + 01 == rechte maus
	
	/*
	// AUS CMD STATUS
	playerstatus = 0x0842308C
	sizeofplayer = 0x000B1064
	qport =        0x0006E6E8 -> 8491774
	rate =         0x0006E6B8 -> 
	*/

/*
	STRANGE; es gibt noch ein anderen Player-States (e.g. used in getorigin())
	
	int __cdecl sub_81104D2(int playernum)
	{
	  int v2; // [sp+4h] [bp-4h]@2

	  if ( HIWORD(playernum) )
	  {
		com_error("not an entity");
		v2 = 0;
	  }
	  else
	  {
		v2 = (int)(&playerStates_byte_8716400 + 140 * (unsigned __int16)playernum);
	  }
	  return v2;
	}

	int __cdecl getOrigin_sub_8111AE0(__int16 playernum)
	{
	  int player; // eax@1
	  int v3; // [sp+2Ch] [bp-Ch]@1
	  float origin[3]; // [sp+10h] [bp-28h]@1

	  player = sub_81104D2(playernum);
	  v3 = player;
	  copy_float_from_to_sub_8118698((float *)(player + 312), origin);
	  return stackReturnVector_sub_8085306(origin);
	}
*/

	// todo: return undefined
	return stackReturnInt(0);
}





int sub_8101B40(int self, int eInflictor, int eAttacker, float *vDir, float *vPoint, int iDamage, int iDFlags, int iMeansOfDeath, int iHitLoc, int psOffsetTime)
{
	/*
	int (*signature)(int, int, int, float *, float *, int, int, int, int, int);
	signature = (void *)0x08101B40;
	return signature(self, eInflictor, eAttacker, vDir, vPoint, iDamage, iDFlags, iMeansOfDeath, iHitLoc, psOffsetTime);
	*/
	
	int (*signature)(int, int, int, int, int, int, int, float *, float *, int, int);
	*((int *)(&signature)) = 0x081183b8;
	
	
	
	int iWeapon = 0;
	if (eInflictor) {
		iWeapon = *(int *)(eInflictor + 200);
	} else if (eAttacker) {
		iWeapon = *(int *)(eAttacker + 200);
	}
	
	return signature(self, eInflictor, eAttacker, iDamage, iDFlags, iMeansOfDeath, iWeapon, vPoint, vDir, iHitLoc, psOffsetTime);
}


// THIS FUNCTION IS NOT NEEDED!
// ive just hooked it up to test it how its done
// i think this function segfaultet, when shooted a model with 2 players (fine with just 1)
// for security ive uncommented the hook in the lib-startup-code
// todo: move it to another .c-file (just cod2-specific via .so)

// oh fuck it, it is needed!
// how i shall know otherwise WHO did the damage (notify?)

int cdecl_cod2_player_damage_new(
	int self,
	int eInflictor,
	int eAttacker,
	float *vDir,
	float *vPoint,
	int iDamage,
	int iDFlags,
	int iMeansOfDeath,
	int iHitLoc,
	int psOffsetTime
)
{
	//abort();
	//exit(0);
	
	int gentities = 0x08716400;
	int gentities_size = 560;
	/*
	printf("#######################################\n");
	//printf("self=%d\n", (self - gentities) / gentities_size);
	printf("self=%d\n", self);
	printf("eInflictor=%.8x\n", eInflictor);
	printf("eAttacker=%.8x\n", eAttacker);
	if (vDir == NULL)
		printf("no vDir\n");
	else
		printf("vDir=%.2f %.2f %.2f\n", vDir[0], vDir[1], vDir[2]);
	
	if (vPoint == NULL)
		printf("no vPoint\n");
	else
		printf("vPoint=%.2f %.2f %.2f\n", vPoint[0], vPoint[1], vPoint[2]);
		
	printf("damage=%d\n", iDamage);
	
	printf("FLAGS=%.8x\n", iDFlags);
	printf("means of death=%d\n", iMeansOfDeath);
	printf("hit loc=%.8X\n", iHitLoc);
	printf("psOffsetTime=%d\n", psOffsetTime);
	*/
	
	int ret;
	if (*(int *)(self + 344))
	{
		ret = sub_8101B40(self, eInflictor, eAttacker, vDir, vPoint, iDamage, iDFlags, iMeansOfDeath, iHitLoc, psOffsetTime);
		//printf("+++++++++++is player!+++++++++++\n");
	} else {
		//printf("+++++++++++is NOT a player!+++++++++++\n");
		//sub_8101B40(eAttacker/*self*/, eInflictor, eAttacker, vDir, vPoint, iDamage, iDFlags, iMeansOfDeath, iHitLoc, psOffsetTime);
		ret = sub_8101B40(self, eInflictor, eAttacker, vDir, vPoint, iDamage, iDFlags, iMeansOfDeath, iHitLoc, psOffsetTime);
	}
	
	
	
	//printf("#######################################\n");
	//return stackReturnInt(1);
	return ret;
}
