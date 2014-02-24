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

typedef int (*Cmd_RemoveCommand_t)(const char *cmd_name);
#if COD_VERSION == COD2_1_0
	static Cmd_RemoveCommand_t Cmd_RemoveCommand = (Cmd_RemoveCommand_t)0x0806052A;
#elif COD_VERSION == COD2_1_2
	static Cmd_RemoveCommand_t Cmd_RemoveCommand = (Cmd_RemoveCommand_t)0x08060736;
#elif COD_VERSION == COD2_1_3
	static Cmd_RemoveCommand_t Cmd_RemoveCommand = (Cmd_RemoveCommand_t)0x0806072E;
#else
	#warning static Cmd_RemoveCommand_t Cmd_RemoveCommand = (Cmd_RemoveCommand_t)NULL;
	static Cmd_RemoveCommand_t Cmd_RemoveCommand = (Cmd_RemoveCommand_t)NULL;
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
	static int fsrestrict_ServerCommand = 0x0808C8F2;
#elif COD_VERSION == COD2_1_2
	static int hook_AuthorizeState_call = 0x0808DA52;
	static int fsrestrict_ServerCommand = 0x0808DAAF;
#elif COD_VERSION == COD2_1_3
	static int hook_AuthorizeState_call = 0x0808DB12;
	static int fsrestrict_ServerCommand = 0x0808DB6F;
#else
	#warning static int hook_AuthorizeState_call = NULL;
	#warning static int fsrestrict_ServerCommand = NULL;
	static int hook_AuthorizeState_call = (int)NULL;
	static int fsrestrict_ServerCommand = (int)NULL;
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
