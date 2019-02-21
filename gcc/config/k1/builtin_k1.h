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

/*
 * BCU Instructions
 */

/* AWAIT */
extern void __builtin_k1_await (void);
/* BARRIER */
extern void __builtin_k1_barrier (void);
/* BREAK */
/* CALL */
/* CB */
/* CDB */
/* DOZE */
extern void __builtin_k1_doze (void);
/* GET */
extern unsigned long long __builtin_k1_get (int);
/* GOTO */
/* WFXL */
extern void __builtin_k1_wfxl (unsigned char, unsigned long long);

/* WFXM */
extern void __builtin_k1_wfxm (unsigned char, unsigned long long);
/* ICALL */
/* IGET */
/* IGOTO */
/* IGOTOD */
/* LOOPDO */
/* LOOPGTZ */
/* LOOPNEZ */
/* RET */
/* RFE */
/* SCALL */
/* SET */
/* SLEEP */
extern void __builtin_k1_sleep (void);
/* STOP */
extern void __builtin_k1_stop (void);
/* SYNCGROUP */
extern void __builtin_k1_syncgroup (unsigned long long);
/* TRAPA */
/* TRAPO */
/* TLBDINVAL */
extern void __builtin_k1_tlbdinval (void);
/* TLBIINVAL */
extern void __builtin_k1_tlbiinval (void);
/* TLBPROBE */
extern void __builtin_k1_tlbprobe (void);
/* TLBREAD */
extern void __builtin_k1_tlbread (void);
/* TLBWRITE */
extern void __builtin_k1_tlbwrite (void);
/* WAITIT */
extern unsigned int __builtin_k1_waitit (unsigned int);

/*
 * LSU Instrutions
 */

/* ACSWAPW */
extern unsigned __int128 __builtin_k1_acswapw (void *address,
					       unsigned long long new_value,
					       unsigned long long old_value);
/* ACSWAPD */
extern unsigned __int128 __builtin_k1_acswapd (void *address,
					       unsigned long long new_value,
					       unsigned long long old_value);
/* AFADDD */
extern unsigned long long __builtin_k1_afaddd (void *address, long long addend);
/* AFADDW */
extern unsigned int __builtin_k1_afaddw (void *address, int addend);
/* ALCLRD */
extern unsigned long long __builtin_k1_alclrd (void *addr);
/* ALCLRW */
extern unsigned int __builtin_k1_alclrw (void *addr);
/* DINVAL */
extern void __builtin_k1_dinval (void);
/* DINVALL */
extern void __builtin_k1_dinvall (const void *addr);
/* DTOUCHL */
extern void __builtin_k1_dtouchl (const void *addr);
/* DZEROL */
extern void __builtin_k1_dzerol (void *addr);
/* DZEROLU */
/* FENCE */
extern void __builtin_k1_fence (void);
/* IINVAL */
extern void __builtin_k1_iinval (void);
/* IINVALS */
extern void __builtin_k1_iinvals (const void *addr);
/* LBS_U */
extern char __builtin_k1_lbsu (const void *addr);
/* LBZ_U */
extern unsigned char __builtin_k1_lbzu (const void *addr);
/* LD */
/* LD_N */
/* LD_U */
extern unsigned long long __builtin_k1_ldu (const void *addr);
/* LD_UN */
/* LHS_N */
/* LHS_U */
extern short __builtin_k1_lhsu (const void *addr);
/* LHS_UN */
/* LHZ */
/* LHZ_N */
/* LHZ_U */
extern unsigned short __builtin_k1_lhzu (const void *addr);
/* LHZ_UN */
/* LWZ */
/* LWZ_N */
/* LWZ_U */
extern unsigned int __builtin_k1_lwzu (const void *addr);
/* LWZ_UN */
/* SB */
/* SD */
/* SH */
/* SW */

/*
 * ALU Instruction (non-FPU)
 */

