/*******************************************************************************
* This file is part of Kalray's Metalibm tool
* Copyright (2013)
* All rights reserved
* created:          Dec 23rd, 2013
* last-modified:    Dec 23rd, 2013
*
* author(s): Nicolas Brunie (nicolas.brunie@kalray.eu)
*******************************************************************************/
#include "ml_types.h"

#ifndef __ML_SPECIAL_VALUES_H__
#define __ML_SPECIAL_VALUES_H__

#define ATTRIBUTE_UNUSED __attribute__((unused))

/** binary32 special values */
static uif_conv_t fp32_sv_PlusInfty  ATTRIBUTE_UNUSED = {.u = 0x7f800000};
static uif_conv_t fp32_sv_MinusInfty ATTRIBUTE_UNUSED = {.u = 0xff800000};
static uif_conv_t fp32_sv_PlusOmega  ATTRIBUTE_UNUSED = {.u = 0x7f7fffff};
static uif_conv_t fp32_sv_MinusOmega ATTRIBUTE_UNUSED = {.u = 0xff7fffff};
static uif_conv_t fp32_sv_PlusZero   ATTRIBUTE_UNUSED = {.u = 0x00000000};
static uif_conv_t fp32_sv_MinusZero  ATTRIBUTE_UNUSED = {.u = 0x80000000};
static uif_conv_t fp32_sv_QNaN       ATTRIBUTE_UNUSED = {.u = 0xffffffff};
static uif_conv_t fp32_sv_SNaN       ATTRIBUTE_UNUSED = {.u = 0xffbfffff};


/** binary64 special values */
static uid_conv_t fp64_sv_PlusInfty  ATTRIBUTE_UNUSED = {.u = 0x7ff0000000000000ull};
static uid_conv_t fp64_sv_MinusInfty ATTRIBUTE_UNUSED = {.u = 0xfff0000000000000ull};
static uid_conv_t fp64_sv_PlusOmega  ATTRIBUTE_UNUSED = {.u = 0x7fefffffffffffffull};
static uid_conv_t fp64_sv_MinusOmega ATTRIBUTE_UNUSED = {.u = 0xffefffffffffffffull};
static uid_conv_t fp64_sv_PlusZero   ATTRIBUTE_UNUSED = {.u = 0x0000000000000000ull};
static uid_conv_t fp64_sv_MinusZero  ATTRIBUTE_UNUSED = {.u = 0x8000000000000000ull};
static uid_conv_t fp64_sv_QNaN       ATTRIBUTE_UNUSED = {.u = 0xffffffffffffffffull};
static uid_conv_t fp64_sv_SNaN       ATTRIBUTE_UNUSED = {.u = 0xfff7ffffffffffffull};


#endif /** __ML_SPECIAL_VALUES_H__ */
