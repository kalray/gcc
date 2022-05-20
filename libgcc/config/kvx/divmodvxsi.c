/*
 * Copyright (C) 2021 Kalray SA.
 *
 * Routines for KVX division and modulus on 64-bit integers.
 *
 * Based on the "TMS320C5x User Guide".
 *
 *     #define STSU(b, r) ( ((r) >= (b)) ? (((r) - (b)) << 1 | 1) : ((r) << 1) )
 *
 *     divmod_result_t
 *     divmodu_3(uint32_t a, uint32_t b)
 *     {
 *         uint64_t acc = (uint64_t)a;
 *         uint64_t src = (uint64_t)b << (32 - 1);
 *         uint32_t q = 0, r = a;
 *         if (b == 0) TRAP;
 *         if (b > a)
 *             goto end;
 *         for (int i = 0; i < 32; i++) {
 *           acc = STSU(src, acc);
 *         }
 *         q = acc;
 *         r = acc >> 32;
 *     end:;
 *         return (divmod_result_t){ q, r };
 *     }
 *
 * For SIMD execution, we make the `if (b > a)` path pass through the loop by
 * setting `src = b << 32` so iterating 32 times `acc = STSU(src, acc)` computes
 * `acc == a << 32` then `q == 0` and `r == a`.
 *
 *     divmod_result_t
 *     divmodu_4(uint32_t a, uint32_t b)
 *     {
 *         uint64_t acc = (uint64_t)a;
 *         uint64_t src = (uint64_t)b << (32 - 1);
 *         if (b == 0) TRAP;
 *         if (b > a)
 *           src <<= 1;
 *         for (int i = 0; i < 32; i++) {
 *           acc = STSU(src, acc);
 *         }
 *         uint32_t q = acc;
 *         uint32_t r = acc >> 32;
 *     end:;
 *         return (divmod_result_t){ q, r };
 *     }
 *
 * -- Benoit Dupont de Dinechin (benoit.dinechin@kalray.eu)
 */

#include "divmodtypes.h"

static inline uint32x4_t
uint32x2_divmod (uint32x2_t a, uint32x2_t b)
{
  uint64x2_t acc = __builtin_kvx_widenwdp (a, ".z");
  uint64x2_t src = __builtin_kvx_widenwdp (b, ".z") << (32 - 1);
  uint64x2_t wb = __builtin_kvx_widenwdp (b, ".z");
  if (__builtin_kvx_anywp (b, ".eqz"))
    goto div0;
  // As `src == b << (32 -1)` adding src yields `src == b << 32`.
  src += src & (wb > acc);
  for (int i = 0; i < 32; i++)
    {
      acc = __builtin_kvx_stsudp (src, acc);
    }
  uint32x2_t q = __builtin_kvx_narrowdwp (acc, "");
  uint32x2_t r = __builtin_kvx_narrowdwp (acc >> 32, "");
end:;
  return __builtin_kvx_cat128 (q, r);
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return 0 - (uint32x4_t){};
#endif
  __builtin_trap ();
}

uint32x2_t
__udivv2si3 (uint32x2_t a, uint32x2_t b)
{
  uint32x4_t divmod = uint32x2_divmod (a, b);
  return __builtin_kvx_low64 (divmod);
}

uint32x2_t
__umodv2si3 (uint32x2_t a, uint32x2_t b)
{
  uint32x4_t divmod = uint32x2_divmod (a, b);
  return __builtin_kvx_high64 (divmod);
}

uint32x2_t
__udivmodv2si4 (uint32x2_t a, uint32x2_t b, uint32x2_t *c)
{
  uint32x4_t divmod = uint32x2_divmod (a, b);
  *c = __builtin_kvx_high64 (divmod);
  return __builtin_kvx_low64 (divmod);
}

int32x2_t
__divv2si3 (int32x2_t a, int32x2_t b)
{
  uint32x2_t absa = __builtin_kvx_abswp (a, "");
  uint32x2_t absb = __builtin_kvx_abswp (b, "");
  uint32x4_t divmod = uint32x2_divmod (absa, absb);
  int32x2_t result = __builtin_kvx_low64 (divmod);
  return __builtin_kvx_selectwp (-result, result, a ^ b, ".ltz");
}

