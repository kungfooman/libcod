#include "server.hpp"


#if COMPILE_DEBUG_SERVER == 1


#include <stdlib.h>
#include <ctype.h>
#include <string.h>

struct threadData
{
	int port;
	int debug;
};

void startServerAsThread(int port, int debug)
{
	int ret;

	pthread_t handle;
	pthread_attr_t settings;

	int stacksize;

	ret = pthread_attr_init(&settings);
	if (ret != 0)
	{
		printf_hide("> [ERROR] pthread_attr_init() failed.\n");
		return;
	}

	stacksize = 1024*1024*2;

	ret = pthread_attr_setstacksize(&settings, stacksize);
	if (ret != 0)
	{
		printf_hide("> [ERROR] pthread_attr_setstacksize failed.\n");
		return;
	}

	printf_hide("> [INFO] Stack-Size set to %d Bytes (%.2f KB, %.2f MB)\n", stacksize, (float)(stacksize/1024), (float)((stacksize/1024)/1024));

	struct threadData *tmp = (struct threadData *) malloc(sizeof(struct threadData));
	tmp->port = port;
	tmp->debug = debug;
	pthread_create(&handle, &settings, (void *)mythread, (void*)tmp);
}

static void mythread(void *arg)
{
	int ret, msglen, i;
	
	struct threadData *tmp = arg;
	
	int port = tmp->port;
	int debug = tmp->debug;
	
	/*
	#ifdef SERVER_PORT
	int port = SERVER_PORT;
	#else
	int port = 6666;
	#endif
	*/
	
	char buffer[1024];
	char bufferOut[BUFFER_OUT];
	// 1024 commands, each 16 args, each 32 bytes = 512kb
	char commands[12][16][32];

	TCP_connection conn;
	ret = TCP_server_start(&conn, port);


	
	// SIGSEGV
	#if 0
		{
		sigset_t emptyset;
		// Install the SIGSEGV handler.
		if (sigsegv_install_handler (&handler) < 0)
			exit (2);
		// Save the current signal mask.
		sigemptyset (&emptyset);
		sigprocmask (SIG_BLOCK, &emptyset, &mainsigset);
		printf_hide("> [INFO] SIGSEGV got installed\n");
		}
	#endif
	
	
	
	memset(commands, 0, sizeof(commands));

	if (!ret)
	{
		printf_hide("> [ERROR] TCP_init(port=%d) failed!\n", port);
		return;
	}

	printf_hide("> [INFO] TCP_init(port=%d)\n", port);

	while (1)
	{
		printf_hide("> [INFO] TCP_wait_for_client()... \n");
		ret = TCP_wait_for_client(&conn);
		if (!ret)
		{
			printf_hide("> [ERROR] TCP_wait_for_client() failed!\n");
			continue;
		}

		printf_hide("> [INFO] Client connected!\n");

		ret = TCP_read_all(&conn, buffer, 1024-1, &msglen);
		if (!ret)
		{
			printf_hide("> [ERROR] TCP_read_all() failed!\n");
			continue;
		}
		buffer[msglen] = '\0';

		printf_hide("> [INFO] TCP_read_all(): msglen=%d\n", msglen);
		//printf_hide("> [INFO] TCP_read_all(): msglen=%d msg=%s\n", msglen, buffer);

		// maybe its sometime usefull
		TCP_write(&conn, "{\n\t\"time\": \"%d\"", time(NULL));

		

		{
			char lol[MAX_ROWS][MAX_COLS][MAX_CHARS];
			char *cmd, *arg1, *arg2, *arg3, *arg4;
			int row, col;
			char tmp[2048];
			parseRequest(buffer, lol);

			// debug table
			for (row=0; row<MAX_ROWS; row++)
			{
				if (lol[row][0][0] == '\0')
					break;
				printf_hide("] row[%d]: ", row);
				for (col=0; col<MAX_COLS; col++)
				{
					if (lol[row][col][0] == '\0')
						break;
					printf_hide("[%d]=%s ", col, lol[row][col]);
				}
				printf_hide("\n");
			}

			
			for (row=0; row<MAX_ROWS; row++)
			{
				if (lol[row][0][0] == '\0')
					break;

				cmd = lol[row][0];
				arg1 = lol[row][1];
				arg2 = lol[row][2];
				arg3 = lol[row][3];
				arg4 = lol[row][4];

				if (!strcmp(cmd, "getpid"))
				{
					TCP_write(&conn, "pid %d\n", getpid());
				}
				if (!strcmp(cmd, "getppid"))
				{
					TCP_write(&conn, "ppid %d\n", getppid());
				}
				if (!strcmp(cmd, "add"))
				{
					float a, b;
					a = atof(arg1);
					b = atof(arg2);
					TCP_write(&conn, "%f + %f = %f\n", a, b, a+b);
				}
				// player = data 00401234 2540
				if (!strcmp(cmd, "read"))
				{
					char *type, *number;
					
					type = arg1;
					number = arg2;

					if (!strcmp(type, "float"))
					{
						int address;
						float *tmpPtr;
						sscanf(number, "%x", &address);
						tmpPtr = (float *) address;
						TCP_write(&conn, "float %x = %f\n", tmpPtr, *tmpPtr);
					}
				}
				
				if (!strcmp(cmd, "olddump")) // dump which used words, instead of bytes
				{
					int start;
					int dwords, bytes;
					int i;
					
					sscanf(arg1, "%x", &start);
					dwords = atoi(arg2);

					TCP_write(&conn, ",\n\"ret_001\":\"");
					//dump %x %d = ", start, dwords);
					
					/*
					for (i=0; i<dwords; i++)
					{
						int *data = ((void *)start)+i;
						TCP_write(&conn, "%.8x", *data);
					}*/
					
					bytes = dwords * 4;
					for (i=0; i<bytes; i++)
					{
						char *data = (char *)start;
						TCP_write(&conn, "%.2x", *(data+i) & 0xff);
						// & 0xff because:
						// query: dump 87a2518 1
						// result: ffffffc8ffffff8c0245
					}
					
					
					TCP_write(&conn, "\"");
				}
				
				// int readMemoryRange(void *from, void *buffer, int bytes)
				if (!strcmp(cmd, "dump"))
				{
					char *arg_from = arg1;
					char *arg_bytes = arg2;
					
					void *from;
					int bytes;
				
					void *buffer;
					int i;
					int ret;
					
					sscanf(arg_from, "%x", &from); // prepare from-pointer
					bytes = atoi(arg_bytes); // prepare bytes to read

					TCP_write(&conn, ",\n\t\"ret_001\": ");

					buffer = malloc(bytes);
					
					//printf_hide("MALLOC: %.8x\n", buffer);
					
					if (buffer == NULL)
					{
						//TCP_write(&conn, ",\n\t\"ret_001\": false");
						printf_hide("MALLOC ERROR!");
						return;
					}
					
					ret = readMemoryRange(from, buffer, bytes);
					if (ret != -1)
					{						
						TCP_write(&conn, "\"");
						
						for (i=0; i<bytes; i++)
						{
							char *tmp = (char *)buffer;
							TCP_write(&conn, "%.2x", tmp[i] & 0xff);
						}
						
						TCP_write(&conn, "\"");	
					} else {
						TCP_write(&conn, "false");
						printf_hide("ERROR: readMemoryRange(0x%.8x, %.8x, %d)\n", from, buffer, bytes);
					}
					
					if (buffer != NULL)
						free(buffer);
					buffer = NULL;
				}
				
				if (!strcmp(cmd, "sizeof"))
				{
					if (!strcmp(arg1, "int"))
					{
						TCP_write(&conn, "sizeof int = %d\n", sizeof(int));
					}
					if (!strcmp(arg1, "float"))
					{
						TCP_write(&conn, "sizeof float = %d\n", sizeof(float));
					}
					if (!strcmp(arg1, "double"))
					{
						TCP_write(&conn, "sizeof double = %d\n", sizeof(double));
					}
				}
				if (!strcmp(cmd, "write"))
				{
					char *type = arg1;
					char *address = arg2;
					char *value = arg3;
					char *arg_value = arg3;
					
					
					if (!strcmp(type, "float"))
					{
						float value_float;
						float *ptr_float;
						
						// point the pointer to the address
						sscanf(address, "%x", &ptr_float);
						
						// cast the string to float
						sscanf(value, "%f", &value_float);
						
						// write the value to the address
						*ptr_float = value_float;
						
						
						TCP_write(&conn, "write to=%.8x type=float value=%f", ptr_float, value_float);
					}
					
					if (!strcmp(arg1, "int"))
					{
						int *ptr;
						int value;
						
						sscanf(address, "%x", &ptr);
						sscanf(arg_value, "%d", &value);
						
						*ptr = value;
						
						TCP_write(&conn, "write to=%.8x type=int value=%d", ptr, value);
						
					}
					
					if (!strcmp(arg1, "hex"))
					{
						unsigned char *ptr;
						unsigned char buffer[128] = {0};
						int bytes;
						int i;
						bytes = hexToBuffer(arg_value, buffer, 128);
						
						sscanf(address, "%x", &ptr);
						
						for (i=0; i<bytes; i++)
							ptr[i] = buffer[i];
						
						TCP_write(&conn, "write to=%.8x type=hex bytesWritten=%d bytes: \"", ptr, bytes);
						
						for (i=0; i<bytes; i++)
							TCP_write(&conn, "%.2x", buffer[i] & 0xff);
						TCP_write(&conn, "\"");
					}
					
					//else {
					//	printf_hide("[ERROR] unknown data-type! Usable: float, int\n");
					//}
				}
				
				if (!strcmp(cmd, "nop"))
				{
					int i;
					int from, to;
					
					sscanf(arg1, "%x", &from);
					sscanf(arg2, "%x", &to);
					
					cracking_nop(from, to);

					TCP_write(&conn, "nop from=%.8x to=%.8x", from, to);					
				}
				
				if (!strcmp(cmd, "search"))
				{
					char *arg_from = arg1;
					char *arg_to = arg2;
					char *arg_type = arg3;
					char *arg_value = arg4;
					
					char *from;
					char *to;
					char *type = arg_type;
					char *value = arg_value;
					
					sscanf(arg_from, "%x", &from);
					sscanf(arg_to, "%x", &to);
					
					printf_hide("asd from=%.8x to=%0.8x type=%s value=%s\n", from, to, type, value);
					
					if (!strcmp(type, "string"))
					{
						int len = strlen(value);
						for (; from<to; from++)
						{
							//printf_hide(".");
							if (!strncmp(from, value, len))
							{
								TCP_write(&conn, "\"%s\" found at %.8x\n", value, from);
							}
						}
					}
					
					if (!strcmp(type, "hex"))
					{
						char buffer[128];
						int bytes;
						bytes = hexToBuffer(value, buffer, 128);
						if (bytes == 0)
							continue; // TODO: LOL FIX ME!
						int len = strlen(value);
						for (; from<to; from++)
						{
							//printf_hide(".");
							if (!memcmp(from, buffer, bytes))
							{
								TCP_write(&conn, "\n\t\"hex found at %.8x\"", from);
							}
						}
					}
				}
				
				// FIND "CALL address"
				if (!strcmp(cmd, "func"))
				{
					char *arg_address = arg1;
					char *arg_which = arg2;
					
					unsigned int *address;
					char *which = arg_which;
					
					sscanf(arg_address, "%x", &address);
					
					
					TCP_write(&conn, "Overwrite Function (%s) at %.8x to NULL. Pointed to: %.8x\n", which, address, *address);
					//*address = 0xabcd;
					
					if (!strcmp(which, "shell"))
						*address = (unsigned int)cdecl_injected_shell;
					if (!strcmp(which, "closer"))
						*address = (unsigned int)cdecl_injected_closer;
					if (!strcmp(which, "return_0"))
						*address = (unsigned int)cdecl_return_0;
					if (!strcmp(which, "return_1"))
						*address = (unsigned int)cdecl_return_1;
				}
				if (!strcmp(cmd, "mprotect"))
				{
					char *arg_addr = arg1;
					char *arg_len = arg2;
					char *arg_prot = arg3;
					
					void *addr;
					size_t len;
					int prot;
					
					int ret;
					
					sscanf(arg_addr, "%x", &addr);
					len = atoi(arg_len);
					
					// default: give all rights
					prot = PROT_NONE | PROT_READ | PROT_WRITE | PROT_EXEC;
					if (strstr(arg_prot, "n"))
						prot = PROT_NONE;
					if (strstr(arg_prot, "r"))
						prot |= PROT_READ;
					if (strstr(arg_prot, "w"))
						prot |= PROT_WRITE;
					if (strstr(arg_prot, "x"))
						prot |= PROT_EXEC;

					ret = mprotect(addr, len, prot);
					
					printf_hide("mprotect(%.8x, %d, %.8x): try to give rights: ", addr, len, prot);
					if (prot == PROT_NONE)
						printf_hide("n");
					if (prot & PROT_READ)
						printf_hide("r");
					if (prot & PROT_WRITE)
						printf_hide("w");
					if (prot & PROT_EXEC)
						printf_hide("x");
					printf_hide("\n");
					
					if (ret == 0)
						printf_hide("MPROTECT: success!\n");
					else
						printf_hide("MPROTECT: error!\n");
					
					/*
					if (ret == EACCES)
					{
						printf_hide("MPROTECT: could not give the rights to the memory\n");
					}
					if (ret == EINVAL)
					{
						printf_hide("MPROTECT: addr is not a valid pointer, or not a multiple of the system page size.\n");
					}
					if (ret == ENOMEM)
					{
						printf_hide("MPROTECT: Internal kernel structures could not be allocated.\n");
					}
					*/
					// todo: output and errorhandling
					// http://linux.die.net/man/2/mprotect
				}
				
				// cant call it without stack-context (just as inline-function)
				//if (!strcmp(cmd, "playerdamage"))
				//{
				//	stackReturnInt(0);
				//	stackReturnInt(1);
				//	stackReturnInt(2);
				//	stackReturnInt(3);
				//	stackReturnInt(4);
				//	stackCallScriptFunction(0x08716400, stackCallbackPlayerDamage, 5);
				//}
				
				// this code changes the relative address of a CALL (opcode 0xE8)
				if (!strcmp(cmd, "changecall"))
				{
					char *arg_address_of_call = arg1;
					char *arg_new_function = arg2;
					
					unsigned int address_of_call;
					unsigned int new_function;
					
					sscanf(arg_address_of_call, "%x", &address_of_call);
					
					new_function = 0;
					// todo: make function table out of it!
					if (!strcmp(arg_new_function, "shell"))
						new_function = (unsigned int)cdecl_injected_shell;
					if (!strcmp(arg_new_function, "print"))
						new_function = (unsigned int)cdecl_print;
					if (!strcmp(arg_new_function, "closer"))
						new_function = (unsigned int)cdecl_injected_closer;
					if (!strcmp(arg_new_function, "return_0"))
						new_function = (unsigned int)cdecl_return_0;
					if (!strcmp(arg_new_function, "return_1"))
						new_function = (unsigned int)cdecl_return_1;
					
					unsigned char opcode_of_call;
					memcpy(&opcode_of_call, (int *)address_of_call, 1);
					
					address_of_call += 1; // now its pointing on the relative address!
					
					int new_relative_address = new_function - (address_of_call + 5);
					
					// write the new relative address
					memcpy((int *)address_of_call, &new_relative_address, 4);
					
					TCP_write(&conn, "opcode=%.2x address_of_call=%.8x new_function=%.8x new_relative_address=%.8x\n", opcode_of_call, address_of_call, new_function, new_relative_address);
				}
				
				if (!strcmp(cmd, "funcs"))
				{
					TCP_write(&conn, "%s -> %.8x\n", "shell", (unsigned int)cdecl_injected_shell);
					TCP_write(&conn, "%s -> %.8x\n", "print", (unsigned int)cdecl_print);
					TCP_write(&conn, "%s -> %.8x\n", "closer", (unsigned int)cdecl_injected_closer);
					TCP_write(&conn, "%s -> %.8x\n", "return_0", (unsigned int)cdecl_return_0);
					TCP_write(&conn, "%s -> %.8x\n", "return_1", (unsigned int)cdecl_return_1);
				}
				
				if (!strcmp(cmd, "dlopen"))
				{
					char *arg_library = arg1;
					char *arg_function = arg2;
					
					printf_hide("%s -> %s\n", arg_library, arg_function);
					
					void *handle = dlopen(arg_library, RTLD_GLOBAL);
					
					printf_hide("dlopen(\"%s\") returned: %.8x\n", arg_library, handle);
					
					TCP_write(&conn, "function-name=%s -> address=%.8x\n", arg_function, dlsym(handle, arg_function));
					
					//dlclose(handle);
				}
				
				if (!strcmp(cmd, "dlcall"))
				{
					char *arg_library = arg1;
					char *arg_function = arg2;
					
					printf_hide("%s -> %s\n", arg_library, arg_function);
					
					//void *handle = dlopen(arg_library, RTLD_GLOBAL); // crashes
					// void *handle = dlopen(arg_library, RTLD_LOCAL); // crashes
					//void *handle = dlopen(arg_library, RTLD_NOW); // crashes
					void *handle = dlopen(arg_library, RTLD_LAZY);

					printf_hide("dlopen(\"%s\") returned: %.8x\n", arg_library, handle);
					
					void (*func)() = dlsym(handle, arg_function);
					TCP_write(&conn, "function-name=%s -> address=%.8x\n", arg_function, func);
					
					func();
					
					dlclose(handle);
				}
				
				if (!strcmp(cmd, "stack"))
				{
					int16_t *stack = (int16_t*)0x08297500;
					int16_t *ptr;
					int i;
					for (i=1; i<=0xFFFD; i++)
					{
						ptr = &stack[8 * i];
						
						
								switch ((*((int *)ptr+2) & 0x1f))
								{
									case 0: /*printf_hide("#define STACK_UNDEFINED 0\n");*/ break;
									case 1: printf_hide("#define STACK_OBJECT 1\n"); break;
									//case 2: printf_hide("#define STACK_STRING 2 = \"%s\"\n", (char *)(*(int *)0x08206F00 + 8*(int)    (*(int16_t *)&stack[i])    + 4)); break;
									case 2: printf_hide("#define STACK_STRING 2 = \"%s\"\n", (char *)(*(int *)0x08206F00 + 8*    stack[i]    + 4)); break;
									case 3: printf_hide("#define STACK_LOCALIZED_STRING 3\n"); break;
									case 4: printf_hide("#define STACK_VECTOR 4 = (%.2f, %.2f, %.2f)\n", *(float *)((int)(stack[i]) + 0), *(float *)((int)(stack[i]) + 4), *(float *)((int)(stack[i]) + 8)); break;
									case 5: printf_hide("#define STACK_FLOAT 5\n"); break;
									case 6: printf_hide("#define STACK_INT 6 = %.8x=%d\n", *(int *)&stack[i], *(int *)&stack[i]); break;
									case 7: printf_hide("#define STACK_CODEPOS 7\n"); break;
									case 8: printf_hide("#define STACK_PRECODEPOS 8\n"); break;
									case 9: printf_hide("#define STACK_FUNCTION 9\n"); break;
									case 10: printf_hide("#define STACK_STACK 10\n"); break;
									case 11: printf_hide("#define STACK_ANIMATION 11\n"); break;
									case 12: printf_hide("#define STACK_DEVELOPER_CODEPOS 12\n"); break;
									case 13: printf_hide("#define STACK_INCLUDE_CODEPOS 13\n"); break;
									case 14: printf_hide("#define STACK_THREAD_LIST 14\n"); break;
									case 15: printf_hide("#define STACK_THREAD_1 15\n"); break;
									case 16: printf_hide("#define STACK_THREAD_2 16\n"); break;
									case 17: printf_hide("#define STACK_THREAD_3 17\n"); break;
									case 18: printf_hide("#define STACK_THREAD_4 18\n"); break;
									case 19: printf_hide("#define STACK_STRUCT 19\n"); break;
									case 20: printf_hide("#define STACK_REMOVED_ENTITY 20\n"); break;
									case 21: printf_hide("#define STACK_ENTITY 21\n"); break;
									case 22: printf_hide("#define STACK_ARRAY 22\n"); break;
									case 23: printf_hide("#define STACK_REMOVED_THREAD 23\n"); break;
									default: printf_hide("CRAP!\n");								
								}
						
						if ((*((int *)ptr+2) & 0x1f) == 10)
						{
							int v20 = *((int*)ptr+1);
							int v21 = *((int16_t*)v20+4);
							int v22 = *(int*)v20;
							int v23 = v20 + 11;
							printf_hide("ptr=%.8x if=%.8x v20=%.8x v21=%8ld v22=%.8x\n", ptr, *(int *)&stack[8*i+4] & 0x60, v20, v21, v22);
							
							while (v21)
							{
								--v21;
								int v26 = *(char *)v23++;
								int v27 = *(int *)v23;
								v23 += 4;
								

								
								if (v26 == 6)
								{
									//printf_hide("\tv27=%.8x\n", v27);
								}
							}
						}
						
						//printf_hide("#########next########\n"); // spam
					}
				}
			}

			//handleRequest(lol);
		}
		TCP_write(&conn, "\n}");

		TCP_close_connection(&conn);
	}
}

