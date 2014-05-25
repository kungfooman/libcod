#if !defined(_FUNCTIONS_HPP_) && (defined(COD2_VERSION) || defined(COD_VERSION))
#define _FUNCTIONS_HPP_

/* MAKE FUNCTIONS STATIC, SO THEY CAN BE IN EVERY FILE */

// CoD2 1.2 = 80601F2
static int trap_Argc()
{
	#if COD2_VERSION == COD2_VERSION_1_0
		return *(int *)0x0819BE80;
	#elif COD2_VERSION == COD2_VERSION_1_2
		return *(int *)0x0819E080;
	#elif COD2_VERSION == COD2_VERSION_1_3
		return *(int *)0x0819F100;
	#else
		#warning trap_Argc() return *(int *)NULL;
		return *(int *)NULL;
	#endif
}

typedef char * (*Cmd_Argv_t)(int arg);
#if COD_VERSION == COD2_1_0
	static Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x0806001C;
#elif COD_VERSION == COD2_1_2
	static Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060228;
#elif COD_VERSION == COD2_1_3
	static Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)0x08060220;
#else
	#warning static Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)NULL;
	static Cmd_Argv_t Cmd_Argv = (Cmd_Argv_t)NULL;
#endif

typedef int (*trap_Argv_t)(unsigned int param, char *buf, int bufLen);
#if COD2_VERSION == COD2_VERSION_1_0
	static trap_Argv_t trap_Argv = (trap_Argv_t)0x08060074;
#elif COD2_VERSION == COD2_VERSION_1_2
	static trap_Argv_t trap_Argv = (trap_Argv_t)0x08060280;
#elif COD2_VERSION == COD2_VERSION_1_3
	static trap_Argv_t trap_Argv = (trap_Argv_t)0x08060278;
#else
	#warning static trap_Argv_t trap_Argv = (trap_Argv_t)NULL;
	static trap_Argv_t trap_Argv = (trap_Argv_t)NULL;
#endif

#if COD_VERSION == COD4_1_7
	typedef int (*Com_Printf_t)(int a1, const char *format, ...);
#else
	typedef int (*Com_Printf_t)(const char *format, ...);
#endif

#if COD_VERSION == COD2_1_0
    static Com_Printf_t Com_Printf = (Com_Printf_t)0x08060B2C;
#elif COD_VERSION == COD2_1_2
    static Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DF2;
#elif COD_VERSION == COD2_1_3
    static Com_Printf_t Com_Printf = (Com_Printf_t)0x08060DEA;
#elif COD_VERSION == COD4_1_7
    static Com_Printf_t Com_Printf = (Com_Printf_t)0x08122B2E;
#else
	#warning static Com_Printf_t Com_Printf = (Com_Printf_t)NULL;
	static Com_Printf_t Com_Printf = (Com_Printf_t)NULL;
#endif

/*
	search for '\"%s\" is: \"%s^7\" default: \"%s^7\"\n'
	Now see code ref. Now you need find a function that only calls that function
*/
typedef int (*Cmd_ExecuteString_t)(const char *text);

#if COD_VERSION == COD2_1_0
	static Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x08060754;
#elif COD_VERSION == COD2_1_2
	static Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609D4;
#elif COD_VERSION == COD2_1_3
	static Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x080609CC;
#elif COD_VERSION == COD4_1_7
	static Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)0x08111F32;
#else
	static Cmd_ExecuteString_t Cmd_ExecuteString = (Cmd_ExecuteString_t)NULL;
	#warning Cmd_ExecuteString_t Cmd_ExecuteString = NULL;
#endif

typedef int (*ClientCommand_t)(int clientNum);
#if COD2_VERSION == COD2_VERSION_1_0
	static ClientCommand_t ClientCommand = (ClientCommand_t)0x080FE998; // search 'say_team' and see code xref function
	static int hook_ClientCommand_call = 0x0808F281;
#elif COD2_VERSION == COD2_VERSION_1_2
	static ClientCommand_t ClientCommand = (ClientCommand_t)0x08100D1E;
	static int hook_ClientCommand_call = 0x08090B0C;
#elif COD2_VERSION == COD2_VERSION_1_3
	static ClientCommand_t ClientCommand = (ClientCommand_t)0x08100E62;
	static int hook_ClientCommand_call = 0x08090BA0;
#else
	#warning static ClientCommand_t ClientCommand = (ClientCommand_t)NULL;
	#warning static int hook_ClientCommand_call = NULL;
	static ClientCommand_t ClientCommand = (ClientCommand_t)NULL;
	static int hook_ClientCommand_call = (int)NULL;
#endif

typedef int (*Cvar_VariableValue_t)(const char *var_name);
#if COD2_VERSION == COD2_VERSION_1_0
	static Cvar_VariableValue_t CvarVariableValue = (Cvar_VariableValue_t)0x080B0BB6;
