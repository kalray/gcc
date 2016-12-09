/*
 *      Copyright 2010, Kalray.
 *      All rights reserved.
 *
 * Kalray, INCORPORATED PROPRIETARY INFORMATION
 * This software is supplied under the terms of a license agreement
 * or nondisclosure agreement with Kalray and may not be
 * copied or disclosed except in accordance with the terms of that
 * agreement.
 */

/* Note on dedicated registers: as these registers are not allocated by the
 * compiler, it is necessary to pass the index of such register. When it is
 * passed as builtin operand, an immediate of the form 'u[0..n]' is expected in
 * place of dedicated register. 'n' is the size of the register file minus one.
 * When dedicated register is the result, one additional operand of the same
 * form 'u[0..n]' is placed as first parameter to select the result register
 * index.
 *
 * Note on immediates: some builtins may accept constant value as parameter.
 * Programmer must respect the immediate range given by the bits number and
 * signess. Immediates are given under the form '[su][0-9]+'
 * - [su] for signed and unsigned respectively.
 * - [0-9]+ the bits number associated to the immediate.
 */

/* Register classes used:
 * GPR: 64 General Purpose Regisers
 * PR: 32 Paired General Purpose Registers
 */

/* Builtins for class k1 */
#ifdef __K1__

#ifndef _BUILTIN_K1_H_
#define _BUILTIN_K1_H_