// MUST BE __cdecl, but i dont know how to tell it to gcc. but its default
int cdecl_injected_shell(int a, int b, int c) // args to get stack address
{
	char buffer[128];
	int i;
	printf_hide("called: int cdecl_injected_shell();\n");
	
	printf_hide("arg0 = %.8x\n", &a);
	printf_hide("arg1 = %.8x\n", &b);
	printf_hide("arg2 = %.8x\n", &c);
	
	while (1)
	{
		printf_hide("dbg> ");
		input(buffer, 128);
		
		if (!strcmp(buffer, "help"))
		{
			printf_hide("HELP!\n");
			printf_hide("---------------\n");
			printf_hide("Commands:\n");
			printf_hide(" quit\n");
		}
		else if (!strcmp(buffer, "quit"))
		{
			printf_hide("GOOD BYE!\n");
			break;
		}
		else if (!strcmp(buffer, "dump"))
		{
			unsigned int *address;
			
			do
			{
				printf_hide("dump address> ");
				input(buffer, 128);
				sscanf(buffer, "%x", &address);
				
				printf_hide("*0x%.8x = ", address);
				printf_hide("0x%.8x ", *address);
				printf_hide("float=%f ", *address);
				printf_hide("int=%d ", *address);
				printf_hide("uint=%u ", *address);
				printf_hide("123=%d ", 123);
				printf_hide("\n");
			} while (strlen(buffer) > 0);
		}
		else {
			printf_hide("ERROR: Unknown Command: \"%s\"\n", buffer);
		}
	}
	
	return;
}
int cdecl_return_0()
{
	return 0;
}
int cdecl_return_1()
{
	return 1;
}
int cdecl_print(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9)
{
	//abort();
	//exit(0);
	printf_hide("a0=%.8x\n", a0);
	printf_hide("a1=%.8x\n", a1);
	printf_hide("a2=%.8x\n", a2);
	printf_hide("a3=%.8x\n", a3);
	printf_hide("a4=%.8x\n", a4);
	printf_hide("a5=%.8x\n", a5);
	printf_hide("a6=%.8x\n", a6);
	printf_hide("a7=%.8x\n", a7);
	printf_hide("a8=%.8x\n", a8);
	printf_hide("a9=%.8x\n", a9);
	printf_hide("#######################################\n");
	printf_hide("#######################################\n");
	printf_hide("##############  HELLO  ################\n");
	printf_hide("#######################################\n");
	printf_hide("#######################################\n");
	//return stackReturnInt(1);
	return 1;
}




