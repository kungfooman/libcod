#include "gsc_utils.hpp"

#if COMPILE_UTILS == 1

void gsc_utils_disableGlobalPlayerCollision() {
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
	
	stackPushUndefined();
}

void gsc_utils_getAscii() {
	char *str;
	if ( ! stackGetParams("s", &str) || strlen(str) == 0) {
		stackPushUndefined();
		return;
	}
	stackPushInt(str[0]);
}

void gsc_utils_system() { // closer 903, "ls"
	char *cmd;
	if ( ! stackGetParams("s",  &cmd)) {
		printf("scriptengine> ERROR: please specify the command as string to gsc_system_command()\n");
		stackPushUndefined();
		return;
	}
	setenv("LD_PRELOAD", "", 1); // dont inherit lib of parent
	stackPushInt( system(cmd) );
}

void gsc_utils_file_link() {
	char *source, *dest;
	if ( ! stackGetParams("ss",  &source, &dest)) {
		printf("scriptengine> ERROR: please specify source and dest to gsc_link_file()\n");
		stackPushUndefined();
		return;
	}
	stackPushInt( link(source, dest) ); // 0 == success
}

void gsc_utils_file_unlink() {
	char *file;
	if ( ! stackGetParams("s",  &file)) {
		printf("scriptengine> ERROR: please specify file to gsc_unlink_file()\n");
		stackPushUndefined();
		return;
	}
	stackPushInt( unlink(file) ); // 0 == success
}

void gsc_utils_file_exists() {
	char *filename;
	if ( ! stackGetParams("s", &filename)) {
		stackPushUndefined();
		return;
	}
	stackPushInt( ! (access(filename, F_OK) == -1) );
}

void gsc_utils_FS_LoadDir() {
	char *path, *dir;
	if ( ! stackGetParams("ss", &path, &dir)) {
		stackPushUndefined();
		return;
	}
	//printf("path %s dir %s \n", path, dir);
	stackPushInt( FS_LoadDir(path, dir) );
}

void gsc_utils_getType() {
	if (stackGetNumberOfParams() == 0) {
		stackPushUndefined();
		return;
	}
	stackPushString( stackGetParamTypeAsString(0) );
}

void gsc_utils_stringToFloat() {
	char *str;
	if ( ! stackGetParams("s", &str)) {
		stackPushUndefined();
		return;
	}
	stackPushFloat( atof(str) );
}

// rundll("print.so", "test_print")
void gsc_utils_rundll() {
	char *arg_library, *arg_function;

	if ( ! stackGetParams("ss", &arg_library, &arg_function)) {
		printf("scriptengine> wrongs args for: dlcall(library, function)\n");
		stackPushUndefined();
		return;
	}
	
	printf("lib=%s func=%s\n", arg_library, arg_function);
	
	//void *handle = dlopen(arg_library, RTLD_GLOBAL); // crashes
	// void *handle = dlopen(arg_library, RTLD_LOCAL); // crashes
	//void *handle = dlopen(arg_library, RTLD_NOW); // crashes
	void *handle = dlopen(arg_library, RTLD_LAZY);

	if ( ! handle) {
		printf("ERROR: dlopen(\"%s\") failed!\n", arg_library);
		stackPushInt(0);
		return;
	}
		
	printf("dlopen(\"%s\") returned: %.8x\n", arg_library, handle);
	
	void (*func)();
	//*((void *)&func) = dlsym(handle, arg_function);
	*(int *)&func = (int)dlsym(handle, arg_function);
	if (!func) {
		printf("ERROR: dlsym(\"%s\") failed!\n", arg_function);
		stackPushInt(0);
		return;
	}
	
	printf("function-name=%s -> address=%.8x\n", arg_function, func);
	func();
	dlclose(handle);
	stackPushInt(1);
}

int gsc_utils_RemoveCommand()
{
	char *command;

	if (stackGetNumberOfParams() < 2) // function, command
	{
		printf_hide("scriptengine> ERROR: please specify atleast 2 arguments to gsc_utils_RemoveCommand()\n");
		return stackPushInt(0);
	}

	if (!stackGetParamString(1, &command))
	{
		printf_hide("scriptengine> ERROR: closer(): param \"command\"[1] has to be a string!\n");
		return stackPushInt(0);
	}

	Cmd_RemoveCommand(command);
	return stackPushInt(1);
}

#endif