#ifdef __cplusplus__
extern "C" {
#endif

#if defined(__native__)
#include "builtins_k1_native.h"

#else /* include for k1-compilation */

/* abdhp
 *  res_hp.high = | op0_hp.high - op1_hp.high |
 *  res_hp.low = | op0_hp.low - op1_hp.low |
 */
extern int __builtin_k1_abdhp (int op0_hp, int op1_hp);

/* addhp
 *  res_hp.high = op0_hp.high + op1_hp.high
 *  res_hp.low = op0_hp.low + op1_hp.low
 */
extern int __builtin_k1_addhp (int op0_hp, int op1_hp);

/* adds
 *  op0 + op1
 *  saturated
 */
extern int __builtin_k1_adds (int op0, int op1);

/* barrier
 *
 */
extern void __builtin_k1_barrier (void);

/* bwlu
 *  see The Kalray-1 VLIW Core Architecture and Reference Manual document on
 * bwlu instructions use
 */
extern unsigned __builtin_k1_bwlu (unsigned op0_a, unsigned op1_b,
				   unsigned op2_c, unsigned op3_d,
				   unsigned short op4_lut);

/* bwluhp
 *  see The Kalray-1 VLIW Core Architecture and Reference Manual document on
 * bwlu instructions use
 */
extern int __builtin_k1_bwluhp (int op0_hp_ab, int op1_hp_cd,
				unsigned int op2_hp_lut);

/* bwluwp
 *  see The Kalray-1 VLIW Core Architecture and Reference Manual document on
 * bwlu instructions use
 */
extern long long __builtin_k1_bwluwp (long long op0_wp_ab, long long op1_wp_cd,
				      unsigned int op2_hp_lut);

/* clear1
 *  event = event_reg * 32 + event_index
 *  see MPPA-256 Cluster and IO Subsystem Architecture document for event lines
 * description
 */
extern void __builtin_k1_clear1 (unsigned int event);

/* cbs
 *
 */
extern unsigned int __builtin_k1_cbs (unsigned int op0);

/* clz
 *
 */
extern int __builtin_k1_clz (unsigned int op0);

/* clzdl
 *
 */
extern int __builtin_k1_clzdl (unsigned long long op0);

/* cmove
 *  condition != 0 ? new_value : curent_value
 */
extern int __builtin_k1_cmove (int condition, int new_value, int curent_value);

/* ctz
 *
 */
extern int __builtin_k1_ctz (unsigned int op0);

/* ctzdl
 *
 */
extern int __builtin_k1_ctzdl (unsigned long long op0);

/* cws
 *
 */
extern long long __builtin_k1_cws (void *address, unsigned int new_value,
				   unsigned int old_value);

/* cws_s10
 *  <deprecated> remapped to cws
 */
static __inline__ __attribute__ ((deprecated)) long long
__builtin_k1_cws_s10 (short a, unsigned int b, long long c)
{
  return __builtin_k1_cws ((void *) (a + b), (c & 0xFFFFFFFF),
			   ((c >> 32) & 0xFFFFFFFF));
}

/* dflush
 *
 */
extern void __builtin_k1_dflush (void);

/* dflushl
 *
 */
extern void __builtin_k1_dflushl (void *addr);

/* dflushl_s10
 *  <deprecated> remapped to dflushl
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_dflushl_s10 (short a, int b)
{
  __builtin_k1_dflushl ((void *) (a + b));
}

/* dinval
 *
 */
extern void __builtin_k1_dinval (void);

/* dinvall
 *
 */
extern void __builtin_k1_dinvall (void *addr);

/* dinvall_s10
 *  <deprecated> remapped to dinvall
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_dinvall_s10 (short a, unsigned int b)
{
  __builtin_k1_dinvall ((void *) (a + b));
}

/* dpurge
 *
 */
extern void __builtin_k1_dpurge (void);

/* dpurgel
 *
 */
extern void __builtin_k1_dpurgel (void *addr);

/* dpurgel_s10
 *  <deprecated> remapped to dpurgel
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_dpurgel_s10 (short a, unsigned int b)
{
  __builtin_k1_dpurgel ((void *) (a + b));
}

/* dtouchl
 *
 */
extern void __builtin_k1_dtouchl (void *addr);

/* dtouchl_s10
 *  <deprecated> remapped to dtouchl
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_dtouchl_s10 (short a, unsigned int b)
{
  __builtin_k1_dtouchl ((void *) (a + b));
}

/* dzerol
 *
 */
extern void __builtin_k1_dzerol (void *addr);

/* dzerol_s10
 *  <deprecated> remapped to dzerol
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_dzerol_s10 (short a, unsigned int b)
{
  __builtin_k1_dzerol ((void *) (a + b));
}

/* extfz
 *
 */
extern unsigned int __builtin_k1_extfz (unsigned int op0,
					unsigned int op1_highbit,
					unsigned int op2_lowbit);

/* faddrn
 *  op0 + op1
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_faddrn (float op0, float op1);

/* fcdiv
 *
 */
extern float __builtin_k1_fcdiv (float dividend, float divisor);

/* fcma
 *  mult0_op0 * mult0_op1 + mult1_op0 * mult1_op1
 */
extern float __builtin_k1_fcma (float mult1_op0, float mult0_op0,
				float mult0_op1, float mult1_op1);

/* fcmawd
 *  mult0_op0 * mult0_op1 + mult1_op0 * mult1_op1
 */
extern double __builtin_k1_fcmawd (float mult1_op0, float mult0_op0,
				   float mult0_op1, float mult1_op1);

/* fcms
 *  mult0_op0 * mult0_op1 - mult1_op0 * mult1_op1
 */
extern float __builtin_k1_fcms (float mult1_op0, float mult0_op0,
				float mult0_op1, float mult1_op1);

/* fcmswd
 *  mult0_op0 * mult0_op1 - mult1_op0 * mult1_op1
 */
extern double __builtin_k1_fcmswd (float mult1_op0, float mult0_op0,
				   float mult0_op1, float mult1_op1);

/* fdma
 *  mult0_op0 * mult0_op1 + mult1_op0 * mult1_op1
 */
extern float __builtin_k1_fdma (float mult0_op0, float mult1_op0,
				float mult0_op1, float mult1_op1);

/* fdmawd
 *  mult0_op0 * mult0_op1 + mult1_op0 * mult1_op1
 */
extern double __builtin_k1_fdmawd (float mult0_op0, float mult1_op0,
				   float mult0_op1, float mult1_op1);

/* fdms
 *  mult0_op0 * mult0_op1 - mult1_op0 * mult1_op1
 */
extern float __builtin_k1_fdms (float mult0_op0, float mult1_op0,
				float mult0_op1, float mult1_op1);

/* fdmswd
 *  mult0_op0 * mult0_op1 - mult1_op0 * mult1_op1
 */
extern double __builtin_k1_fdmswd (float mult0_op0, float mult1_op0,
				   float mult0_op1, float mult1_op1);

/* fence
 *
 */
extern void __builtin_k1_fence (void);

/* ffma
 *  mult_op0 * mult_op1 + addend
 */
extern float __builtin_k1_ffma (float addend, float mult_op0, float mult_op1);

/* ffman
 *  - (mult_op0 * mult_op1 + addend)
 */
extern float __builtin_k1_ffman (float addend, float mult_op0, float mult_op1);

/* ffmanrn
 *  - (mult_op0 * mult_op1 + addend)
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_ffmanrn (float addend, float mult_op0,
				   float mult_op1);

/* ffmarn
 *  mult_op0 * mult_op1 + addend
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_ffmarn (float addend, float mult_op0, float mult_op1);

/* ffms
 *  addend - mult_op0 * mult_op1
 */
extern float __builtin_k1_ffms (float addend, float mult_op0, float mult_op1);

/* ffmsn
 *  mult_op0 * mult_op1 - addend
 */
extern float __builtin_k1_ffmsn (float addend, float mult_op0, float mult_op1);

/* ffmsnrn
 *  mult_op0 *  mult_op1 - addend
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_ffmsnrn (float addend, float mult_op0,
				   float mult_op1);

/* ffmsrn
 *  addend - mult_op0 * mult_op1
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_ffmsrn (float addend, float mult_op0, float mult_op1);

/* fixed
 *  float_to_fixed ( fp_32b_num * (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern int __builtin_k1_fixed (unsigned char rounding, float fp_32b_num,
			       unsigned char u6_coef);

/* fixedd
 *  float_to_fixed ( fp_64b_num * (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern long long __builtin_k1_fixedd (unsigned char rounding, double fp_64b_num,
				      unsigned char u6_coef);

/* fixedu
 *  float_to_unsigned_fixed ( fp_32b_num * (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern unsigned __builtin_k1_fixedu (unsigned char rounding, float fp_32b_num,
				     unsigned char u6_coef);

/* fixedu
 *  float_to_unsigned_fixed ( fp_64b_num * (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern unsigned long long __builtin_k1_fixedud (unsigned char rounding,
						double fp_64b_num,
						unsigned char u6_coef);

/* float
 *  fixed_to_float ( fixed_32b_num / (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern float __builtin_k1_float (unsigned char rounding, int fixed_32b_num,
				 unsigned char u6_coef);

/* floatd
 *  fixed_to_float ( fixed_64b_num / (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern double __builtin_k1_floatd (unsigned char rounding,
				   long long fixed_64b_num,
				   unsigned char u6_coef);

/* floatu
 *  fixed_to_float ( fixed_unsigned_32b_num / (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern float __builtin_k1_floatu (unsigned char rounding,
				  unsigned int fixed_unsigned_32b_num,
				  unsigned char u6_coef);

/* floatud
 *  fixed_to_float ( fixed_unsigned_64b_num / (1<<u6_coef))
 *  rounded to "rounding" mode
 */
extern double __builtin_k1_floatud (unsigned char rounding,
				    unsigned long long fixed_unsigned_64b_num,
				    unsigned char u6_coef);

/* fmulrn
 *  op0 * op1
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_fmulrn (float op0, float op1);

/* fmulnrn
 *  - op0 * op1
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_fmulnrn (float op0, float op1);

/* fsbfrn
 *  op1 - op0
 *  rounding to nearest, non excepting
 */
extern float __builtin_k1_fsbfrn (float op0, float op1);

/* fsdiv
 *
 */
extern float __builtin_k1_fsdiv (float, float);

/* fsinv
 *
 */
extern float __builtin_k1_fsinv (float);

/* fsinvn
 *
 */
extern float __builtin_k1_fsinvn (float);

/* fsisr
 *
 */
extern float __builtin_k1_fsisr (float);

/* get GPR = systemreg
 * GPR get( u[0..63]);
 */
extern unsigned int __builtin_k1_get (int);

/* get GPR = GPR
 * GPR get( GPR);
 */
extern unsigned int __builtin_k1_get_r (int);

/* get PRF = systemreg
 * PRF get( u[0..63]);
 */
extern unsigned long long __builtin_k1_getd (int);

/* get GPR = GPR
 * GPR get( GPR);
 */
extern unsigned long long __builtin_k1_getd_r (int);

/* hfxb onlyhfxreg, <u32>
 *
 */
extern void __builtin_k1_hfxb (unsigned char, int);

/* hfxt onlyhfxreg, <u32>
 *
 */
extern void __builtin_k1_hfxt (unsigned char, int);

#if defined(__open64__) || defined(__clang__)
#define __builtin_error(MSG)                                                   \
  do                                                                           \
    {                                                                          \
    }                                                                          \
  while (0);
#else
#define __builtin_error(MSG)                                                   \
  {                                                                            \
    extern int __attribute__ ((error (#MSG))) __builtin_compile_time_check (); \
    __builtin_compile_time_check ();                                           \
  }
#endif /* defined(__open64__) || defined(__clang__) */

#define __builtin_k1_hfx(arg1, arg2, arg3, arg4)                               \
  {                                                                            \
    switch (arg1)                                                              \
      {                                                                        \
      case 0:                                                                  \
	__builtin_k1_hfxb (arg2,                                               \
			   ((int) (arg3) << 16) | ((int) (arg4) &0xffff));     \
	break;                                                                 \
      case 1:                                                                  \
	__builtin_k1_hfxt (arg2,                                               \
			   ((int) (arg3) << 16) | ((int) (arg4) &0xffff));     \
	break;                                                                 \
      default:                                                                 \
	__builtin_error (                                                      \
	  "__builtin_k1_hfx expects a 1 bits immediate first argument.");      \
      }                                                                        \
  }

/* iinval
 *
 */
extern void __builtin_k1_iinval (void);

/* iinvall
 *
 */
extern void __builtin_k1_iinvall (void *addr);

/* iinvall_s10
 *  <deprecated> remapped to iinvall
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_iinvall_s10 (short a, unsigned int b)
{
  __builtin_k1_iinvall ((void *) (a + b));
}

/* indexjtlb
 *
 */
extern void __builtin_k1_indexjtlb (void);

/* indexltlb
 *
 */
extern void __builtin_k1_indexltlb (void);

/* invaldtlb
 *
 */
extern void __builtin_k1_invaldtlb (void);

/* invalitlb
 *
 */
extern void __builtin_k1_invalitlb (void);

/* itouchl
 *
 */
extern void __builtin_k1_itouchl (void *addr);

/* itouchl_s10
 *  <deprecated> remapped to itouchl
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_itouchl_s10 (short a, unsigned int b)
{
  __builtin_k1_itouchl ((void *) (a + b));
}

/* landhp
 *
 */
extern int __builtin_k1_landhp (int, int);

/* lbqz
 *
 */
extern long long __builtin_k1_lbqz (void *addr);

/* lbqz_s32_p
 *  <deprecated> remapped to lbqz
 */
static __inline__ __attribute__ ((deprecated)) long long
__builtin_k1_lbqz_s32_p (int a, int b)
{
  return __builtin_k1_lbqz ((void *) (a + b));
}

/* ldc
 *
 */
extern long long __builtin_k1_ldc (void *addr);

/* ldc_s32_p
 *  <deprecated> remapped to ldc
 */
static __inline__ __attribute__ ((deprecated)) long long
__builtin_k1_ldc_s32_p (int a, int b)
{
  return __builtin_k1_ldc ((void *) (a + b));
}

/* ldu
 *
 */
extern long long __builtin_k1_ldu (void *addr);

/* ldu_s10_p
 *  <deprecated> remapped to ldu
 */
static __inline__ __attribute__ ((deprecated)) long long
__builtin_k1_ldu_s10_p (int s10, unsigned int addr)
{
  return __builtin_k1_ldu ((void *) (addr + s10));
}

/* lwu
 *
 */
extern unsigned int __builtin_k1_lwu (void *addr);

/* maduuciwd
 *
 */
extern unsigned long long __builtin_k1_maduuciwd (unsigned long long,
						  unsigned int, unsigned int);

/* notify1
 *  event = event_reg * 32 + event_index
 *  see MPPA-256 Cluster and IO Subsystem Architecture document for event lines
 * description
 */
extern void __builtin_k1_notify1 (unsigned int);

/* probetlb
 *
 */
extern void __builtin_k1_probetlb (void);

/* raise1
 *  event = event_reg * 32 + event_index
 *  see MPPA-256 Cluster and IO Subsystem Architecture document for event lines
 * description
 */
extern void __builtin_k1_raise1 (unsigned int);

/* readtlb
 *
 */
extern void __builtin_k1_readtlb (void);

/* xord
 *
 */
extern void __builtin_k1_r_xord (unsigned char, unsigned long long,
				 unsigned short);

/* sat
 *
 */
extern int __builtin_k1_sat (int, unsigned char);

/* satd
 *
 */
extern long long int __builtin_k1_satd (long long int, unsigned char);

/* sbfhp
 *
 */
extern int __builtin_k1_sbfhp (int, int);

/* sbmm8
 *
 */
extern unsigned long long __builtin_k1_sbmm8_d (unsigned long long,
						unsigned long long);

/* sbmm8l
 *
 */
extern unsigned long long __builtin_k1_sbmm8_l (unsigned long long,
						unsigned long long);

/* sbmmt8
 *
 */
extern unsigned long long __builtin_k1_sbmmt8_d (unsigned long long,
						 unsigned long long);

/* set nopcpsreg = GPR
 *  set( u[0..63], GPR);
 */
extern void __builtin_k1_set (int, unsigned int);

/* set onlypsreg = GPR
 *  set( u[0..63], GPR);
 */
extern void __builtin_k1_set_ps (int, unsigned int);

/* sdu
 *
 */
extern void __builtin_k1_sdu (void *addr, unsigned long long value);

/* sdu_s10_p
 *  <deprecated> remapped to sdu
 */
static __inline__ __attribute__ ((deprecated)) void
__builtin_k1_sdu_s10_p (int s10, unsigned int addr, unsigned long long val)
{
  __builtin_k1_sdu ((void *) (addr + s10), val);
}

/* sllhps
 *
 */
extern int __builtin_k1_sllhps_r (int, int);

/* srahps
 *
 */
extern int __builtin_k1_srahps_r (int, int);

/* stsu
 *
 */
extern unsigned int __builtin_k1_stsu (unsigned int, unsigned int);

/* stsud
 *
 */
extern unsigned long long __builtin_k1_stsud (unsigned long long,
					      unsigned long long);

/* swu
 *
 */
extern void __builtin_k1_swu (void *addr, unsigned int data);

/* syncgroup
 *
 */
extern void __builtin_k1_syncgroup (unsigned int);

/* waitall
 *
 */
extern void __builtin_k1_waitall (unsigned int, unsigned char);

/* waitclr1
 *  event = event_reg * 32 + event_index
 *  see MPPA-256 Cluster and IO Subsystem Architecture document for event lines
 * description
 */
extern void __builtin_k1_waitclr1 (unsigned int);

/* waitany
 *
 */
extern unsigned int __builtin_k1_waitany (unsigned int, unsigned char);

/* wantany
 *
 */
extern unsigned int __builtin_k1_wantany (unsigned int, unsigned char);

/* wpurge
 *
 */
extern void __builtin_k1_wpurge (void);

/* writetlb
 *
 */
extern void __builtin_k1_writetlb (void);

#ifdef __cplusplus__
}
#endif

#endif

#endif /* _BUILTIN_K1_H_ */
#endif /* __K1__ */