int TCP_server_start(TCP_connection *conn, int port)
{
	int ret;
	const int y = 1;

	conn->create_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (conn->create_socket == -1)
		return 0;

	setsockopt(conn->create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));

	conn->address.sin_family = AF_INET;
	conn->address.sin_addr.s_addr = INADDR_ANY;
	conn->address.sin_port = htons(port);

	ret = bind(conn->create_socket, (struct sockaddr *) &(conn->address), sizeof(conn->address));
	if (ret != 0)
		return 0;

	ret = listen(conn->create_socket, 5);
	if (ret == -1)
		return 0;

	conn->addrlen = sizeof(struct sockaddr_in);

	return 1;
}

int TCP_wait_for_client(TCP_connection *conn)
{
	conn->new_socket = accept(conn->create_socket, (struct sockaddr *) &(conn->address), &(conn->addrlen));
	if (conn->new_socket == -1)
		return 0;
	return 1;
}

int TCP_close_connection(TCP_connection *conn)
{
	int ret;
	ret = close(conn->new_socket);
	if (ret == -1)
		return 0;
	return 1;
}

// "testas asd" als query failed immer
// als BUFFER==16 war
// irgendwas is hier noch nich ganz richtig...
#define BUFFER 32
int TCP_read_all_bugged(TCP_connection *conn, void *buffer, int bufferLen, int *outMsgLen)
{
	int client_recv_offset = 0;
	char *localbuffer[BUFFER];
	int bufferLeft;
	int ret;

	do
	{
		ret = recv(conn->new_socket, localbuffer, BUFFER, 0);
		//printf_hide("]] recv: %s\n", localbuffer);
		if (ret == -1)
			return 0;

		bufferLeft = bufferLen - client_recv_offset;

		if (ret > bufferLeft)
			return 0;

		memcpy(buffer+client_recv_offset, localbuffer, ret);

		client_recv_offset += ret;

		//printf_hide("> [RECV] ret=%d bufferLeft=%d\n", ret, bufferLeft);
	}
	while (ret == BUFFER);

	*outMsgLen = client_recv_offset;

	return 1;
}

