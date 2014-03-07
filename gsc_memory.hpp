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
void gsc_memory_malloc();
void gsc_memory_free();
void gsc_memory_int_get();
void gsc_memory_int_set();
void gsc_memory_memset();
#endif

#ifdef __cplusplus
}
#endif

#endif
