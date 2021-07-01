/*
 * Copyright (C) 2021 Kalray SA.
 *
 * All rights reserved.
 */
#ifndef RTLIB_KVX_KVXFP_F32_H
#define RTLIB_KVX_KVXFP_F32_H

#include "kvxfp.h"

/******************************
 * KVX 32 bits floating point *
 ******************************/

union kvxfp_f32
{
  struct
  {
    uint32_t significand : 23;
    uint32_t exponent : 8;
    uint32_t sign : 1;
  };
  uint32_t ui;
  float f;
};

_Static_assert(sizeof (union kvxfp_f32) == sizeof (float)
		 && sizeof (uint32_t) == sizeof (float),
	       "Invalid union kvxfp_f32 size");

static inline bool
kvxfp_f32_isnormal (union kvxfp_f32 f32)
{
  return f32.exponent - 1U < 0xFE;
}

static inline bool
kvxfp_f32_issnan (union kvxfp_f32 f32)
{
  return (f32.ui & UINT32_C (0x7FC00000)) == UINT32_C (0x7F800000)
	 && (f32.ui & UINT32_C (0x003FFFFF));
}

static inline int
kvxfp_f32_normalize_subnormal (union kvxfp_f32 *f32)
{
  const uint32_t significand = f32->significand;
  const unsigned shift = clz (significand) - 8;

  f32->significand = significand << shift;

  return 1 - shift;
}

/* RTLIB_KVX_KVXFP_H */
#endif