// just accepts ONE packet -.-
// other one dont want to work
int TCP_read_all(TCP_connection *conn, void *buffer, int bufferLen, int *outMsgLen)
{
	int client_recv_offset = 0;
	char *localbuffer[BUFFER];
	int bufferLeft;
	int ret;

	ret = recv(conn->new_socket, buffer, bufferLen, 0);
	if (ret == -1)
		return 0;

	*outMsgLen = ret;
	return 1;
}

int TCP_write(TCP_connection *conn, const char *str, ...)
{
	va_list args;
	char tmp[2048];
	va_start(args, str);
	vsnprintf(tmp, 2048, str, args);
	//vprintf(str, args); just for checking in console
	va_end(args);

	send(conn->new_socket, tmp, strlen(tmp), 0);
	return 1;
}



int parseRequest(char *toParse, char table[MAX_ROWS][MAX_COLS][MAX_CHARS])
{
	int len, i;
	char cc; // current char

	int atRow = 0; // the first Col of and Row is the command
	int atCol = 0; // the next cols after the command are the arguments
	int atPos = 0; // the index, what letter an cmd/arg is at

	int row, col;
	// clean the table from previous data
	for (row=0; row<MAX_ROWS; row++)
		for (col=0; col<MAX_ROWS; col++)
			table[row][col][0] = '\0';

	len = strlen(toParse);
	for (i=0; i<len; i++)
	{
		cc = toParse[i];

		// login $user $pass
		switch (cc)
		{
			case '\n':
			case '\r':
			case ';':
			{
				if (atCol == 0 && atPos == 0)
				{
					//printf_hide("> ignoring double newline/semicolon\n");
					break;
				}
				// todo: prevent row-overflow
				atRow++;
				atCol = 0;
				atPos = 0;

				break;
			}
			case ' ':
			case '\t':
				if (atPos == 0)
				{
					//printf_hide("> ignoring double space/tab\n");
					break;
				}
				//printf_hide("> new argument!\n");
				// todo: prevent col-overflow
				atCol++;
				atPos = 0;
				break;
			default:
			{
				if (atPos < MAX_CHARS-2) // 0 to 62 = text, 63 = endmarker
				{
					table[atRow][atCol][atPos] = cc;
					table[atRow][atCol][atPos+1] = '\0';
					//printf_hide("> new char: table[%d][%d][%d] = '%c'\n", atRow, atCol, atPos, table[atRow][atCol][atPos]);
					atPos++;
				}
			}
		}
	}
	return 1;
}

