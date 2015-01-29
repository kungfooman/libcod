#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

/*
	DID MOST OF THIS TO HARDEN THE LIB FOR DECOMPILING
	countercheck:
		strings -a libcod2_ver_1_3.so | less
		strings libcod2_ver_1_3.so | less
		nm libcod2_ver_1_3.so | less
		nm -D libcod2_ver_1_3.so | less
*/

// that makes debugging easier after some months 
#define DEBUG_MYSQL 0
#define DEBUG_GSC 0
#define DEBUG_MEMORY 0
#define DEBUG_CAR 1
#define DEBUG_TCC 1

// GSC MODULES
#define COMPILE_MYSQL 1
#define COMPILE_MYSQL_TESTS 1
#define COMPILE_PLAYER 1
#define COMPILE_ASTAR 1
#define COMPILE_MEMORY 1
#define COMPILE_CAR 0
#define COMPILE_TCC 0
#define COMPILE_UTILS 1

// LIB FEATURES
#define COMPILE_DEBUG_SERVER 0

#define DEBUG_PRINTF 1

// GLOBAL MACROS
#define STR(s) #s // stringify used for __FUNCTION__
#define INT(address) (*(int*)(address))

#endif
