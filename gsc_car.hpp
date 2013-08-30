#ifndef _GSC_CAR_HPP_
#define _GSC_CAR_HPP_

#ifdef __cplusplus
extern "C" {
#endif

/* default stuff */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* gsc functions */
#include "gsc.hpp"

#if COMPILE_CAR == 1
int gsc_car_new();
int gsc_car_update();
#endif

#ifdef __cplusplus
}
#endif

#endif