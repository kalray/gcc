/*
 * Copyright (C) 2021 Kalray SA.
 *
 * Routines for KVX division and modulus on 16-bit integers.
 *
 * Based on the "TMS320C5x User Guide".
 *
 *     #define STSU(b, r) ( ((r) >= (b)) ? (((r) - (b)) << 1 | 1) : ((r) << 1) )
 *
 *     divmod_result_t
 *     divmodu_3(uint16_t a, uint16_t b)
 *     {
 *         uint32_t acc = (uint32_t)a;
 *         uint32_t src = (uint32_t)b << (16 - 1);
 *         uint16_t q = 0, r = a;
 *         if (b == 0) TRAP;
 *         if (b > a)
 *             goto end;
 *         for (int i = 0; i < 16; i++) {
 *           acc = STSU(src, acc);
 *         }
 *         q = acc;
 *         r = acc >> 16;
 *     end:;
 *         return (divmod_result_t){ q, r };
 *     }
 *
 * For SIMD execution, we make the `if (b > a)` path pass through the loop by
 * setting `src = b << 16` so iterating 16 times `acc = STSU(src, acc)` computes
 * `acc == a << 16` then `q == 0` and `r == a`.
 *
 *     divmod_result_t
 *     divmodu_4(uint16_t a, uint16_t b)
 *     {
 *         uint32_t acc = (uint32_t)a;
 *         uint32_t src = (uint32_t)b << (16 - 1);
 *         if (b == 0) TRAP;
 *         if (b > a)
 *           src <<= 1;
 *         for (int i = 0; i < 16; i++) {
 *           acc = STSU(src, acc);
 *         }
 *         uint16_t q = acc;
 *         uint16_t r = acc >> 16;
 *     end:;
 *         return (divmod_result_t){ q, r };
 *     }
 *
 * -- Benoit Dupont de Dinechin (benoit.dinechin@kalray.eu)
 */

#include "divmodtypes.h"

static inline uint16x8_t
uint16x4_divmod (uint16x4_t a, uint16x4_t b)
{
  uint32x4_t acc = __builtin_kvx_widenhwq (a, ".z");
  uint32x4_t src = __builtin_kvx_widenhwq (b, ".z") << (16 - 1);
  uint32x4_t wb = __builtin_kvx_widenhwq (b, ".z");
  if (__builtin_kvx_anyhq (b, ".eqz"))
    goto div0;
  // As `src == b << (16 -1)` adding src yields `src == b << 16`.
  src += src & (wb > acc);
#if 0 //__kv3_2__ CR
  for (int i = 0; i < 16; i++)
    {
      acc = __builtin_kvx_stsuwq (src, acc);
    }
#else
  uint64x4_t w_acc = __builtin_kvx_widenwdq (acc, ".z");
  uint64x4_t w_src = __builtin_kvx_widenwdq (src, ".z");
  for (int i = 0; i < 16; i++)
    {
      w_acc = __builtin_kvx_stsudq (w_src, w_acc);
    }
  acc = __builtin_kvx_narrowdwq (w_acc, "");
#endif
  uint16x4_t q = __builtin_kvx_narrowwhq (acc, "");
  uint16x4_t r = __builtin_kvx_narrowwhq (acc >> 16, "");
end:;
  return __builtin_kvx_cat128 (q, r);
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return 0 - (uint16x8_t){};
#endif
  __builtin_trap ();
}

uint16x4_t
__udivv4hi3 (uint16x4_t a, uint16x4_t b)
{
  uint16x8_t divmod = uint16x4_divmod (a, b);
  return __builtin_kvx_low64 (divmod);
}

uint16x4_t
__umodv4hi3 (uint16x4_t a, uint16x4_t b)
{
  uint16x8_t divmod = uint16x4_divmod (a, b);
  return __builtin_kvx_high64 (divmod);
}

uint16x4_t
__udivmodv4hi4 (uint16x4_t a, uint16x4_t b, uint16x4_t *c)
{
  uint16x8_t divmod = uint16x4_divmod (a, b);
  *c = __builtin_kvx_high64 (divmod);
  return __builtin_kvx_low64 (divmod);
}

