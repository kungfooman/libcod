#include "gsc_utils.hpp"

#if COMPILE_UTILS == 1

#include <dirent.h> // dir stuff
#include <assert.h>

#define MAX_LANGUAGES 16
#define MAX_LANGUAGE_ITEMS 1024

// 1.2 0x080F6D5A
int utils_hook_player_eject(int player) { // player 0 = 0x08679380 + 0x11c = 0x0867949c
	//printf("int hook_player_eject(int player=%.8x)\n", player);
	return 0;
}

int languages_defined = 0;
int language_items_defined = 0;
char languages[MAX_LANGUAGES][3]; //add space for \0
char *language_items[MAX_LANGUAGE_ITEMS];
char *language_references[MAX_LANGUAGES][MAX_LANGUAGE_ITEMS];
bool language_reference_mallocd[MAX_LANGUAGES][MAX_LANGUAGE_ITEMS];

void gsc_add_language()
{
	char *str;
	if(!stackGetParamString(0, &str))
	{
		printf("Param 0 needs to be a string for add_language\n");
		stackPushUndefined();
		return;
	}
	if(str[0] == '\0' || str[1] == '\0' || str[2] != '\0')
	{
		printf("Languages are defined by 2 characters\n");
		stackPushUndefined();
		return;
	}
	for(int i = 0; i < languages_defined; i++)
	{
		if(!strcmp(languages[i], str))
		{
			printf("%s is already an added language\n", str);
			stackPushUndefined();
			return;
		}
	}
	if(languages_defined == MAX_LANGUAGES)
	{
		printf("Cannot add another language. Already got %d languages\n", MAX_LANGUAGES);
		stackPushUndefined();
		return;
	}
	strcpy(languages[languages_defined], str);
	languages_defined++;
	//printf("Added %s as language %d\n", str, languages_defined);
	stackPushInt(0);
}

void add_lang_item(char* lang, char* item, char* txt)
{
	//printf("adding %s to %s, contents: %s\n", item, lang, txt);
	int language_number = -1;
	for(int i = 0; i < languages_defined; i++)
	{
		if(languages[i][0] == lang[0] && languages[i][1] == lang[1])
		{
			language_number = i;
			break;
		}
	}
	if(language_number == -1)
	{
		printf("Language (%s) not added\n", lang);
		return;
	}
	int language_item_number = language_items_defined;
	for(int i = 0; i < language_items_defined; i++)
	{
		if(!strcmp(language_items[i], item))
		{
			language_item_number = i;
			break;
		}
	}
	if(language_item_number == MAX_LANGUAGE_ITEMS)
	{
		printf("Maximum language items reached\n");
		return;
	}
	bool fill_other_langs = false;
	if(language_item_number == language_items_defined)
	{
		//printf("malloccing item\n");
		char *item_m = (char*)malloc(sizeof(char) * (COD2_MAX_STRINGLENGTH + 1));
		if(item_m == NULL)
		{
			printf("Could not malloc\n");
			return;
		}
		fill_other_langs = true;
		strncpy(item_m, item, COD2_MAX_STRINGLENGTH);
		language_items[language_item_number] = item_m;
		language_items_defined++;
		for(int i = 0; i < languages_defined; i++)
			language_reference_mallocd[i][language_item_number] = false;
	}
	char *txt_m;
	if(!language_reference_mallocd[language_number][language_item_number])
	{
		//printf("malloccing text\n");
		txt_m = (char*)malloc(sizeof(char) * (COD2_MAX_STRINGLENGTH + 1));
		if(txt_m == NULL)
		{
			printf("Could not malloc\n");
			return;
		}
		language_reference_mallocd[language_number][language_item_number] = true;
		language_references[language_number][language_item_number] = txt_m;
	}
	{
		//printf("reusing previous malloc\n");
 		txt_m = language_references[language_number][language_item_number];
	}
	strncpy(txt_m, txt, COD2_MAX_STRINGLENGTH);
	if(fill_other_langs)
	{
		//printf("filling other items\n");
		for(int i = 0; i < languages_defined; i++)
		{
			if(i == language_number)
				continue;
			language_references[i][language_item_number] = txt_m;
		}
	}
}

