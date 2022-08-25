#ifndef _DIVVXF3_H_
#define _DIVVXF3_H_

typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;

#define UNUSED(a) a __attribute__((unused))

#define ROUND_MASK 0x3
#define CS_REG     4
#define RND_LOC    16
#define get_round() ((__builtin_kvx_get (CS_REG) >> RND_LOC) & ROUND_MASK)

#define q_snd_rd_compl   q_snd_ru
#define q_snd_ru_compl   q_snd_rd
#define q_snd_rn_compl   q_snd
#define q_snd_rz_compl   q_snd

#define DOUBLE_fp_suffix_0     d
#define DOUBLE_fp_suffix_1     ds
#define DOUBLE_fp_suffix_2     dp
#define DOUBLE_fp_suffix_4     dq

#define FLOAT_fp_suffix_0      w
#define FLOAT_fp_suffix_1      ws
#define FLOAT_fp_suffix_2      wp
#define FLOAT_fp_suffix_4      wq
#define FLOAT_fp_suffix_8      wo

#define DOUBLE_fp_mode_0       df
#define DOUBLE_fp_mode_1       v1df
#define DOUBLE_fp_mode_2       v2df
#define DOUBLE_fp_mode_4       v4df
#define DOUBLE_i_mode_0        di
#define DOUBLE_i_mode_1        v1di
#define DOUBLE_i_mode_2        v2di
#define DOUBLE_i_mode_4        v4di

#define FLOAT_fp_mode_0        sf
#define FLOAT_fp_mode_1        v1sf
#define FLOAT_fp_mode_2        v2sf
#define FLOAT_fp_mode_4        v4sf
#define FLOAT_fp_mode_8        v8sf
#define FLOAT_i_mode_0         si
#define FLOAT_i_mode_1         v1si
#define FLOAT_i_mode_2         v2si
#define FLOAT_i_mode_4         v4si
#define FLOAT_i_mode_8         v8si

#define __divm3                CONCAT3 (__div,fp_mode,3)

#define DOUBLE_vi_elt          int64_t
#define DOUBLE_vi_uelt         uint64_t
#define DOUBLE_vf_elt          double

#define FLOAT_vi_elt           int32_t
#define FLOAT_vi_uelt          uint32_t
#define FLOAT_vf_elt           float

#define fp_suffix              CONCAT3 (FP_TYPE,_fp_suffix_,FP_VECTOR_DEPTH)
#define fp_suffix_0            CONCAT3 (FP_TYPE,_fp_suffix_,0)
#define fp_suffix_1            CONCAT3 (FP_TYPE,_fp_suffix_,1)

#define fp_mode_0              CONCAT3 (FP_TYPE,_fp_mode_,0)
#define i_mode_0               CONCAT3 (FP_TYPE,_i_mode_,0)
#define fp_mode_1              CONCAT3 (FP_TYPE,_fp_mode_,1)
#define i_mode_1               CONCAT3 (FP_TYPE,_i_mode_,1)
#define fp_mode                CONCAT3 (FP_TYPE,_fp_mode_,FP_VECTOR_DEPTH)
#define i_mode                 CONCAT3 (FP_TYPE,_i_mode_,FP_VECTOR_DEPTH)

#define xFLOAT                 0
#define xDOUBLE                1
#define xFP_TYPE               CONCAT2 (x,FP_TYPE)

#define vi_elt                 CONCAT2 (FP_TYPE,_vi_elt)
#define vi_uelt                CONCAT2 (FP_TYPE,_vi_uelt)
#define vf_elt                 CONCAT2 (FP_TYPE,_vf_elt)

#define DOUBLE_SIGN_SZ         1
#define DOUBLE_EXP_SZ          11
#define DOUBLE_MANTISSA_SZ     52
#define DOUBLE_SZ              64

#define DOUBLE_MANTISSA_MASK   0x000fffffffffffff
#define DOUBLE_EXP_MASK        0x7ff

#define DOUBLE_BIAS            1023LL
#define DOUBLE_E_MAX           2046LL
#define DOUBLE_OMEGA           0x7FEFFFFFFFFFFFFFULL

#define DOUBLE_ONE_I           1ULL
#define DOUBLE_Z_I             0ULL

#define DOUBLE_builtin_nan     __builtin_nan
#define DOUBLE_builtin_inf     __builtin_inf