/* ABDW */
extern int __builtin_k1_abdw (int op0, int op1);
/* ABDD */
extern long long __builtin_k1_abdd (long long op0, long long op1);
/* ABDHP */
/* ABDHQ */
/* ABDWP */
/* ABSW */
/* ABSD */
/* ABSHP */
/* ABSHQ */
/* ABSWP */
/* ADDW */
/* ADDC */
/* ADDCD */
/* ADDCI */
/* ADDCID */
/* ADDD */
/* ADDHP */
/* ADDHQ */
/* ADDHR */
/* ADDHRWP */
/* ADDSW */
extern int __builtin_k1_addsw (int op0, int op1);
/* ADDSD */
extern long long __builtin_k1_addsd (long long op0, long long op1);
/* ADDSHP */
/* ADDSHQ */
/* ADDSWP */
/* ADDUHR */
/* ADDUHRWP */
/* ADDUWD */
/* ADDWD */
/* ADDWP */
/* ADDX2 */
/* ADDX2D */
/* ADDX2HP */
/* ADDX2HQ */
/* ADDX2UWD */
/* ADDX2WD */
/* ADDX2WP */
/* ADDX4 */
/* ADDX4D */
/* ADDX4HP */
/* ADDX4HQ */
/* ADDX4UWD */
/* ADDX4WD */
/* ADDX4WP */
/* ADDX8 */
/* ADDX8D */
/* ADDX8HP */
/* ADDX8HQ */
/* ADDX8UWD */
/* ADDX8WD */
/* ADDX8WP */
/* AND */
/* ANDD */
/* ANDHR */
/* ANDHRWP */
/* ANDN */
/* ANDND */
/* AVGW */
extern int __builtin_k1_avgw (int op0, int op1);
/* AVGUW */
extern unsigned int __builtin_k1_avguw (unsigned int op0, unsigned int op1);
/* AVGHP */
/* AVGHQ */
/* AVGRW */
extern int __builtin_k1_avgrw (int op0, int op1);
/* AVGRUW */
extern unsigned int __builtin_k1_avgruw (unsigned int op0, unsigned int op1);
/* AVGRHP */
/* AVGRHQ */
/* AVGRU */
/* AVGRUD */
/* AVGRUHP */
/* AVGRUHQ */
/* AVGRUWP */
/* AVGRWP */
/* AVGU */
/* AVGUD */
/* AVGUHP */
/* AVGUHQ */
/* AVGUWP */
/* AVGWP */
/* CBSW */
extern int __builtin_k1_cbsw (unsigned int op0);
/* CBSD */
extern long long __builtin_k1_cbsd (unsigned long long op0);
/* CBSWP */
/* CDMOVED */
/* CLS */
/* CLSDL */
/* CLSWP */
/* CLZW */
extern int __builtin_k1_clzw (unsigned int op0);
/* CLZD */
extern long long __builtin_k1_clzd (unsigned long long op0);
/* CLZWP */
/* CMOVE */
extern int __builtin_k1_cmove (int condition, int new_value, int curent_value);
/* CMOVED */
/* CMOVEHP */
/* CMOVEHQ */
/* CMOVEWP */
/* COMP */
/* COMPDL */
/* COMPHP */
/* COMPHQ */
/* COMPUWDL */
/* COMPWDL */
/* COMPWP */
/* CTZW */
extern int __builtin_k1_ctzw (unsigned int op0);
/* CTZD */
extern long long __builtin_k1_ctzd (unsigned long long op0);
/* CTZWP */
/* EXTFDS */
/* EXTFDZ */
/* EXTFZ */
extern unsigned int __builtin_k1_extfz (unsigned int op0,
					unsigned int op1_highbit,
					unsigned int op2_lowbit);
