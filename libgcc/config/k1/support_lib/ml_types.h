/*******************************************************************************
* This file is part of Kalray's Metalibm tool
* Copyright (2013)
* All rights reserved
* created:          Dec 23rd, 2013
* last-modified:    Dec 23rd, 2013
*
* author(s): Nicolas Brunie (nicolas.brunie@kalray.eu)
*******************************************************************************/

#ifndef __ML_TYPES_H__
#define __ML_TYPES_H__

#define ml_int32_t int
#define ml_uint32_t unsigned int
#define ml_int64_t long long
#define ml_uint64_t unsigned long long


/** conversion union for bitfield/binary32 cast */
typedef union {
    ml_uint32_t u;
    ml_int32_t i;
    float f;
} uif_conv_t;

/** conversion union for bitfield/binary64 cast */
typedef union {
    ml_uint64_t u;
    ml_int64_t i;
    double d;
} uid_conv_t;

typedef struct {
    double hi;
    double lo;
} ml_dd_t;

typedef struct {
    double hi;
    double me;
    double lo;
} ml_td_t;



#endif /** __ML_TYPES_H__ */
