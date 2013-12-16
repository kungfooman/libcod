#ifndef _GSC_UTILS_HPP_
#define _GSC_UTILS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

/* link unlink */
#include <unistd.h>

int gsc_utils_disableGlobalPlayerCollision();
int gsc_utils_ClientCommand();
int gsc_utils_getAscii();
int gsc_utils_system();
int gsc_utils_file_link();
int gsc_utils_file_unlink();
int gsc_utils_FS_LoadDir();
int gsc_utils_fileexists();

#ifdef __cplusplus
}
#endif

#endif