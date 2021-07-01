/*
 * Copyright (C) 2021 Kalray SA.
 *
 * All rights reserved.
 */
#ifndef RTLIB_KVX_KVXFP_H
#define RTLIB_KVX_KVXFP_H

#include "kvxint.h"

/**************
 * FPU System *
 **************/

#define KVXFP_EXCEPT_INVALID 0x02
#define KVXFP_EXCEPT_DIVBYZERO 0x04
#define KVXFP_EXCEPT_OVERFLOW 0x08
#define KVXFP_EXCEPT_UNDERFLOW 0x10
#define KVXFP_EXCEPT_INEXACT 0x20

#define KVXFP_ROUNDING_TONEAREST (0 << 16)
#define KVXFP_ROUNDING_UPWARD (1 << 16)
#define KVXFP_ROUNDING_DOWNWARD (2 << 16)
#define KVXFP_ROUNDING_TOWARDZERO (3 << 16)

#define KVXFP_ROUNDING_MASK KVXFP_ROUNDING_TOWARDZERO

/* The following macro specifies whether the FPU detects
 * tininess before rounding, this is used in both 32-bits and 64-bits
 * floating point divisions during final round packing */
#define KVXFP_DETECT_TININESS_BEFORE_ROUNDING 0

static inline void
kvxfp_raiseexcept (uint64_t excepts)
{
  /* The wfxl insn allows to set the least significant word of the
   * operated system register (here $cs), with 'excepts << 32' as a
   * set/clear mask.
   */
  __builtin_kvx_wfxl (4, excepts << 32);
}

static inline int
kvxfp_getround (void)
{
  /* Get $cs flags (rounding mode only) */
  return __builtin_kvx_get (4) & KVXFP_ROUNDING_MASK;
}

/*******************
 * Misc. functions *
 *******************/

#define KVXFP_FMA(...) __builtin_kvx_ffmad (__VA_ARGS__)

/*
 * Returns an approximation to the reciprocal of the number represented by 'a',
 * where 'a' is interpreted as an unsigned fixed-point number with one integer
 * bit and 31 fraction bits.  The 'a' input must be "normalized", meaning that
 * its most-significant bit (bit 31) must be 1.  Thus, if A is the value of
 * the fixed-point interpretation of 'a', then 1 <= A < 2.  The returned value
 * is interpreted as a pure unsigned fraction, having no integer bits and 32
 * fraction bits.  The approximation returned should never be greater than
 * the true reciprocal 1/A, and it differs from the true reciprocal by
 * at most 2.006 ulp (units in the last place).
 */
static uint32_t
kvxfp_approximate_reciprocal32 (uint32_t a)
{
  const double D = __builtin_kvx_floatud (a, 0, ".s");
  union
  {
    uint64_t i;
    double f;
  } Xi = {.f = __builtin_kvx_fsrecd (D, ".s")};

  /* Xi's fsrecd estimate has a 9 bits precision
   * Newton-Raphson roughly doubles precision at each iteration,
   * effectively reaching 36>32 bits precision after iteration 2 */
  Xi.f = KVXFP_FMA (KVXFP_FMA (Xi.f, -D, 1, ".s"), Xi.f, Xi.f, ".s");
  Xi.f = KVXFP_FMA (KVXFP_FMA (Xi.f, -D, 1, ".s"), Xi.f, Xi.f, ".s");

  return (Xi.i & 0x000FFFFFFFFFFFFF) >> 21 | 0x80000000;
}

/* RTLIB_KVX_KVXFP_H */
#endif