/* EXTFS */
/* EXTFZ */
/* INSF */
/* LAND */
/* LANDDL */
/* LANDHP */
extern int __builtin_k1_landhp (int, int);
/* LANDHQ */
/* LANDWP */
/* LNAND */
/* LNANDDL */
/* LNANDHP */
/* LNANDHQ */
/* LNANDWP */
/* LNOR */
/* LNORDL */
/* LNORHP */
/* LNORHQ */
/* LNORWP */
/* LOR */
/* LORDL */
/* LORHP */
/* LORHQ */
/* LORWP */
/* MAKE */
/* MAKED */
/* MAX */
/* MAXD */
/* MAXHP */
/* MAXHPS */
/* MAXHQ */
/* MAXHR */
/* MAXHRWP */
/* MAXS */
/* MAXU */
/* MAXUD */
/* MAXUHP */
/* MAXUHPS */
/* MAXUHQ */
/* MAXUHR */
/* MAXUHRWP */
/* MAXUS */
/* MAXUWP */
/* MAXWP */
/* MIN */
/* MIND */
/* MINHP */
/* MINHPS */
/* MINHQ */
/* MINHR */
/* MINHRWP */
/* MINS */
/* MINU */
/* MINUD */
/* MINUHP */
/* MINUHPS */
/* MINUHQ */
/* MINUHR */
/* MINUHRWP */
/* MINUS */
/* MINUWP */
/* MINWP */
/* NAND */
/* NANDD */
/* NEG */
/* NEGD */
/* NEGHP */
/* NEGHQ */
/* NEGWP */
/* NOR */
/* NORD */
/* NXOR */
/* NXORD */
/* OR */
/* ORD */
/* ORN */
/* ORND */
/* ROL */
/* ROLWPS */
/* ROR */
/* RORWPS */
/* SATD */
extern long long int __builtin_k1_satd (long long int, unsigned char);
/* SATUD */
extern unsigned long long int __builtin_k1_satud (long long int, unsigned char);
/* SBF */
/* SBFC */
/* SBFCD */
/* SBFCI */
/* SBFCID */
/* SBFD */
/* SBFHP */
extern int __builtin_k1_sbfhp (int, int);
/* SBFHQ */
/* SBFSW */
extern int __builtin_k1_sbfsw (int op0, int op1);
/* SBFSD */
extern long long __builtin_k1_sbfsd (long long op0, long long op1);
/* SBFSHP */
/* SBFSHQ */
/* SBFSWP */
/* SBFUWD */
/* SBFWD */
/* SBFWP */
/* SBFX2 */
/* SBFX2D */
/* SBFX2HP */
/* SBFX2HQ */
/* SBFX2UWD */
/* SBFX2WD */
/* SBFX2WP */
/* SBFX4 */
/* SBFX4D */
/* SBFX4HP */
/* SBFX4HQ */
/* SBFX4UWD */
/* SBFX4WD */
/* SBFX4WP */
/* SBFX8 */
/* SBFX8D */
/* SBFX8HP */
/* SBFX8HQ */
/* SBFX8UWD */
/* SBFX8WD */
/* SBFX8WP */
/* SBMM8 */
extern unsigned long long __builtin_k1_sbmm8 (unsigned long long a,
					      unsigned long long b);
/* SBMMT8 */
extern unsigned long long __builtin_k1_sbmmt8 (unsigned long long a,
					       unsigned long long b);
/* SLL */
/* SLLD */
/* SLLDM */
/* SLLHPS */
extern unsigned int __builtin_k1_sllhps (unsigned int, unsigned int);
/* SLLHQS */
/* SLLWPS */
/* SLS */
/* SLSD */
/* SLSHPS */
/* SLSHQS */
/* SLSWPS */
/* SRA */
/* SRAD */
/* SRADL */
/* SRAHPS */
extern unsigned int __builtin_k1_srahps (unsigned int, unsigned int);
/* SRAHQS */
/* SRAWPS */
/* SRL */
/* SRLD */
/* SRLDL */
/* SRLHPS */
/* SRLHQS */
/* SRLWPS */
/* SRS */
/* SRSD */
/* SRSHPS */
/* SRSHQS */
/* SRSWPS */
/* STSUW */
extern unsigned int __builtin_k1_stsuw (unsigned int, unsigned int);
/* STSUD */
extern unsigned long long __builtin_k1_stsud (unsigned long long,
					      unsigned long long);
/* SXBP */
/* SXBPWP */
/* XOR */
/* XORD */
/* XORHR */
/* XORHRWP */
/* ZXBP */
/* ZXBPWP */

/*
 * MAU Instructions (non-FPU)
 */

/* CMAHD */
/* CMAQHD */
/* CMASUHD */
/* CMAUUHD */
/* CMSHD */
/* CMSQHD */
/* CMSSUHD */
/* CMSUUHD */
/* DMAHD */
/* DMAQHD */
/* DMASUHD */
/* DMAUUHD */
/* DMSHD */
/* DMSQHD */
/* DMSSUHD */
/* DMSUUHD */
/* MADHWP */
/* MADQBWPRD */
/* MADQTWPRD */
/* MADQWPRD */
/* MADSUCIWD */
/* MADSUCWD */
/* MADSUHWP */
/* MADSUWD */
/* MADUUCIWD */
extern unsigned long long __builtin_k1_maduuciwd (unsigned long long,
						  unsigned int, unsigned int);
