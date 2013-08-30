#ifndef _GSC_MEMORY_HPP_
#define _GSC_MEMORY_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

#if COMPILE_MEMORY == 1
int gsc_memory_malloc();
int gsc_memory_free();
int gsc_memory_int_get();
int gsc_memory_int_set();
int gsc_memory_memset();
#endif

#ifdef __cplusplus
}
#endif

#endif