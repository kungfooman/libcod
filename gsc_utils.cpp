#include "gsc_utils.hpp"

//#if COMPILE_UTILS == 1

int gsc_utils_disableGlobalPlayerCollision()
{
	int ret;
	
	// well, i could also just write LEAVE,RETN C9,C3 at beginnung of function
	
	#if COD_VERSION == COD2_1_0
		cracking_write_hex(0x080F474A, (char *)"C3");
		cracking_write_hex(0x080F5199, (char *)"02");
		cracking_write_hex(0x0805AA0E, (char *)"C3");
	#endif
	#if COD_VERSION == COD2_1_2
	/*
	//ret = cracking_nop(0x080F6D5A, 0x080F7150);
	ret = cracking_nop(0x080F6E82, 0x080F7150); // requires setcontents(0) hack and brushmodels arent working
	*/
	
	cracking_write_hex(0x080F6D5A, (char *)"C3");
	cracking_write_hex(0x080F77AD, (char *)"02");
	cracking_write_hex(0x0805AC1A, (char *)"C3");
	
	#endif
	#if COD_VERSION == COD2_1_3
	//ret = cracking_nop(0x080F6E9E, 0x080F7294);
	//ret = cracking_nop(0x080F6FC6, 0x080F7294);
	
	
	cracking_write_hex(0x080F6E9E, (char *)"C3");
	cracking_write_hex(0x080F78F1, (char *)"02");
	cracking_write_hex(0x0805AC12, (char *)"C3");
	
	#endif
	
	return stackPushInt(ret);
}

int gsc_utils_ClientCommand()
{
	int clientNum;
	if ( ! stackGetParamInt(1, &clientNum))
		return stackPushUndefined();
	int ret = ClientCommand(clientNum);
	return stackPushInt(ret);
}

int gsc_utils_getAscii()
{
	char *str;
	if ( ! stackGetParamString(1, &str))
		return stackPushUndefined();
	if (strlen(str) == 0)
		return stackPushUndefined();
	return stackPushInt(str[0]);
}

int gsc_utils_system() // closer 903, "ls"
{
	char *cmd;
	if (stackGetNumberOfParams() < 2) // function, command
	{
		printf_hide("scriptengine> ERROR: please specify atleast 2 arguments to gsc_system_command()\n");
		return stackPushUndefined();
	}
	if (!stackGetParamString(1, &cmd))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"cmd\"[1] has to be a string!\n");
		return stackPushUndefined();
	}
	setenv("LD_PRELOAD", "", 1); // dont inherit lib of parent
	int ret = system(cmd);
	return stackPushInt(ret);
}
int gsc_utils_file_link()
{
	char *source;
	char *dest;
	if (stackGetNumberOfParams() < 3) // function, source, dest
	{
		printf_hide("scriptengine> ERROR: please specify atleast 3 arguments to gsc_link_file()\n");
		return stackPushUndefined();
	}
	if (!stackGetParamString(1, &source))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"source\"[1] has to be a string!\n");
		return stackPushUndefined();
	}
	if (!stackGetParamString(2, &dest))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"dest\"[2] has to be a string!\n");
		return stackPushUndefined();
	}
	int ret = link(source, dest);
	return stackPushInt(ret); // 0 == success
}
int gsc_utils_file_unlink()
{
	char *file;
	if (stackGetNumberOfParams() < 2) // function, source, dest
	{
		printf_hide("scriptengine> ERROR: please specify atleast 2 arguments to gsc_unlink_file()\n");
		return stackPushUndefined();
	}
	if (!stackGetParamString(1, &file))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"file\"[1] has to be a string!\n");
		return stackPushUndefined();
	}
	int ret = unlink(file); 
	return stackPushInt(ret); // 0 == success
}


//#endif