void gsc_load_languages()
{
	static bool loaded = false;
	char *str;
	if(!stackGetParamString(0, &str))
	{
		printf("Param 0 needs to be a string for load_languages\n");
		stackPushUndefined();
		return;
	}

	int force_reload;
	if(!stackGetParamInt(1, &force_reload))
		force_reload = 0;
	if(!force_reload && loaded)
	{
		printf("Already loaded languages\n");
		stackPushUndefined();
		return;
	}
	char curitem[COD2_MAX_STRINGLENGTH + 1] = "";
	char buffer[COD2_MAX_STRINGLENGTH + 1];
	bool item_found = false;
	FILE * file;
	file = fopen(str, "r");
	int linenum = 0;
	if(file != NULL)
	{
		while(fgets(buffer, sizeof(buffer), file) != NULL)
		{
			linenum++;
			if(!strncmp(buffer, "REFERENCE", 9))
			{
				//read the rest of buffer, starting from the first non-space character
				int start = -1;
				int end = -1;
				for(int i = 9; i < COD2_MAX_STRINGLENGTH; i++)
				{
					if(buffer[i] == '\0' || buffer[i] == '\r' || buffer[i] == '\n')
					{
						end = i;
						if(end - start > 0)
						{
							//string has a length
							//set it as curitem
							strncpy(curitem, &(buffer[start]), end - start);
							curitem[end - start] = '\0';
							//printf("Read item: %s", curitem);
							item_found = true;
						}
						break;
					}
					else if(start == -1 && buffer[i] != ' ' && buffer[i] != '\t')
						start = i;
					else if(start != -1 && (buffer[i] == ' ' || buffer[i] == '\t'))
					{
						//error, trailing whitespace
						//try to cut it off
						end = i;
						if(end - start > 0)
						{
							//string has a length
							//set it as curitem
							strncpy(curitem, &(buffer[start]), end - start);
							curitem[end - start] = '\0';
							//printf("Read item: %s", curitem);
							item_found = true;
						}
						break;
					}
				}
			}
			else if(!strncmp(buffer, "LANG_", 5))
			{
				//language is the [5] and [6]th element of this string
				//rest of string, starting at the first " is the string, ending at the last "
				bool lang_exist = false;
				for(int i = 0; i < languages_defined; i++)
				{
					if(languages[i][0] == buffer[5] && buffer[5] != '\0' && languages[i][1] == buffer[6] && buffer[6] != '\0')
					{
						lang_exist = true;
						break;
					}
				}
				if(!lang_exist)
				{
					if(buffer[5] != '\0' && buffer[6] != '\0')
						printf("Language not yet added for language: %c%c\n", buffer[5], buffer[6]);
					else
						printf("Line ended too soon on line %d\n", linenum);
				}
				else
				{
					//start scanning buffer, starting from 8
					char lang[2];
					lang[0] = buffer[5];
					lang[1] = buffer[6];
					int start = -1;
					int end = -1;
					bool ignore_next = false;
					for(int i = 8; i < COD2_MAX_STRINGLENGTH; i++)
					{
						if(buffer[i] == '\\' && !ignore_next)
						{
							ignore_next = true;
							continue;
						}
						if(buffer[i] == '\0' || buffer[i] == '\r' || buffer[i] == '\n')
						{
							//string ended prematurely
							printf("Error in line %d\n", linenum);
							break;
						}
						if(buffer[i] == '\"' && !ignore_next)
						{
							if(start == -1)
							{
								if(buffer[i + 1] == '\0')
								{
									printf("Premature line end on line %d", linenum);
									break;
								}
								else
									start = i + 1;
							}
							else
							{
								end = i;
								//add buffer to languages stuff
								if(end - start > 0)
								{
									char curdesc[COD2_MAX_STRINGLENGTH + 1];
									strncpy(curdesc, &(buffer[start]), end - start);
									curdesc[end - start] = '\0';
									add_lang_item(lang, curitem, curdesc);
									//printf("Adding %s as %s for language %c%c\n", curdesc, curitem, buffer[5], buffer[6]);
								}
								break;
							}
						}
						ignore_next = false;
					}
				}
			}
		}
		fclose(file);
	}
	else
	{
		printf("File %s does not exist\n", str);
		return;
	}
}

void gsc_get_language_item()
{
	char *str;
	char *str2;
	if(!stackGetParamString(0, &str))
	{
		printf("Param 0 has to be a string for get_language_item\n");
		stackPushUndefined();
		return;
	}
	if(!stackGetParamString(1, &str2))
	{
		printf("Param 1 has to be a string for get_language_item\n");
		stackPushUndefined();
		return;
	}
	//printf("str: %s, str2: %s\n", str, str2);
	if(str[0] == '\0' || str[1] == '\0')
	{
		printf("Invalid language item requested. Should be like EN\n");
		stackPushUndefined();
		return;
	}
	int language_number = -1;
	for(int i = 0; i < languages_defined; i++)
	{
		if(str[0] == languages[i][0] && str[1] == languages[i][1])
		{
			//found a match
			language_number = i;
			break;
		}
	}
	if(language_number == -1)
	{
		printf("Invalid language selected. Load languages first\n");
		stackPushUndefined();
		return;
	}
	int language_item_number = -1;
	for(int i = 0; i < language_items_defined; i++)
	{
		if(!strcmp(str2, language_items[i]))
		{
			//found match
			language_item_number = i;
			break;
		}
	}
	if(language_item_number == -1)
	{
		printf("Invalid language item selected. Load language items first\n");
		stackPushString(str2);
		return;
	}
	//printf("found: %s\n", language_references[language_number][language_item_number]);
	stackPushString(language_references[language_number][language_item_number]);
}

