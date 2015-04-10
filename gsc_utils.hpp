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
void gsc_utils_sendgameservercommand();
void gsc_utils_scandir();
void gsc_G_FindConfigstringIndex();
void gsc_G_FindConfigstringIndexOriginal();

void gsc_add_language();
void gsc_load_languages();
void gsc_get_language_item();
void gsc_themetext();

void gsc_utils_fopen();
void gsc_utils_fread();
void gsc_utils_fwrite();
void gsc_utils_fclose();
void gsc_utils_sprintf();

void gsc_call_function_raw();

void gsc_dlopen();
void gsc_dlsym();
void gsc_dlclose();

void gsc_utils_free();
void gsc_utils_setdefaultweapon();
void gsc_utils_getweaponmaxammo();
void gsc_utils_getweapondamage();
void gsc_utils_setweapondamage();
void gsc_utils_getweaponmeleedamage();
void gsc_utils_setweaponmeleedamage();
void gsc_utils_getweaponfiretime();
void gsc_utils_setweaponfiretime();
void gsc_utils_getweaponmeleetime();
void gsc_utils_setweaponmeleetime();
void gsc_utils_getweaponreloadtime();
void gsc_utils_setweaponreloadtime();
void gsc_utils_getweaponreloademptytime();
void gsc_utils_setweaponreloademptytime();
void gsc_utils_getweaponhitlocmultiplier();
void gsc_utils_setweaponhitlocmultiplier();
void gsc_utils_getloadedweapons();

#ifdef __cplusplus
}
#endif

#endif