#define FLOAT_SIGN_SZ          1
#define FLOAT_EXP_SZ           8
#define FLOAT_MANTISSA_SZ      23
#define FLOAT_SZ               32

#define FLOAT_MANTISSA_MASK    0x007fffff
#define FLOAT_EXP_MASK         0xff

#define FLOAT_BIAS             127
#define FLOAT_E_MAX            254
#define FLOAT_OMEGA            (0x7F7FFFFFU)

#define FLOAT_ONE_I            1
#define FLOAT_Z_I              0

#define FLOAT_builtin_nan      __builtin_nanf
#define FLOAT_builtin_inf      __builtin_inff

#define MAKE_FP(s,e,m)         (((((s) << FP_EXP_SZ) | (e)) << FP_MANTISSA_SZ) | (m))

#define vi                     CONCAT3(v,FP_VECTOR_DEPTH,i_mode_0)
#define vf                     CONCAT3(v,FP_VECTOR_DEPTH,fp_mode_0)

#define FP_SZ                  CONCAT2 (FP_TYPE,_SZ)
#define FP_BIAS                CONCAT2 (FP_TYPE,_BIAS)
#define FP_MANTISSA_SZ         CONCAT2 (FP_TYPE,_MANTISSA_SZ)
#define FP_EXP_SZ              CONCAT2 (FP_TYPE,_EXP_SZ)
#define FP_EXP_MASK            CONCAT2 (FP_TYPE,_EXP_MASK)
#define FP_MANTISSA_MASK       CONCAT2 (FP_TYPE,_MANTISSA_MASK)
#define FP_SIGN_SZ             CONCAT2 (FP_TYPE,_SIGN_SZ)
#define FP_E_MAX               CONCAT2 (FP_TYPE,_E_MAX)
#define FP_OMEGA               CONCAT2 (FP_TYPE,_OMEGA)
#define FP_ONE_I               CONCAT2 (FP_TYPE,_ONE_I)
#define FP_Z_I                 CONCAT2 (FP_TYPE,_Z_I)
#define FP_builtin_nan         CONCAT2 (FP_TYPE,_builtin_nan)
#define FP_builtin_inf         CONCAT2 (FP_TYPE,_builtin_inf)

#define FP_MANTISSA_OF(a)      ((a) & FP_MANTISSA_MASK)
#define FP_EXP_OF(a)           (((a) >> FP_MANTISSA_SZ) & FP_EXP_MASK)
#define FP_SIGN_OF(a)          (((a) >> (FP_SZ - 1)) & 0x1)
#define FP_SET_EXP_OF(a,exp)   vi2vf((vf2vi((a)) & MAKE_FP(FP_ONE_I,FP_Z_I,FP_MANTISSA_MASK)) | ((exp) << (FP_MANTISSA_SZ)));
#define FP_SET_SIGN_OF(a,sign) vi2vf((vf2vi((a)) & MAKE_FP(FP_Z_I,FP_EXP_MASK,FP_MANTISSA_MASK)) | ((sign) << (FP_SZ - 1)))

#define CONCAT2_(a,b)          a ## b
#define CONCAT3_(a,b,c)        a ## b ## c
#define CONCAT4_(a,b,c,d)      a ## b ## c ## d
#define QUOTE(a)               #a
#define PROTECT(a)             QUOTE (a)
#define CONCAT2(a,b)           CONCAT2_ (a,b)
#define CONCAT3(a,b,c)         CONCAT3_ (a,b,c)
#define CONCAT4(a,b,c,d)       CONCAT4_ (a,b,c,d)

#define expand_mk_vect(n,k) k

#define mk_vect(k) \
{ \
  CONCAT2 (expand_initializer_,FP_VECTOR_DEPTH) (mk_vect,k) \
}

#define expand_initializer_8(expander, ...) \
  expand_initializer_7(expander, __VA_ARGS__) \
  expand_ ## expander(7, __VA_ARGS__),

#define expand_initializer_7(expander, ...) \
  expand_initializer_6(expander, __VA_ARGS__) \
  expand_ ## expander(6, __VA_ARGS__),

#define expand_initializer_6(expander, ...) \
  expand_initializer_5(expander, __VA_ARGS__) \
  expand_ ## expander(5, __VA_ARGS__),