/* MADUUCWD */
/* MADUUHWP */
/* MADUUWD */
/* MADWD */
/* MSBHWP */
/* MSBQBWPRD */
/* MSBQTWPRD */
/* MSBQWPRD */
/* MSBSUHWP */
/* MSBSUWD */
/* MSBUUHWP */
/* MSBUUWD */
/* MSBWD */
/* MULBWDL */
/* MULHPSWP */
/* MULHWP */
/* MULQBBS */
/* MULQBTS */
/* MULQBWPRD */
/* MULQBWPRS */
/* MULQHPSPRSWP */
/* MULQHSWP */
/* MULQPRS */
/* MULQTBS */
/* MULQTTS */
/* MULQTWPRD */
/* MULQTWPRS */
/* MULQWPRD */
/* MULSUBWDL */
/* MULSUHPSWP */
/* MULSUHWP */
/* MULSUTWDL */
/* MULSUWD */
/* MULSUWDM */
/* MULTWDL */
/* MULUUBWDL */
/* MULUUHPSWP */
/* MULUUHWP */
/* MULUUTWDL */
/* MULUUWD */
/* MULUUWDM */
/* MULWD */
/* MULWDL */
/* MULWDM */

/*
 * FPU Instructions (ALU and MAU)
 */

/* FINVW */
extern float __builtin_k1_finvw (float op0, unsigned int silent,
				 unsigned int rounding);
/* FISRW */
extern float __builtin_k1_fisrw (float op0, unsigned int silent,
				 unsigned int rounding);
/* FABSW */
/* FABSD */
/* FABSWP */
/* FADDW */
/* FADDD */
/* FADDRN */
extern float __builtin_k1_faddrn (float op0, float op1);
/* FADDRND */
extern double __builtin_k1_faddrnd (double op0, double op1);
/* FADDRNWP */
/* FADDWP */
/* FCDIVW */
extern float __builtin_k1_fcdivw (float dividend, float divisor);
/* FCDIVD */
extern double __builtin_k1_fcdivd (double dividend, double divisor);
/* FCDIVWP */
/* FCOMP */
/* FCOMPDL */
/* FCOMPWP */
/* FFMAW */
extern float __builtin_k1_ffmaw (float addend, float mult_op0, float mult_op1);
/* FFMAD */
extern double __builtin_k1_ffmad (double addend, double mult_op0,
				  double mult_op1);
/* FFMADRN */
extern double __builtin_k1_ffmadrn (double addend, double mult_op0,
				    double mult_op1);
/* FFMARND */
/* FFMARNWD */
/* FFMARNWP */
/* FFMAWD */
extern double __builtin_k1_ffmawd (double addend, float mult_op0,
				   float mult_op1);
/* FFMAWDRN */
extern double __builtin_k1_ffmawdrn (double addend, float mult_op0,
				     float mult_op1);
/* FFMAWP */
/* FFMAWRN */
extern float __builtin_k1_ffmawrn (float addend, float mult_op0,
				   float mult_op1);
/* FFMSW */
extern float __builtin_k1_ffmsw (float addend, float mult_op0, float mult_op1);
/* FFMSD */
extern double __builtin_k1_ffmsd (double addend, double mult_op0,
				  double mult_op1);
/* FFMSDRN */
extern double __builtin_k1_ffmsdrn (double addend, double mult_op0,
				    double mult_op1);
/* FFMSRN */
extern float __builtin_k1_ffmsrn (float addend, float mult_op0, float mult_op1);
/* FFMSRND */
/* FFMSRNWD */
/* FFMSRNWP */
/* FFMSWRN */
extern float __builtin_k1_ffmswrn (float addend, float mult_op0,
				   float mult_op1);
/* FFMSWD */
extern double __builtin_k1_ffmswd (double addend, float mult_op0,
				   float mult_op1);
/* FFMSWDRN */
extern double __builtin_k1_ffmswdrn (double addend, float mult_op0,
				     float mult_op1);
/* FFMSWP */
/* FIXED */
extern int __builtin_k1_fixed (unsigned char rounding, float fp_32b_num,
			       unsigned char u6_coef);
/* FIXEDD */
extern long long __builtin_k1_fixedd (unsigned char rounding, double fp_64b_num,
				      unsigned char u6_coef);
