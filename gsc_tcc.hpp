#if COMPILE_TCC == 0
	#define _GSC_TCC_HPP_
#endif


#ifndef _GSC_TCC_HPP_
#define _GSC_TCC_HPP_

#ifdef __cplusplus
extern "C" {
#endif

	/* default stuff */
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	/* gsc functions */
	#include "gsc.hpp"

	/* tcc functions */
	#include "tcc/tcc.h"
	#include "tcc/libtcc.h"

	int gsc_tcc_new();
	int gsc_tcc_add_include_path();
	int gsc_tcc_add_file();
	int gsc_tcc_run();
	int gsc_tcc_delete();

#ifdef __cplusplus
}
#endif

#endif