int32x2_t
__modv2si3 (int32x2_t a, int32x2_t b)
{
  uint32x2_t absa = __builtin_kvx_abswp (a, "");
  uint32x2_t absb = __builtin_kvx_abswp (b, "");
  uint32x4_t divmod = uint32x2_divmod (absa, absb);
  int32x2_t result = __builtin_kvx_high64 (divmod);
  return __builtin_kvx_selectwp (-result, result, a, ".ltz");
}

#ifdef TEST_V2SI
#define LANE 1

uint32_t
__udivsi3 (uint32_t a, uint32_t b)
{
  uint32x2_t udivv2si3 = __udivv2si3 (a - (uint32x2_t){}, b - (uint32x2_t){});
  return (uint32_t)udivv2si3[LANE];
}

uint32_t
__umodsi3 (uint32_t a, uint32_t b)
{
  uint32x2_t umodv2si3 = __umodv2si3 (a - (uint32x2_t){}, b - (uint32x2_t){});
  return (uint32_t)umodv2si3[LANE];
}

uint32_t
__udivmodsi4 (uint32_t a, uint32_t b, uint32_t *c)
{
  uint32x2_t c_ = {0, 0};
  uint32x2_t udivmodv2si4 = __udivmodv2si4 (a - (uint32x2_t){}, b - (uint32x2_t){}, &c_);
  if (c)
    *c = c_[LANE];
  return (uint32_t)udivmodv2si4[LANE];
}

int32_t
__divsi3 (int32_t a, int32_t b)
{
  int32x2_t divv2si3 = __divv2si3 (a - (int32x2_t){}, b - (int32x2_t){});
  return (int32_t)divv2si3[LANE];
}

int32_t
__modsi3 (int32_t a, int32_t b)
{
  int32x2_t modv2si3 = __modv2si3 (a - (int32x2_t){}, b - (int32x2_t){});
  return (int32_t)modv2si3[LANE];
}

#endif // TEST_V2SI

static inline uint32x8_t
uint32x4_divmod (uint32x4_t a, uint32x4_t b)
{
  uint64x4_t acc = __builtin_kvx_widenwdq (a, ".z");
  uint64x4_t src = __builtin_kvx_widenwdq (b, ".z") << (32 - 1);
  uint64x4_t wb = __builtin_kvx_widenwdq (b, ".z");
  if (__builtin_kvx_anywq (b, ".eqz"))
    goto div0;
  // As `src == b << (32 -1)` adding src yields `src == b << 32`.
  src += src & (wb > acc);
  for (int i = 0; i < 32; i++)
    {
      acc = __builtin_kvx_stsudq (src, acc);
    }
  uint32x4_t q = __builtin_kvx_narrowdwq (acc, "");
  uint32x4_t r = __builtin_kvx_narrowdwq (acc >> 32, "");
end:;
  return __builtin_kvx_cat256 (q, r);
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return 0 - (uint32x8_t){};
#endif
  __builtin_trap ();
}

uint32x4_t
__udivv4si3 (uint32x4_t a, uint32x4_t b)
{
  uint32x8_t divmod = uint32x4_divmod (a, b);
  return __builtin_kvx_low128 (divmod);
}

uint32x4_t
__umodv4si3 (uint32x4_t a, uint32x4_t b)
{
  uint32x8_t divmod = uint32x4_divmod (a, b);
  return __builtin_kvx_high128 (divmod);
}

uint32x4_t
__udivmodv4si4 (uint32x4_t a, uint32x4_t b, uint32x4_t *c)
{
  uint32x8_t divmod = uint32x4_divmod (a, b);
  *c = __builtin_kvx_high128 (divmod);
  return __builtin_kvx_low128 (divmod);
}

int32x4_t
__divv4si3 (int32x4_t a, int32x4_t b)
{
  uint32x4_t absa = __builtin_kvx_abswq (a, "");
  uint32x4_t absb = __builtin_kvx_abswq (b, "");
  uint32x8_t divmod = uint32x4_divmod (absa, absb);
  int32x4_t result = __builtin_kvx_low128 (divmod);
  return __builtin_kvx_selectwq (-result, result, a ^ b, ".ltz");
}

int32x4_t
__modv4si3 (int32x4_t a, int32x4_t b)
{
  uint32x4_t absa = __builtin_kvx_abswq (a, "");
  uint32x4_t absb = __builtin_kvx_abswq (b, "");
  uint32x8_t divmod = uint32x4_divmod (absa, absb);
  int32x4_t result = __builtin_kvx_high128 (divmod);
  return __builtin_kvx_selectwq (-result, result, a, ".ltz");
}