#define expand_initializer_5(expander, ...) \
  expand_initializer_4(expander, __VA_ARGS__) \
  expand_ ## expander(4, __VA_ARGS__),

#define expand_initializer_4(expander, ...) \
  expand_initializer_3(expander, __VA_ARGS__) \
  expand_ ## expander(3, __VA_ARGS__),

#define expand_initializer_3(expander, ...) \
  expand_initializer_2(expander, __VA_ARGS__) \
  expand_ ## expander(2, __VA_ARGS__),

#define expand_initializer_2(expander, ...) \
  expand_initializer_1(expander, __VA_ARGS__) \
  expand_ ## expander(1, __VA_ARGS__),

#define expand_initializer_1(expander, ...) \
  expand_ ## expander(0, __VA_ARGS__),

typedef vf_elt fp_mode_1 __attribute__((__vector_size__ (1 * sizeof(vf_elt))));
typedef vi_elt i_mode_1 __attribute__((__vector_size__ (1 * sizeof(vi_elt))));

typedef vf_elt fp_mode __attribute__((__vector_size__ (FP_VECTOR_DEPTH * sizeof(vf_elt))));
typedef vi_elt i_mode  __attribute__((__vector_size__ (FP_VECTOR_DEPTH * sizeof(vi_elt))));

static inline fp_mode_1
CONCAT2 (__builtin_kvx_select, fp_suffix_1) (fp_mode_1 a, fp_mode_1 b, i_mode_1 sel, const char UNUSED (*mode))
{
  fp_mode_1 res = { CONCAT2 (__builtin_kvx_selectf, fp_suffix_0) (*(vf_elt*)&a, *(vf_elt*)&b, *(vi_elt*)&sel, ".dnez") };
  return res;
}

static inline fp_mode_1
CONCAT2 (__builtin_kvx_fsrec, fp_suffix_1) (fp_mode_1 a, const char *mode)
{
  if (!__builtin_strcmp (mode, ".s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_fsrec, fp_suffix_0) (*(vf_elt *)&a, ".s") };
    return res;
  } else {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_fsrec, fp_suffix_0) (*(vf_elt *)&a, "") };
    return res;
  }
}

#if xFP_TYPE==xDOUBLE && FP_VECTOR_DEPTH > 1
static inline fp_mode
CONCAT2 (__builtin_kvx_frec, fp_suffix) (fp_mode a, const char UNUSED(*mode))
{
  return a;
}
#endif

static inline fp_mode_1
CONCAT2 (__builtin_kvx_frec, fp_suffix_1) (fp_mode_1 a, const char UNUSED(*mode))
{
#if xFP_TYPE==xFLOAT
  if (!__builtin_strcmp (mode, ".s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_frec, fp_suffix_0) (*(vf_elt *)&a, ".s") };
    return res;
  } else {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_frec, fp_suffix_0) (*(vf_elt *)&a, "") };
    return res;
  }
#else
  return a;
#endif
}

static inline fp_mode_1
CONCAT2 (__builtin_kvx_ffms, fp_suffix_1) (fp_mode_1 a, fp_mode_1 b, fp_mode_1 c, const char UNUSED(*mode))
{

  if (!__builtin_strcmp (mode, ".rn.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffms, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".rn.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".ru.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffms, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".ru.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".rd.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffms, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".rd.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".rz.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffms, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".rz.s") };
    return res;
  } else {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffms, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, "") };
    return res;
  }
}

static inline fp_mode_1
CONCAT2 (__builtin_kvx_ffma, fp_suffix_1) (fp_mode_1 a, fp_mode_1 b, fp_mode_1 c, const char *mode)
{

  if (!__builtin_strcmp (mode, ".rn.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffma, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".rn.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".ru.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffma, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".ru.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".rd.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffma, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".rd.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".rz.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffma, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, ".rz.s") };
    return res;
  } else {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_ffma, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, *(vf_elt *)&c, "") };
    return res;
  }
}

