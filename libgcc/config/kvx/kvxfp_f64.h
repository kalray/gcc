/*
 * Copyright (C) 2021 Kalray SA.
 *
 * All rights reserved.
 */
#ifndef RTLIB_KVX_KVXFP_F64_H
#define RTLIB_KVX_KVXFP_F64_H

#include "kvxfp.h"

/******************************
 * KVX 64 bits floating point *
 ******************************/

union kvxfp_f64
{
  struct
  {
    uint64_t significand : 52;
    uint64_t exponent : 11;
    uint64_t sign : 1;
  };
  uint64_t ui;
  double f;
};

_Static_assert(sizeof (union kvxfp_f64) == sizeof (double)
		 && sizeof (uint64_t) == sizeof (double),
	       "Invalid union kvxfp_f64 size");

static inline bool
kvxfp_f64_isnormal (union kvxfp_f64 f64)
{
  return f64.exponent - 1U < 0x7FE;
}

static inline bool
kvxfp_f64_issnan (union kvxfp_f64 f64)
{
  return (f64.ui & UINT64_C (0x7FF8000000000000))
	   == UINT64_C (0x7FF0000000000000)
	 && (f64.ui & UINT64_C (0x0007FFFFFFFFFFFF));
}

static inline int
kvxfp_f64_normalize_subnormal (union kvxfp_f64 *f64)
{
  const uint64_t significand = f64->significand;
  const unsigned shift = clz (significand) - 11;

  f64->significand = significand << shift;

  return 1 - shift;
}

/* RTLIB_KVX_KVXFP_H */
#endif