void gsc_utils_sprintf()
{
	int max_iterations = stackGetNumberOfParams();
	if(max_iterations == 0)
	{
		printf("scriptengine> At least 1 argument required for sprintf\n");
		stackPushUndefined();
		return;
	}
	char result[2 * COD2_MAX_STRINGLENGTH] = ""; //output buffer, cod string size should be enough
	char *str;
	if(!stackGetParamString(0, &str))
	{
		printf("Param 0 needs to be a string for sprintf\n");
		stackPushUndefined();
		return;
	}
	unsigned int i = 0;
	unsigned int current_char = 0; //in result, from which character to start the next write
	unsigned int prev_char = 0; //in str (have copied up to here, not including this one
	int iteration_count = 0;
	while(str[i] != '\0')
	{
		if(current_char > COD2_MAX_STRINGLENGTH)
		{
			printf("scriptengine> Error, string in sprintf is larger than the max stringsize in cod2\n");
			stackPushUndefined();
			return;
		}
		if(str[i] == '%')
		{
			unsigned int j = i + 1;
			bool found = false;
			while(str[j] != '\0' && !found)
			{
				switch(str[j])
				{
					case 'd':
					case 'i':
					case 'u':
					case 'o':
					case 'x':
					case 'X':
					case 'f':
					case 'F':
					case 'e':
					case 'E':
					case 'g':
					case 'G':
					case 'a':
					case 'A':
					case 'c':
					case 's':
					case 'p':
					case 'n':
					case '%':
						found = true;
						break;
					default:
						j++;
						break;
				}
			}
			if(str[j] == '\0')
			{
				printf("scriptengine> error. Incorrect sprintf string input\n");
				stackPushUndefined();
				return;
			}
			char replaced_char = str[j + 1];
			str[j + 1] = '\0';

			int chars_written;
			if(str[j] != '%')
			{
				iteration_count++;
				if(iteration_count == max_iterations)
				{
					printf("scriptengine> Not enough input arguments for sprintf to make cake\n");
					stackPushUndefined();
					return;
				}

				int stackint;
				float stackfloat;
				char *stackstring;
				int type = 0;
				switch(stackGetParamType(iteration_count))
				{
					case STACK_STRING:
					{
						stackGetParamString(iteration_count, &stackstring);
						type = 1;
						break;
					}
					case STACK_FLOAT:
					{
						stackGetParamFloat(iteration_count, &stackfloat);
						type = 2;
						break;
					}
					case STACK_INT:
					{
						stackGetParamInt(iteration_count, &stackint);
						type = 3;
						break;
					}
					default:
					{
						printf("scriptengine> Unsupported input type for sprintf. Supported: string, int, float\n");
						stackPushUndefined();
						return;
					}
				}
				if(type == 1)
				{
					chars_written = sprintf(&(result[current_char]), &(str[prev_char]), stackstring);
				}
				else if(type == 2)
				{
					chars_written = sprintf(&(result[current_char]), &(str[prev_char]), stackfloat);
				}
				else if(type == 3)
				{
					chars_written = sprintf(&(result[current_char]), &(str[prev_char]), stackint);
				}
				else
				{
					printf("scripteninge> Something bad happened during sprintf\n");
					stackPushUndefined();
					return;
				}
			}
			else
			{
				chars_written = sprintf(&(result[current_char]), &(str[prev_char]), '\0'); //doesnt actually print that but well, needs some input
			}
			if(chars_written < 0)
			{
				printf("scriptengine> error. Incorrect sprintf input\n");
				stackPushUndefined();
				return;
			}
			str[j + 1] = replaced_char;
			current_char += chars_written;
			prev_char = j + 1;
			i = j;
		}
		i++;
	}
	strncpy(&(result[current_char]), &(str[prev_char]), i - prev_char); //copy the remaining part into the buffer
	/*int k = 0;
	while(result[k] != '\0')
	{
		printf("%c", result[k]);
		k++;
	}
	printf("\n");*/
	stackPushString(result);
	return;
}