int16x4_t
__divv4hi3 (int16x4_t a, int16x4_t b)
{
  uint16x4_t absa = __builtin_kvx_abshq (a, "");
  uint16x4_t absb = __builtin_kvx_abshq (b, "");
  uint16x8_t divmod = uint16x4_divmod (absa, absb);
  int16x4_t result = __builtin_kvx_low64 (divmod);
  return __builtin_kvx_selecthq (-result, result, a ^ b, ".ltz");
}

int16x4_t
__modv4hi3 (int16x4_t a, int16x4_t b)
{
  uint16x4_t absa = __builtin_kvx_abshq (a, "");
  uint16x4_t absb = __builtin_kvx_abshq (b, "");
  uint16x8_t divmod = uint16x4_divmod (absa, absb);
  int16x4_t result = __builtin_kvx_high64 (divmod);
  return __builtin_kvx_selecthq (-result, result, a, ".ltz");
}

static inline uint16x16_t
uint16x8_divmod (uint16x8_t a, uint16x8_t b)
{
  uint32x8_t acc = __builtin_kvx_widenhwo (a, ".z");
  uint32x8_t src = __builtin_kvx_widenhwo (b, ".z") << (16 - 1);
  uint32x8_t wb = __builtin_kvx_widenhwo (b, ".z");
  if (__builtin_kvx_anyho (b, ".eqz"))
    goto div0;
  // As `src == b << (16 -1)` adding src yields `src == b << 16`.
  src += src & (wb > acc);
#if 0 //__kv3_2__ CR
  for (int i = 0; i < 16; i++)
    {
      acc = __builtin_kvx_stsuwo (src, acc);
    }
#else
  uint32x4_t acc_lo = __builtin_kvx_low128 (acc);
  uint32x4_t src_lo = __builtin_kvx_low128 (src);
  uint64x4_t w_acc_lo = __builtin_kvx_widenwdq (acc_lo, ".z");
  uint64x4_t w_src_lo = __builtin_kvx_widenwdq (src_lo, ".z");
  uint32x4_t acc_hi = __builtin_kvx_high128 (acc);
  uint32x4_t src_hi = __builtin_kvx_high128 (src);
  uint64x4_t w_acc_hi = __builtin_kvx_widenwdq (acc_hi, ".z");
  uint64x4_t w_src_hi = __builtin_kvx_widenwdq (src_hi, ".z");
  for (int i = 0; i < 16; i++)
    {
      w_acc_lo = __builtin_kvx_stsudq (w_src_lo, w_acc_lo);
      w_acc_hi = __builtin_kvx_stsudq (w_src_hi, w_acc_hi);
    }
  acc_lo = __builtin_kvx_narrowdwq (w_acc_lo, "");
  acc_hi = __builtin_kvx_narrowdwq (w_acc_hi, "");
  acc = __builtin_kvx_cat256 (acc_lo, acc_hi);
#endif
  uint16x8_t q = __builtin_kvx_narrowwho (acc, "");
  uint16x8_t r = __builtin_kvx_narrowwho (acc >> 16, "");
end:;
  return __builtin_kvx_cat256 (q, r);
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return 0 - (uint16x16_t){};
#endif
  __builtin_trap ();
}

uint16x8_t
__udivv8hi3 (uint16x8_t a, uint16x8_t b)
{
  uint16x16_t divmod = uint16x8_divmod (a, b);
  return __builtin_kvx_low128 (divmod);
}

uint16x8_t
__umodv8hi3 (uint16x8_t a, uint16x8_t b)
{
  uint16x16_t divmod = uint16x8_divmod (a, b);
  return __builtin_kvx_high128 (divmod);
}

uint16x8_t
__udivmodv8hi4 (uint16x8_t a, uint16x8_t b, uint16x8_t *c)
{
  uint16x16_t divmod = uint16x8_divmod (a, b);
  *c = __builtin_kvx_high128 (divmod);
  return __builtin_kvx_low128 (divmod);
}

int16x8_t
__divv8hi3 (int16x8_t a, int16x8_t b)
{
  uint16x8_t absa = __builtin_kvx_absho (a, "");
  uint16x8_t absb = __builtin_kvx_absho (b, "");
  uint16x16_t divmod = uint16x8_divmod (absa, absb);
  int16x8_t result = __builtin_kvx_low128 (divmod);
  return __builtin_kvx_selectho (-result, result, a ^ b, ".ltz");
}