static inline fp_mode_1
CONCAT2 (__builtin_kvx_fmul, fp_suffix_1) (fp_mode_1 a, fp_mode_1 b, const char *mode)
{
  if (!__builtin_strcmp (mode, ".rn.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_fmul, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, ".rn.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".ru.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_fmul, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, ".ru.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".rd.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_fmul, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, ".rd.s") };
    return res;
  } else if (!__builtin_strcmp (mode, ".rz.s")) {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_fmul, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, ".rz.s") };
    return res;
  } else {
    fp_mode_1 res = { CONCAT2 (__builtin_kvx_fmul, fp_suffix_0) (*(vf_elt *)&a, *(vf_elt *)&b, "") };
    return res;
  }
}

static inline i_mode_1
CONCAT2 (__builtin_kvx_bitcnt, fp_suffix_1) (i_mode_1 a, const char UNUSED(*mode))
{
    i_mode_1 res = { CONCAT2 (__builtin_kvx_bitcnt, fp_suffix_0) (*(vi_elt *)&a, ".lz") };
    return res;
}

#define __builtin_kvx_fmul     CONCAT2 (__builtin_kvx_fmul,fp_suffix)
#define __builtin_kvx_ffms     CONCAT2 (__builtin_kvx_ffms,fp_suffix)
#define __builtin_kvx_ffma     CONCAT2 (__builtin_kvx_ffma,fp_suffix)
#define __builtin_kvx_select   CONCAT2 (__builtin_kvx_select,fp_suffix)
#define __builtin_kvx_fsrec    CONCAT2 (__builtin_kvx_fsrec,fp_suffix)
#define __builtin_kvx_frec     CONCAT2 (__builtin_kvx_frec,fp_suffix)
#define __builtin_kvx_bitcnt   CONCAT2 (__builtin_kvx_bitcnt,fp_suffix)

static inline fp_mode_1
CONCAT2 (dbl2, fp_mode_1) (vf_elt d) {
  fp_mode_1 res;
  __builtin_memcpy ((void *)&res, (void *)&d, sizeof d);
  return res;
}

static inline vf_elt
CONCAT2 (fp_mode_1, 2dbl) (fp_mode_1 d) {
  vf_elt res;
  __builtin_memcpy ((void *)&res, (void *)&d, sizeof d);
  return res;
}

#define TO_VECT(x) CONCAT2 (dbl2, fp_mode_1) (x)
#define TO_SCAL(x) CONCAT2 (fp_mode_1, 2dbl) (x)

static inline vi
vf2vi (vf v)
{
  vi res;
  __builtin_memcpy ((void *)&res, (void *)&v, sizeof v);
  return res;
}

static inline vf
vi2vf (vi v)
{
  vf res;
  __builtin_memcpy ((void *)&res, (void *)&v, sizeof v);
  return res;
}

static inline vi
is_nan (vf a)
{
  return a != a;
}

static inline vi
is_inf (vf a)
{
  vi b;
  __builtin_memcpy ((void *)&b, (void *)&a, sizeof a);
  return ((b & MAKE_FP (FP_Z_I, FP_EXP_MASK, FP_MANTISSA_MASK)) ==
	MAKE_FP (FP_Z_I, FP_EXP_MASK, FP_Z_I));
}