#elif COD2_VERSION == COD2_VERSION_1_2
	static Cvar_VariableValue_t CvarVariableValue = (Cvar_VariableValue_t)0x080B2E66;
#elif COD2_VERSION == COD2_VERSION_1_3
	static Cvar_VariableValue_t CvarVariableValue = (Cvar_VariableValue_t)0x080B2FAA;
#else
	#warning static Cvar_VariableValue_t CvarVariableValue = NULL;
	static Cvar_VariableValue_t CvarVariableValue = (Cvar_VariableValue_t)NULL;
#endif

typedef char * (*Cvar_VariableString_t)(const char *var_name);
#if COD_VERSION == COD2_1_0
	static Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x080B0D96;
#elif COD_VERSION == COD2_1_2
	static Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x080B3046;
#elif COD_VERSION == COD2_1_3
	static Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)0x080B318A;
#else
	#warning static Cvar_VariableValue_t CvarVariableValue = NULL;
	static Cvar_VariableString_t Cvar_VariableString = (Cvar_VariableString_t)NULL;
#endif

typedef int (*FS_ReadFile_t)(const char *qpath, void **buffer);
#if COD_VERSION == COD2_1_0
	static FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x0809E892;
#elif COD_VERSION == COD2_1_2
	static FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0958;
#elif COD_VERSION == COD2_1_3
	static FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)0x080A0A9C;
#else
	#warning static FS_ReadFile_t FS_ReadFile = NULL;
	static FS_ReadFile_t FS_ReadFile = (FS_ReadFile_t)NULL;
#endif

// e.g. FS_LoadDir("/home/ns_test", "NsZombiesV4.3");
typedef int (*FS_LoadDir_t)(char *path, char *dir);
#if COD_VERSION == COD2_1_0
	static FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A01A4;
#elif COD_VERSION == COD2_1_2
	static FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A22D8;
#elif COD_VERSION == COD2_1_3
	static FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)0x080A241C;
#else
	#warning static FS_LoadDir_t FS_LoadDir = NULL;
	static FS_LoadDir_t FS_LoadDir = (FS_LoadDir_t)NULL;
#endif

#if COD_VERSION == COD2_1_0
	static int hook_findMap_call = 0x0808AD00;
#elif COD_VERSION == COD2_1_2
	static int hook_findMap_call = 0x0808BCFC;
#elif COD_VERSION == COD2_1_3
	static int hook_findMap_call = 0x0808BDC8;
#else
	#warning static int hook_findMap_call = NULL;
	static int hook_findMap_call = (int)NULL;
#endif

#if COD_VERSION == COD2_1_0
	static int hook_AuthorizeState_call = 0x0808C8C0;
#elif COD_VERSION == COD2_1_2
	static int hook_AuthorizeState_call = 0x0808DA52;
#elif COD_VERSION == COD2_1_3
	static int hook_AuthorizeState_call = 0x0808DB12;
#else
	#warning static int hook_AuthorizeState_call = NULL;
	static int hook_AuthorizeState_call = (int)NULL;
#endif

typedef int (*SV_GameSendServerCommand_t)(int clientNum, signed int a2, const char *msg);
#if COD_VERSION == COD2_1_0
	static SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x0808FE96;
#elif COD_VERSION == COD2_1_2
	static SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080916A6;
#elif COD_VERSION == COD2_1_3
	static SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x080917AA;
#else
	#warning static SV_GameSendServerCommand_t SV_GameSendServerCommand = NULL;
	static SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)NULL;
#endif

typedef int (*SV_BeginDownload_f_t)(int a1);
static SV_BeginDownload_f_t SV_BeginDownload_f = (SV_BeginDownload_f_t)NULL;

typedef short (*codscript_call_callback_entity_t)(int self, int callback, int params);
#if COD2_VERSION == COD2_VERSION_1_0
	static codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)0x08118DF4; // search 'badMOD'

#elif COD2_VERSION == COD2_VERSION_1_2
	static codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)0x0811B128;
#elif COD2_VERSION == COD2_VERSION_1_3
	static codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)0x0811B284;
#else
	#warning static codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)NULL;
	static codscript_call_callback_entity_t codscript_call_callback_entity = (codscript_call_callback_entity_t)NULL;
#endif

typedef int (*codscript_callback_finish_t)(short callback_handle);
#if COD2_VERSION == COD2_VERSION_1_0
	static codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)0x08083B8E;
#elif COD2_VERSION == COD2_VERSION_1_2
	static codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)0x0808410A;
#elif COD2_VERSION == COD2_VERSION_1_3
	static codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)0x080841D6;
#else
	#warning static codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)NULL;
	static codscript_callback_finish_t codscript_callback_finish = (codscript_callback_finish_t)NULL;
#endif

#endif