int16x8_t
__modv8hi3 (int16x8_t a, int16x8_t b)
{
  uint16x8_t absa = __builtin_kvx_absho (a, "");
  uint16x8_t absb = __builtin_kvx_absho (b, "");
  uint16x16_t divmod = uint16x8_divmod (absa, absb);
  int16x8_t result = __builtin_kvx_high128 (divmod);
  return __builtin_kvx_selectho (-result, result, a, ".ltz");
}

static inline uint16x32_t
uint16x16_divmod (uint16x16_t a, uint16x16_t b)
{
  uint32x16_t acc = __builtin_kvx_widenhwx (a, ".z");
  uint32x16_t src = __builtin_kvx_widenhwx (b, ".z") << (16 - 1);
  uint32x16_t wb = __builtin_kvx_widenhwx (b, ".z");
  if (__builtin_kvx_anyhx (b, ".eqz"))
    goto div0;
  // As `src == b << (16 -1)` adding src yields `src == b << 16`.
  src += src & (wb > acc);
#if 0 //__kv3_2__ CR
  for (int i = 0; i < 16; i++)
    {
      acc = __builtin_kvx_stsuwx (src, acc);
    }
#else
  uint32x8_t acc_lo = __builtin_kvx_low256 (acc);
  uint32x8_t src_lo = __builtin_kvx_low256 (src);
  uint64x8_t w_acc_lo = __builtin_kvx_widenwdo (acc_lo, ".z");
  uint64x8_t w_src_lo = __builtin_kvx_widenwdo (src_lo, ".z");
  uint32x8_t acc_hi = __builtin_kvx_high256 (acc);
  uint32x8_t src_hi = __builtin_kvx_high256 (src);
  uint64x8_t w_acc_hi = __builtin_kvx_widenwdo (acc_hi, ".z");
  uint64x8_t w_src_hi = __builtin_kvx_widenwdo (src_hi, ".z");
  for (int i = 0; i < 16; i++)
    {
      w_acc_lo = __builtin_kvx_stsudo (w_src_lo, w_acc_lo);
      w_acc_hi = __builtin_kvx_stsudo (w_src_hi, w_acc_hi);
    }
  acc_lo = __builtin_kvx_narrowdwo (w_acc_lo, "");
  acc_hi = __builtin_kvx_narrowdwo (w_acc_hi, "");
  acc = __builtin_kvx_cat512 (acc_lo, acc_hi);
#endif
  uint16x16_t q = __builtin_kvx_narrowwhx (acc, "");
  uint16x16_t r = __builtin_kvx_narrowwhx (acc >> 16, "");
end:;
  return __builtin_kvx_cat512 (q, r);
div0:
#ifndef __linux__
  if (&_KVX_NO_DIVMOD0_TRAP)
    return 0 - (uint16x32_t){};
#endif
  __builtin_trap ();
}

uint16x16_t
__udivv16hi3 (uint16x16_t a, uint16x16_t b)
{
  uint16x32_t divmod = uint16x16_divmod (a, b);
  return __builtin_kvx_low256 (divmod);
}

uint16x16_t
__umodv16hi3 (uint16x16_t a, uint16x16_t b)
{
  uint16x32_t divmod = uint16x16_divmod (a, b);
  return __builtin_kvx_high256 (divmod);
}

uint16x16_t
__udivmodv16hi4 (uint16x16_t a, uint16x16_t b, uint16x16_t *c)
{
  uint16x32_t divmod = uint16x16_divmod (a, b);
  *c = __builtin_kvx_high256 (divmod);
  return __builtin_kvx_low256 (divmod);
}

int16x16_t
__divv16hi3 (int16x16_t a, int16x16_t b)
{
  uint16x16_t absa = __builtin_kvx_abshx (a, "");
  uint16x16_t absb = __builtin_kvx_abshx (b, "");
  uint16x32_t divmod = uint16x16_divmod (absa, absb);
  int16x16_t result = __builtin_kvx_low256 (divmod);
  return __builtin_kvx_selecthx (-result, result, a ^ b, ".ltz");
}

int16x16_t
__modv16hi3 (int16x16_t a, int16x16_t b)
{
  uint16x16_t absa = __builtin_kvx_abshx (a, "");
  uint16x16_t absb = __builtin_kvx_abshx (b, "");
  uint16x32_t divmod = uint16x16_divmod (absa, absb);
  int16x16_t result = __builtin_kvx_high256 (divmod);
  return __builtin_kvx_selecthx (-result, result, a, ".ltz");
}

