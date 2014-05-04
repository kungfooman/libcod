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

void gsc_utils_disableGlobalPlayerCollision();
void gsc_utils_getAscii();
void gsc_utils_system();
void gsc_utils_file_link();
void gsc_utils_file_unlink();
void gsc_utils_file_exists();
void gsc_utils_FS_LoadDir();
void gsc_utils_getType();
void gsc_utils_stringToFloat();
void gsc_utils_rundll();
void gsc_utils_ExecuteString();
void gsc_utils_scandir();

void gsc_add_language();
void gsc_load_languages();
void gsc_get_language_item();

void gsc_utils_fopen();
void gsc_utils_fread();
void gsc_utils_fwrite();
void gsc_utils_fclose();
void gsc_utils_sprintf();

#ifdef __cplusplus
}
#endif

#endif
