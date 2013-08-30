#ifndef _GSC_SERVER_HPP_
#define _GSC_SERVER_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* va_args */
#include <stdarg.h>

/* cdecl_injected_closer() */
#include "gsc.hpp"


#include <dlfcn.h>

#include <pthread.h>

// for getpid()
#include <sys/types.h>
#include <unistd.h> // also for usleep()

// for time()
#include <time.h>


#include <inttypes.h> // int16_t


// NET
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h> /* mprotect */

#include "cracking.hpp"

typedef struct
{
	int create_socket;
	int new_socket;
	struct sockaddr_in address;
	socklen_t addrlen;
} TCP_connection;


#define BUFFER_OUT 4096

void startServerAsThread(int port, int debug);

static void mythread(void *arg);

int cdecl_injected_shell(int a, int b, int c);
int cdecl_return_0();
int cdecl_return_1();
int cdecl_print(int a0, int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);



int TCP_server_start(TCP_connection *conn, int port);
int TCP_wait_for_client(TCP_connection *conn);
int TCP_close_connection(TCP_connection *conn);
int TCP_read_all_bugged(TCP_connection *conn, void *buffer, int bufferLen, int *outMsgLen);
int TCP_read_all(TCP_connection *conn, void *buffer, int bufferLen, int *outMsgLen);
int TCP_write(TCP_connection *conn, const char *str, ...);

#define MAX_ROWS 16
#define MAX_COLS 16
#define MAX_CHARS 64
int parseRequest(char *toParse, char table[MAX_ROWS][MAX_COLS][MAX_CHARS]);
int handleRequest(char lol[MAX_ROWS][MAX_COLS][MAX_CHARS]);

void input(char *buffer, int len);

// now in cracking.hpp
//int singleHexToNumber(char hexchar);
//int hexToBuffer(char *hex, char *buffer, int bufferLen);

int readMemoryRange(void *from, void *buffer, int bytes);

#ifdef __cplusplus
}
#endif

#endif



