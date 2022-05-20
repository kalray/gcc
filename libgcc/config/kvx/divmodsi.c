/*
 * Copyright (C) 2021 Kalray SA.
 *
 * Routines for KVX division and modulus on 32-bit integers.
 *
 * Based on the "TMS320C6000 Integer Division" Application Report, October 2000.
 *
 *     #define STSU(b, r) ( ((r) >= (b)) ? (((r) - (b)) << 1 | 1) : ((r) << 1) )
 *
 *     divmod_result_t
 *     divmodu32(uint32_t a, uint32_t b)
 *     {
 *         uint64_t r = a;
 *         if (b == 0) TRAP;
 *         if (b > a)
 *             return (divmod_result_t){ 0, a };
 *         int k = CLZ(b) - CLZ(a);
 *         b <<= k++;
 *         for (int i = 0; i < k; i++) {
 *             r = STSU(b, r);
 *         }
 *         uint32_t q = r & ((1LL<<k) - 1);
 *         r >>= k;
 *     end:;
 *         return (divmod_result_t){ q, r };
 *     }
 *
 * -- Benoit Dupont de Dinechin (benoit.dinechin@kalray.eu)
 */

#include "divmodtypes.h"

#if 0
static inline uint32x2_t
uint32_divmod (uint32_t a, uint32_t b)
{
  uint32_t q = 0;
  uint64_t r = a;
  int k = __builtin_clz (b) - __builtin_clz (a);
  if (b == 0)
    goto div0;
  if (b > a)
    goto end;
  b <<= k++;
  uint64_t s = (1LL << k) - 1;
  for (int i = 0; i < k; i++)
    {
      r = __builtin_kvx_stsud (b, r);
    }
  q = r & s;
  r >>= k;
end:;
  return (uint32x2_t){q, r};
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return (uint32x2_t){0, 0};
#endif
  __builtin_trap ();
}
#else
static inline uint32x2_t
uint32_divmod (uint32_t a, uint32_t b)
{
  double double1 = 1.0;
  float floatb = __builtin_kvx_floatuw (b, 0,  ".rn.s");
  float floatrec = __builtin_kvx_frecw (floatb, ".rn.s");
  if (b == 0) goto div0;
  double doublea = __builtin_kvx_floatud (a, 0, ".rn.s");
  double doubleb = __builtin_kvx_floatud (b, 0, ".rn.s");
  double doublerec = __builtin_kvx_fwidenwd (floatrec, ".s");
  double alpha = __builtin_kvx_ffmsd (doublerec, doubleb, double1, ".rn.s");
  double beta = __builtin_kvx_ffmad (alpha, doublerec, doublerec, ".rn.s");
  double gamma = __builtin_kvx_fmuld (doublea, beta, ".rn.s");
  uint32_t quo = __builtin_kvx_fixedud (gamma, 0, ".rn.s");
  int32_t rem = a - quo*b;
  uint32_t cond = rem >> 31;
  uint32_t q = quo + cond;
  uint32_t r = rem + (b & cond);
  return (uint32x2_t){q, r};
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return (uint32x2_t){0, 0};
#endif
  __builtin_trap ();
}
#endif

#if !defined(TEST_V2SI) && !defined(TEST_V4SI)

uint32_t
__udivsi3 (uint32_t a, uint32_t b)
{
  uint32x2_t divmod = uint32_divmod (a, b);
  return (uint32_t)divmod[0];
}

uint32_t
__umodsi3 (uint32_t a, uint32_t b)
{
  uint32x2_t divmod = uint32_divmod (a, b);
  return (uint32_t)divmod[1];
}

uint32_t
__udivmodsi4 (uint32_t a, uint32_t b, uint32_t *c)
{
  uint32x2_t divmod = uint32_divmod (a, b);
  *c = (uint32_t)divmod[1];
  return (uint32_t)divmod[0];
}

int32_t
__divsi3 (int32_t a, int32_t b)
{
  uint32_t absa = __builtin_kvx_absw (a, "");
  uint32_t absb = __builtin_kvx_absw (b, "");
  uint32x2_t divmod = uint32_divmod (absa, absb);
  int32_t result = (int32_t)divmod[0];
  if ((a ^ b) < 0)
    result = -result;
  return result;
}

int32_t
__modsi3 (int32_t a, int32_t b)
{
  uint32_t absa = __builtin_kvx_absw (a, "");
  uint32_t absb = __builtin_kvx_absw (b, "");
  uint32x2_t divmod = uint32_divmod (absa, absb);
  int32_t result = (int32_t)divmod[1];
  if (a < 0)
    result = -result;
  return result;
}

#endif // TEST_V2SI, TEST_V4SI