#ifdef TEST_V4SI
#define LANE 2

uint32_t
__udivsi3 (uint32_t a, uint32_t b)
{
  uint32x4_t udivv4si3 = __udivv4si3 (a - (uint32x4_t){}, b - (uint32x4_t){});
  return (uint32_t)udivv4si3[LANE];
}

uint32_t
__umodsi3 (uint32_t a, uint32_t b)
{
  uint32x4_t umodv4si3 = __umodv4si3 (a - (uint32x4_t){}, b - (uint32x4_t){});
  return (uint32_t)umodv4si3[LANE];
}

uint32_t
__udivmodsi4 (uint32_t a, uint32_t b, uint32_t *c)
{
  uint32x4_t c_ = {0, 0};
  uint32x4_t udivmodv4si4 = __udivmodv4si4 (a - (uint32x4_t){}, b - (uint32x4_t){}, &c_);
  if (c)
    *c = c_[LANE];
  return (uint32_t)udivmodv4si4[LANE];
}

int32_t
__divsi3 (int32_t a, int32_t b)
{
  int32x4_t divv4si3 = __divv4si3 (a - (int32x4_t){}, b - (int32x4_t){});
  return (int32_t)divv4si3[LANE];
}

int32_t
__modsi3 (int32_t a, int32_t b)
{
  int32x4_t modv4si3 = __modv4si3 (a - (int32x4_t){}, b - (int32x4_t){});
  return (int32_t)modv4si3[LANE];
}

#endif // TEST_V4SI

static inline uint32x16_t
uint32x8_divmod (uint32x8_t a, uint32x8_t b)
{
  uint64x8_t acc = __builtin_kvx_widenwdo (a, ".z");
  uint64x8_t src = __builtin_kvx_widenwdo (b, ".z") << (32 - 1);
  uint64x8_t wb = __builtin_kvx_widenwdo (b, ".z");
  if (__builtin_kvx_anywo (b, ".eqz"))
    goto div0;
  // As `src == b << (32 -1)` adding src yields `src == b << 32`.
  src += src & (wb > acc);
  for (int i = 0; i < 32; i++)
    {
      acc = __builtin_kvx_stsudo (src, acc);
    }
  uint32x8_t q = __builtin_kvx_narrowdwo (acc, "");
  uint32x8_t r = __builtin_kvx_narrowdwo (acc >> 32, "");
end:;
  return __builtin_kvx_cat512 (q, r);
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return 0 - (uint32x16_t){};
#endif
  __builtin_trap ();
}

uint32x8_t
__udivv8si3 (uint32x8_t a, uint32x8_t b)
{
  uint32x16_t divmod = uint32x8_divmod (a, b);
  return __builtin_kvx_low256 (divmod);
}

uint32x8_t
__umodv8si3 (uint32x8_t a, uint32x8_t b)
{
  uint32x16_t divmod = uint32x8_divmod (a, b);
  return __builtin_kvx_high256 (divmod);
}

uint32x8_t
__udivmodv8si4 (uint32x8_t a, uint32x8_t b, uint32x8_t *c)
{
  uint32x16_t divmod = uint32x8_divmod (a, b);
  *c = __builtin_kvx_high256 (divmod);
  return __builtin_kvx_low256 (divmod);
}

int32x8_t
__divv8si3 (int32x8_t a, int32x8_t b)
{
  uint32x8_t absa = __builtin_kvx_abswo (a, "");
  uint32x8_t absb = __builtin_kvx_abswo (b, "");
  uint32x16_t divmod = uint32x8_divmod (absa, absb);
  int32x8_t result = __builtin_kvx_low256 (divmod);
  return __builtin_kvx_selectwo (-result, result, a ^ b, ".ltz");
}

int32x8_t
__modv8si3 (int32x8_t a, int32x8_t b)
{
  uint32x8_t absa = __builtin_kvx_abswo (a, "");
  uint32x8_t absb = __builtin_kvx_abswo (b, "");
  uint32x16_t divmod = uint32x8_divmod (absa, absb);
  int32x8_t result = __builtin_kvx_high256 (divmod);
  return __builtin_kvx_selectwo (-result, result, a, ".ltz");
}