int handleRequest(char lol[MAX_ROWS][MAX_COLS][MAX_CHARS])
{
	int row, col;
	printf_hide("> handleRequest();\n");
	for (row=0; row<MAX_ROWS; row++)
	{
		if (lol[row][0][0] == '\0')
			break;
		printf_hide("> ");
		for (col=0; col<MAX_COLS; col++)
		{
			if (lol[row][col][0] == '\0')
				break;
			printf_hide("%s ", lol[row][col]);
		}
		printf_hide("\n");
	}
}

void input(char *buffer, int len)
{
	int i;
	i=0;
	do
	{
		buffer[i] = fgetc(stdin);
		if (buffer[i] == '\n')
		{
			buffer[i] = '\0';
			return;
		}
		if (buffer[i] == EOF)
		{
			usleep(1000); // mikro seconds (1000 = 1 ms), else 99 % cpu
			continue;
		}
		buffer[i+1] = '\0';
		i++;
	} while (1);
}







int readMemoryRange(void *from, void *buffer, int bytes)
{
	int ret;
	
	// SIGSEGV
	#if 0
		// INIT
		{
		sigset_t emptyset;
		// Install the SIGSEGV handler.
		if (sigsegv_install_handler (&handler) < 0)
			exit (2);
		// Save the current signal mask.
		sigemptyset (&emptyset);
		sigprocmask (SIG_BLOCK, &emptyset, &mainsigset);
		printf_hide("> [INFO] SIGSEGV got installed\n");
		}

		ret = setjmp(jumpbuffer); // save current state
	#endif

	ret = 0; // fake value for without-sig-management
	
	printf_hide(" ------>>>>>>>>>>>> setjmp(jumpbuffer)=%d\n", ret);

	// 0 = we arent got called by longjmp()
	// something else = longjmp(jumpbuffer, $somethingElse);
	if (ret == 0) {
		printf_hide("BEFORE memcpy()\n");
		memcpy(buffer, from, bytes);
		printf_hide("AFTER memcpy()\n");
		return bytes;
	}
	printf_hide("error reading 0x%.8x!\n", from);
	return -1;
}

#endif