void gsc_utils_disableGlobalPlayerCollision() {
	// well, i could also just write LEAVE,RETN C9,C3 at beginnung of function
	#  if COD_VERSION == COD2_1_0
		cracking_write_hex(0x080F474A, (char *)"C3");
		cracking_write_hex(0x080F5199, (char *)"02");
		cracking_write_hex(0x0805AA0E, (char *)"C3");
	#elif COD_VERSION == COD2_1_2
		////ret = cracking_nop(0x080F6D5A, 0x080F7150);
		//ret = cracking_nop(0x080F6E82, 0x080F7150); // requires setcontents(0) hack and brushmodels arent working
		cracking_write_hex(0x080F6D5A, (char *)"C3");
		cracking_write_hex(0x080F77AD, (char *)"02");
		cracking_write_hex(0x0805AC1A, (char *)"C3");
	
		cracking_hook_function(0x80F6D5A, (int)utils_hook_player_eject);
		cracking_hook_function(0x80F553E, (int)utils_hook_player_eject); //g_setclientcontents
		#if 0
			//just a quick snippet for if u want to switch to turn it on or off
			unsigned char on[5] = {0x90};
			unsigned char off[5] = {0xe8, 0xbd, 0xf5, 0xff, 0xff};
			memcmp((void*)0x80F6D5A, on, 5);
		#endif
	#elif COD_VERSION == COD2_1_3
		//ret = cracking_nop(0x080F6E9E, 0x080F7294);
		//ret = cracking_nop(0x080F6FC6, 0x080F7294);
		cracking_write_hex(0x080F6E9E, (char *)"C3");
		cracking_write_hex(0x080F78F1, (char *)"02");
		cracking_write_hex(0x0805AC12, (char *)"C3");
	
		cracking_hook_function(0x80F6E9E, (int)utils_hook_player_eject);
		cracking_hook_function(0x80F5682, (int)utils_hook_player_eject); //g_setclientcontents
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
		
	printf("dlopen(\"%s\") returned: %.8x\n", arg_library, (unsigned int)handle);
	
	void (*func)();
	//*((void *)&func) = dlsym(handle, arg_function);
	*(int *)&func = (int)dlsym(handle, arg_function);
	if (!func) {
		printf("ERROR: dlsym(\"%s\") failed!\n", arg_function);
		stackPushInt(0);
		return;
	}
	
	printf("function-name=%s -> address=%.8x\n", arg_function, (unsigned int)func);
	func();
	dlclose(handle);
	stackPushInt(1);
}

void gsc_utils_ExecuteString() {
	char *str;
	if ( ! stackGetParams("s", &str)) {
		stackPushUndefined();
		return;
	}

	Cmd_ExecuteString(str);
	stackPushInt(1);
}

void gsc_utils_sendgameservercommand() {
	int clientNum;
	char *message;

	if ( ! stackGetParams("is", &clientNum, &message)) {
		stackPushUndefined();
		return;
	}

	SV_GameSendServerCommand(clientNum, 0, message);

	stackPushInt(1);
}

void gsc_utils_scandir() {
	char *dirname;
	if ( ! stackGetParams("s", &dirname)) {
		stackPushUndefined();
		return;
	}
	DIR *dir;
	struct dirent *dir_ent;
	dir = opendir(dirname);
	if ( ! dir) {
		stackPushUndefined();
		return;
	}
	stackPushArray();
	while (dir_ent = readdir(dir)) {
		stackPushString(dir_ent->d_name);
		stackPushArrayLast();
	}
	closedir(dir);
}

void gsc_utils_fopen() {
	char *filename, *mode;
	if ( ! stackGetParams("ss", &filename, &mode)) {
		stackPushUndefined();
		return;
	}
	FILE *file = fopen(filename, mode);
	stackPushInt((int)file);
}

void gsc_utils_fread() {
	FILE *file;
	if ( ! stackGetParams("i", &file)) {
		stackPushUndefined();
		return;
	}
	assert(file);
	char buffer[256];
	int ret = fread(buffer, 1, 255, file);
	if ( ! ret) {
		stackPushUndefined();
		return;
	}
	buffer[ret] = '\0';
	stackPushString(buffer);
}

void gsc_utils_fwrite() {
	FILE *file;
	char *buffer;
	if ( ! stackGetParams("is", &file, &buffer)) {
		stackPushUndefined();
		return;
	}
	assert(file);
	int bytesWritten = fwrite(buffer, 1, strlen(buffer), file);
	stackPushInt(bytesWritten);
}

void gsc_utils_fclose() {
	FILE *file;
	if ( ! stackGetParams("i", &file)) {
		stackPushUndefined();
		return;
	}
	assert(file);
	stackPushInt( fclose(file) );
}
#endif