#define define_divm3(mode)                                                    \
vf CONCAT3 (__divm3, _, mode) (vf a, vf b);                                   \
vf                                                                            \
CONCAT3 (__divm3, _, mode) (vf a, vf b)                                       \
{                                                                             \
  vi _a = vf2vi (a), _b = vf2vi(b);                                           \
                                                                              \
  vi cond_inf_a   = is_inf (a);                                               \
  vi cond_inf_b   = is_inf (b);                                               \
  vi cond_nan_a   = is_nan (a);                                               \
  vi cond_nan_b   = is_nan (b);                                               \
  vi cond_all_inf = cond_inf_a & cond_inf_b;                                  \
  vi cond_b_0     = b == 0;                                                   \
  vi cond_a_0     = a == 0;                                                   \
  vi cond_all_0   = cond_b_0 & cond_a_0;                                      \
  vi a_is_b_p     = (_a == _b) | cond_all_inf | cond_all_0;                   \
  vi cond_nan_neg = a_is_b_p & (cond_a_0 | cond_inf_a | cond_nan_a);          \
  vi cond_nan_pos = cond_nan_a | cond_nan_b;                                  \
  vi a_exp        = FP_EXP_OF(_a);                                            \
  vi b_exp        = FP_EXP_OF(_b);                                            \
  vi a_m          = FP_MANTISSA_OF(_a);                                       \
  vi b_m          = FP_MANTISSA_OF(_b);                                       \
  vf one_v1df     = mk_vect (1);                                              \
                                                                              \
  /**                                                                         \
   * 1. Computation of the initial shift                                      \
   *                                                                          \
   * Floating-point numbers are in base-2 exponential notation with an        \
   * implicit leading 1. This means that, unless further correction are       \
   * needed, the exponent of the result is the difference the                 \
   * exponents.                                                               \
   **/                                                                        \
  vi shift = a_exp - b_exp;                                                   \
                                                                              \
  /**                                                                         \
   * 2. Normalize                                                             \
   *                                                                          \
   * If the numbers are subnormals, their exponents is 0 but the              \
   * significant is not. A_SUBNORMAL_P and B_SUBNORMAL_P are predicates       \
   * whose components holds                                                   \
   * -1 where those are subnormals.                                           \
   *                                                                          \
   * During the normalization process, they are shifted such that there       \
   * is no more leading zeroes.  Those leading zeroes are stored in LZA       \
   * and LZB, and the difference of those counts is stored in SHIFT.          \
   **/                                                                        \
  vi lza = __builtin_kvx_bitcnt (a_m, ".lz") - (FP_SIGN_SZ + FP_EXP_SZ);      \
  vi lzb = __builtin_kvx_bitcnt (b_m, ".lz") - (FP_SIGN_SZ + FP_EXP_SZ);      \
  vi a_subnormal_p = (a_exp == 0) & ~cond_a_0;                                \
  vi b_subnormal_p = (b_exp == 0) & ~cond_b_0;                                \
  shift += (a_subnormal_p & -lza) + (b_subnormal_p & lzb);                    \
                                                                              \
  /**                                                                         \
   * 3. Only keep the significand                                             \
   *                                                                          \
   * From now on, the division works with SA and SB, which are floating-point \
   * numbers with the bias as the exponent.                                   \
   **/                                                                        \
  vf sa = vi2vf (MAKE_FP(FP_Z_I,FP_BIAS,(a_m<<(a_subnormal_p & (lza + 1))))); \
  vf sb = vi2vf (MAKE_FP(FP_Z_I,FP_BIAS,(b_m<<(b_subnormal_p & (lzb + 1))))); \
  vi sign_bit = FP_SIGN_OF(_a ^ _b);                                          \
                                                                              \
  /**                                                                         \
   * 4. Compute (an approximation of) 1/b                                     \
   *                                                                          \
   * We use single-precision floating point we can directly get the           \
   * approximate value by using __builtin_kvx_frec.  Otherwise, we            \
   * first get a seed with  __builtin_kvx_fsrec and iterate it.               \
   **/                                                                        \
  vf y3;                                                                      \
                                                                              \
  if (!__builtin_strcmp(PROTECT(FP_TYPE), "DOUBLE"))                          \
    {                                                                         \
      vf y0     = __builtin_kvx_fsrec (sb, "");                               \
                                                                              \
      /**                                                                     \
       * Iterate the approximation.                                           \
       *                                                                      \
       * For the Goldsmith iteration see eq. 4.12 in Handbook of              \
       * Floating-point Arithmetic. (p129, second edition)                    \
       *                                                                      \
       * For the Newton-Raphson iteration see eq. 4.9 in Handbook of          \
       * Floating-point Arithmetic. (p129, second edition)                    \
       *                                                                      \
       * The main difference between those iterations steps is that the       \
       * Goldsmith iteration step exposes instruction-level parallelism,      \
       * but is not self-correcting.  Therefore, the algorithm here, first    \
       * apply to Goldsmith step and finish by a Newton-Raphson step so       \
       * that we get self-correction on the last step.                        \
       **/                                                                    \
      /* 1st Goldsmith iteration */                                           \
      vf eps0 = __builtin_kvx_ffms (sb, y0, one_v1df, ".rn.s");               \
      vf y1   = __builtin_kvx_ffma (y0, eps0, y0, ".rn.s");                   \
      /* 2nd Goldsmith iteration */                                           \
      vf eps1 = __builtin_kvx_fmul (eps0, eps0, ".rn.s");                     \
      vf y2   = __builtin_kvx_ffma (y1, eps1, y1, ".rn.s");                   \
      /* 1st Newton-Raphson iteration */                                      \
      vf r    = __builtin_kvx_ffms (sb, y2, one_v1df, ".rn.s");               \
      y3      = __builtin_kvx_ffma (r, y2, y2, ".rn.s");                      \
    }                                                                         \
  else if (!__builtin_strcmp(PROTECT(FP_TYPE), "FLOAT"))                      \
      y3      = __builtin_kvx_frec (sb, "");                                  \
                                                                              \
  /**                                                                         \
   * 6. Apply Theorem 2                                                       \
   *                                                                          \
   * Cf. [1] Scaling Newton-Raphson division iterations to avoid double       \
   * rounding, by Jean-Michel Muller.                                         \
   *                                                                          \
   * NB. All along [1] computes B/A, but here we compute A/B so our use       \
   * and their use of A and B are flipped.                                    \
   * */                                                                       \
  vf q      = __builtin_kvx_fmul (sa, y3, ".rn.s");                           \
                                                                              \
  vf r1     = __builtin_kvx_ffms (sb, q, sa, ".rn.s");                        \
  vf q_star = __builtin_kvx_ffma (r1, y3, q, ".rn.s");                        \
                                                                              \
  vf r2     = __builtin_kvx_ffms (sb, q_star, sa, "." #mode ".s");            \
  vf q_snd  = __builtin_kvx_ffma (r2, y3, q_star, "." #mode ".s");            \
                                                                              \
  vf res = q_snd;                                                             \
  if (!__builtin_strcmp (#mode, "ru") || !__builtin_strcmp (#mode, "rd"))     \
    {                                                                         \
      vf UNUSED (q_snd_rd) = __builtin_kvx_ffma (r2, y3, q_star, ".rd.s");    \
      vf UNUSED (q_snd_ru) = __builtin_kvx_ffma (r2, y3, q_star, ".ru.s");    \
      res = vi2vf( ((sign_bit == 1) & vf2vi(CONCAT3(q_snd_,mode,_compl)))     \
	         | ((sign_bit == 0) & vf2vi(q_snd)));                         \
    }                                                                         \
                                                                              \
  /**                                                                         \
   * 7. Rectify the exponent                                                  \
   *                                                                          \
   * If the result from the normalized numbers has a non null exponent,       \
   * we have to add it to the SHIFT value.                                    \
   **/                                                                        \
  shift += FP_EXP_OF (vf2vi(res));                                            \
                                                                              \
  /**                                                                         \
   * 8. Compute the subnormal shift (if round to even)                        \
   *                                                                          \
   * This is a consequence of apply Theorem 2 on scaled iterations.  We       \
   * may hit a subnormal midpoint, and thus have to check whether this        \
   * is the case.                                                             \
   *                                                                          \
   * If we are on a subnormal midpoint we will need to round up or down       \
   * according to the sign of R2. (cf. step 10)                               \
   *                                                                          \
   * BEWARE: Do not move this computation, it has to be done before the       \
   * rescaling.                                                               \
   **/                                                                        \
  vi subnormal_shift;                                                         \
  if (!__builtin_strcmp (#mode, "rn"))                                        \
    subnormal_shift = (shift <= 0) & (-shift < (FP_MANTISSA_SZ + 2)) &        \
    (((FP_MANTISSA_OF(vf2vi(res)) | 1ULL << FP_MANTISSA_SZ)                   \
    & ((1ULL << (1 - shift)) - 1)) == (1ULL << (-shift)));                    \
                                                                              \
  /**                                                                         \
   * 9. Rescaling                                                             \
   **/                                                                        \
  /**                                                                         \
   * 9.1 Prescaling                                                           \
   *                                                                          \
   * Ensure the correct rounding of the result.                               \
   **/                                                                        \
  vi offset = 10*(((shift<=-(FP_MANTISSA_SZ))&1) - ((shift>=FP_E_MAX)&1));    \
  shift += offset;                                                            \
                                                                              \
  /**                                                                         \
   * 9.2 Actual rescaling                                                     \
   **/                                                                        \
  vf scale = vi2vf(                                                           \
    ((shift <= 0) &                                                           \
      (((-shift > FP_MANTISSA_SZ) & 1) + (~(-shift > FP_MANTISSA_SZ) &        \
        (1ULL << (FP_MANTISSA_SZ + (shift - 1))))))                           \
    | ((shift > FP_E_MAX) & FP_OMEGA)                                         \
    | (((0 < shift) & (shift <= FP_E_MAX)) & (shift << FP_MANTISSA_SZ)));     \
                                                                              \
  vf resd_ = FP_SET_EXP_OF(res, FP_BIAS - offset);                            \
  vf resd  = __builtin_kvx_fmul (resd_, scale, "." #mode ".s");               \
                                                                              \
  /**                                                                         \
   * 10. Apply subnormal shift (if round to even)                             \
   *                                                                          \
   * cf. [1] (Avoid Double Rounding)                                          \
   *                                                                          \
   * We only compute the rounded down value and select between the            \
   * rounded down and the rounded up value by adding one or nothing to        \
   * the hexadecimal reprensentation of the rounded down value.               \
   **/                                                                        \
  if (!__builtin_strcmp (#mode, "rn"))                                        \
    {                                                                         \
      vi resd_d   = vf2vi(__builtin_kvx_fmul (resd_, scale, ".rd.s"));        \
      vf resd_vec = vi2vf(                                                    \
          (((subnormal_shift != 0) & (r2 != 0)) &                             \
           (resd_d + ((((q_star != q_snd) & ~(r2 > 0)) +                      \
                      (~(q_star != q_snd) &  (r2 > 0))) & 1)))                \
          | (~((subnormal_shift != 0) & (r2 != 0)) & vf2vi(resd)));           \
      resd = resd_vec;                                                        \
    }                                                                         \
                                                                              \
  vi res_exp = FP_EXP_OF(vf2vi((resd)));                                      \
  vi res_m   = FP_MANTISSA_OF(vf2vi((resd)));                                 \
  vf _res0f  = vi2vf(MAKE_FP(sign_bit,res_exp,res_m));                        \
                                                                              \
  /**                                                                         \
   * 11. Handle exceptional values                                            \
   **/                                                                        \
  vf mnan_vf  = mk_vect(-FP_builtin_nan (""));                                \
  vf  nan_vf  = mk_vect(FP_builtin_nan (""));                                 \
  vf  inf_vf_ = mk_vect(FP_builtin_inf ());                                   \
  vf  inf_vf  = FP_SET_SIGN_OF(inf_vf_, sign_bit);                            \
  vf    a_vf  = FP_SET_SIGN_OF(a, sign_bit);                                  \
  vf    z_vf  = vi2vf(MAKE_FP(sign_bit, FP_Z_I, FP_Z_I));                     \
  vi sel1 = cond_nan_neg;                                                     \
  vi sel2 = ~(sel1) & cond_nan_pos;                                           \
  vi sel3 = ~(sel1) & ~(sel2) & cond_inf_b;                                   \
  vi sel4 = ~(sel1) & ~(sel2) & ~(sel3) & cond_inf_a;                         \
  vi sel5 = ~(sel1) & ~(sel2) & ~(sel3) & ~(sel4) & cond_b_0;                 \
  vi sel6 = ~(sel1) & ~(sel2) & ~(sel3) & ~(sel4) & ~(sel5) & cond_a_0;       \
  _res0f = __builtin_kvx_select (mnan_vf, _res0f, sel1, ".nez");              \
  _res0f = __builtin_kvx_select (nan_vf,  _res0f, sel2, ".nez");              \
  _res0f = __builtin_kvx_select (z_vf,    _res0f, sel3, ".nez");              \
  _res0f = __builtin_kvx_select (a_vf,    _res0f, sel4, ".nez");              \
  _res0f = __builtin_kvx_select (inf_vf,  _res0f, sel5, ".nez");              \
  _res0f = __builtin_kvx_select (z_vf,    _res0f, sel6, ".nez");              \
  return _res0f;                                                              \
}

#define define_selector                                                       \
fp_mode (*CONCAT2 (__divm3, _rnd)[]) (fp_mode, fp_mode) = {                   \
  CONCAT2 (__divm3, _rn), CONCAT2 (__divm3, _rd),                             \
  CONCAT2 (__divm3, _ru), CONCAT2 (__divm3, _rz),                             \
};

#define selector CONCAT2 (__divm3, _rnd)

#endif //_DIVXF3_H_