/* FIXEDDN */
/* FIXEDN */
/* FIXEDU */
extern unsigned int __builtin_k1_fixedu (unsigned char rounding,
					 float fp_32b_num,
					 unsigned char u6_coef);
/* FIXEDUD */
extern unsigned long long __builtin_k1_fixedud (unsigned char rounding,
						double fp_64b_num,
						unsigned char u6_coef);
/* FIXEDUDN */
/* FIXEDUN */
/* FIXEDUWP */
/* FIXEDUWPN */
/* FIXEDWP */
/* FIXEDWPN */
/* FLOAT */
extern float __builtin_k1_float (unsigned char rounding, int fixed_32b_num,
				 unsigned char u6_coef);
/* FLOATD */
extern double __builtin_k1_floatd (unsigned char rounding,
				   long long fixed_64b_num,
				   unsigned char u6_coef);
/* FLOATDN */
/* FLOATN */
/* FLOATU */
extern float __builtin_k1_floatu (unsigned char rounding,
				  unsigned int fixed_unsigned_32b_num,
				  unsigned char u6_coef);
/* FLOATUD */
extern double __builtin_k1_floatud (unsigned char rounding,
				    unsigned long long fixed_unsigned_64b_num,
				    unsigned char u6_coef);
/* FLOATUDN */
/* FLOATUN */
/* FLOATUWP */
/* FLOATUWPN */
/* FLOATWP */
/* FLOATWPN */
/* FMAX */
/* FMAXD */
/* FMAXS */
/* FMAXWP */
/* FMIN */
/* FMIND */
/* FMINS */
/* FMINWP */
/* FMUL */
/* FMULD */
/* FMULRN */
extern float __builtin_k1_fmulrn (float op0, float op1);
/* FMULRND */
extern double __builtin_k1_fmulrnd (double op0, double op1);
/* FMULRNWD */
extern double __builtin_k1_fmulrnwd (float op0, float op1);
/* FMULRNWP */
/* FMULWD */
extern double __builtin_k1_fmulwd (float op0, float op1);
/* FMULWND */
extern double __builtin_k1_fmulwnd (double op0, double op1);
/* FMULWP */
/* FNARROWD */
/* FNARROWW */
/* FNARROWWH */
extern unsigned short __builtin_k1_fnarrowwh (float op0);
/* FNEG */
/* FNEGD */
/* FNEGWP */
/* FSBF */
/* FSBFD */
/* FSBFRN */
extern float __builtin_k1_fsbfrn (float op0, float op1);
/* FSBFRND */
extern double __builtin_k1_fsbfrnd (double op0, double op1);
/* FSBFRNWP */
/* FSBFWP */
/* FSDIVW */
extern float __builtin_k1_fsdivw (float, float);
/* FSDIVD */
extern double __builtin_k1_fsdivd (double, double);
/* FSDIVWP */
/* FSINVW */
extern float __builtin_k1_fsinvw (float);
/* FSINVD */
extern double __builtin_k1_fsinvd (double);
/* FSINVWP */
/* FSISRW */
extern float __builtin_k1_fsisrw (float);
/* FSISRD */
extern double __builtin_k1_fsisrd (double);
/* FSISRWP */
/* FWIDENB */
/* FWIDENLHW */
extern float __builtin_k1_fwidenlhw (unsigned int);
/* FWIDENMHW */
extern float __builtin_k1_fwidenmhw (unsigned int);
/* FWIDENHLWP */
/* FWIDENHMWP */

/*
 * Other Builtin Functions
 */

/* Returns SRF size */
extern int __builtin_k1_srfsize (int);

/* get GPR = GPR
 * GPR get( GPR);
 */
extern unsigned long long __builtin_k1_get_r (int);

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

/* xord
 *
 */
extern void __builtin_k1_r_xord (unsigned char, unsigned long long,
				 unsigned short);

/* set nopcpsreg = GPR
 *  set( u[0..63], GPR);
 */
extern void __builtin_k1_set (int, unsigned long long);

/* set onlypsreg = GPR
 *  set( u[0..63], GPR);
 */
extern void __builtin_k1_set_ps (int, unsigned long long);

#ifdef __cplusplus__
}
#endif

#endif

#endif /* _BUILTIN_K1_H_ */
#endif /* __K1__ */
