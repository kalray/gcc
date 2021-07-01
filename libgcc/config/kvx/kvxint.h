/*
 * Copyright (C) 2021 Kalray SA.
 *
 * All rights reserved.
 */
#ifndef RTLIB_KVX_KVXINT_H
#define RTLIB_KVX_KVXINT_H

#include <sys/types.h>

#define clz(a)                                                                 \
  _Generic((a), int                                                            \
	   : __builtin_clz, unsigned int                                       \
	   : __builtin_clz, long                                               \
	   : __builtin_clzl, unsigned long                                     \
	   : __builtin_clzl, long long                                         \
	   : __builtin_clzll, unsigned long long                               \
	   : __builtin_clzll) (a)

#define min(a, b) ((a) < (b) ? (a) : (b))

#define abs(a) ((a) < 0 ? -(a) : (a))

#define stsu(a, b)                                                             \
  _Generic((a) - (b), uint32_t                                                 \
	   : __builtin_kvx_stsuw, uint64_t                                     \
	   : __builtin_kvx_stsud) (a, b)

/*
 * Shifts 'lhs' right by the number of bits given in 'rhs'.
 * If any nonzero bits are shifted off, they are "jammed"
 * into the least-significant bit of the shifted value by
 * setting the least-significant bit to 1. This shifted-and-jammed
 * value is returned. The value of 'rhs' is saturated to ensure branchless lhs
 * bits jamming.
 */
#define jsr(lhs, rhs)                                                          \
  ((lhs) >> min (sizeof (lhs) * 8 - 1, rhs)                                    \
   | !!((lhs) << (-min (sizeof (lhs) * 8 - 1, rhs) & (sizeof (lhs) * 8 - 1))))

typedef _Bool bool;

/* RTLIB_KVX_KVXINT_H */
#endif
