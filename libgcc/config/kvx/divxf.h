#ifndef _DIVXF3_H_
#define _DIVXF3_H_

/* PAPERS
  - MULLER, Jean-Michel. Scaling Newton-Raphson division iterations to avoid
  double rounding. 2010. Floating Point Handbook (p135 especially) also helped.
  - VIITANEN, Timo, JÄÄSKELÄINEN, Pekka, et TAKALA, Jarmo. Inexpensive correctly
  rounded floating-point division and square root with input scaling. In: SiPS
  2013 Proceedings. IEEE, 2013. p. 159-164.  This paper has 1 million typos in
  the algos
*/

typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;

#define IS_NAN(X) (X != X)

#define INEXACT_CST    (0x20ULL)
#define UNDERFLOW_CST  (0x10ULL)
#define OVERFLOW_CST   (0x8ULL)
#define DIVZERO_CST    (0x4ULL)
#define INVALID_CST    (0x2ULL)
#define EXCEPTION_MASK (0x3E)

/* #define RN         (0x0) */
/* #define RU         (0x1) */
/* #define RD         (0x2) */
/* #define RZ         (0x3) */
#define ROUND_MASK (0x3)

// WFXL: first word is set mask, other is clear mask -> no clearing so second
// word is 0
#define RAISE_INEXACT (__builtin_kvx_wfxl (4, INEXACT_CST << 32))
#define RAISE_UNDERFLOW                                                        \
  (__builtin_kvx_wfxl (4, (UNDERFLOW_CST | INEXACT_CST) << 32)) // underflow always raised when inexact
#define RAISE_OVERFLOW (__builtin_kvx_wfxl (4, OVERFLOW_CST << 32))
#define RAISE_DIVZERO (__builtin_kvx_wfxl (4, DIVZERO_CST << 32))
#define RAISE_INVALID (__builtin_kvx_wfxl (4, INVALID_CST << 32))

#define RESET_EV (__builtin_kvx_wfxl (4, EXCEPTION_MASK))

// returns: Inexact | Underflow | Overflow | Div by zero | Invalid | 0
#define GET_EV ((__builtin_kvx_get (4) & EXCEPTION_MASK))
#define OR_SET_EV(EV) (__builtin_kvx_wfxl (4, EV << 32))

#define GET_ROUND ((__builtin_kvx_get (4) >> 16) & ROUND_MASK)

#ifndef DEBUG
#define DEBUG 0
#endif

#define SILENT 0

#if SILENT == 1
#define SILENT_STRING ".s"
#else
#define SILENT_STRING ""
#endif

// Double
typedef struct double_bit_field_s
{
  uint64_t m:52;
  uint64_t e:11;
  uint64_t s:1;
} double_bit_field;

typedef union double_uint_u
{
  double d;
  uint64_t u;
  double_bit_field f;
} double_uint;

double __divdf3 (double a, double b);
double __divdf3_RN (double a, double b);
double __divdf3_RU (double a, double b);
double __divdf3_RD (double a, double b);
double __divdf3_RZ (double a, double b);

// Float
typedef struct float_bit_field_s
{
  uint32_t m:23;
  uint32_t e:8;
  uint32_t s:1;
} float_bit_field;

typedef union float_uint_u
{
  float d;
  uint32_t u;
  float_bit_field f;
} float_uint;

float __divsf3 (float a, float b);
float __divsf3_RN (float a, float b);
float __divsf3_RU (float a, float b);
float __divsf3_RD (float a, float b);
float __divsf3_RZ (float a, float b);

#define CONCAT2_(a,b)         a ## b
#define CONCAT3_(a,b,c)       a ## b ## c
#define CONCAT4_(a,b,c,d)     a ## b ## c ## d
#define QUOTE(a)              #a
#define PROTECT(a)            QUOTE(a)
#define CONCAT2(a,b)          CONCAT2_(a,b)
#define CONCAT3(a,b,c)        CONCAT3_(a,b,c)
#define CONCAT4(a,b,c,d)      CONCAT4_(a,b,c,d)

#define FP_TYPE_SIZE          CONCAT2(FP_TYPE,_SIZE)
#define FP_UINT               CONCAT2(fp_type,_uint)
#define IS_INF                CONCAT3(IS_,FP_TYPE,_INF)
#define IS_SNAN               CONCAT3(IS_,FP_TYPE,_SNAN)
#define QNAN                  CONCAT2(FP_TYPE,_QNAN)
#define FP_TYPE_INF           CONCAT2(FP_TYPE,_INF)
#define FP_TYPE_BIAS          CONCAT2(FP_TYPE,_BIAS)
#define FP_TYPE_MANTISSA_SIZE CONCAT2(FP_TYPE,_MANTISSA_SIZE)
#define normalize_fp          CONCAT2(normalize_,fp_type)
#define FP_TYPE_E_MAX         CONCAT2(FP_TYPE,_E_MAX)
#define FP_TYPE_OMEGA         CONCAT2(FP_TYPE,_OMEGA)

#define __builtin_kvx_fmul    CONCAT2(__builtin_kvx_fmul,fp_suffix)
#define __builtin_kvx_ffms    CONCAT2(__builtin_kvx_ffms,fp_suffix)
#define __builtin_kvx_ffma    CONCAT2(__builtin_kvx_ffma,fp_suffix)

#define __divm3               CONCAT3(__div,fp_mode,3)
#define __divm3_RN            CONCAT3(__div,fp_mode,3_RN)
#define __divm3_RU            CONCAT3(__div,fp_mode,3_RU)
#define __divm3_RD            CONCAT3(__div,fp_mode,3_RD)
#define __divm3_RZ            CONCAT3(__div,fp_mode,3_RZ)
#define __divm3_RN_tail       CONCAT3(__div,fp_mode,3_RN_tail)
#define __divm3_RU_tail       CONCAT3(__div,fp_mode,3_RU_tail)
#define __divm3_RD_tail       CONCAT3(__div,fp_mode,3_RD_tail)
#define __divm3_RZ_tail       CONCAT3(__div,fp_mode,3_RZ_tail)

/* Renormalize float subnormals, returns the exponent shift needed. */
#define define_normalize                                                      \
static inline int                                                             \
CONCAT2(normalize_, fp_type) (FP_UINT *d)                                     \
{                                                                             \
  /* -9 (8 bits of exponent and bit of sign) */                               \
  unsigned lz = clz (d->f.m) - (FP_TYPE_SIZE - FP_TYPE_MANTISSA_SIZE);        \
  /* +1 for implicit bit (then discarded with assign) */                      \
  UINT m_shifted = ((UINT) d->f.m) << (lz + 1);                               \
  int ret = 0;                                                                \
  if (d->f.e == 0 && d->d != 0)                                               \
    {                                                                         \
      d->f.m = m_shifted;                                                     \
      ret = -lz;                                                              \
    }                                                                         \
  return ret;                                                                 \
}                                                                             \

#define define_divm3_generic                                                  \
__attribute__ ((noinline)) fp_type                                            \
__divm3 (fp_type a, fp_type b)                                                \
{                                                                             \
  uint64_t ev = 0;                                                            \
                                                                              \
  int round_mode = GET_ROUND;                                                 \
                                                                              \
  /* Step 1: scale floats */                                                  \
  FP_UINT da = { .d = a };                                                    \
  FP_UINT db = { .d = b };                                                    \
  FP_UINT dres;                                                               \
  int res_sign = da.f.s ^ db.f.s;                                             \
                                                                              \
  /* Special cases */                                                         \
  if (!handle_special_cases (da, db, &dres, &ev))                             \
    return dres.d;                                                            \
                                                                              \
  /* normalize when subnormals, computes rescale factor */                    \
  int ra = normalize_fp (&da);                                                \
  int rb = normalize_fp (&db);                                                \
                                                                              \
  /* rescaled a and b */                                                      \
  FP_UINT r_da = {.f.s = 0,.f.m = da.f.m,.f.e = FP_TYPE_BIAS };               \
  FP_UINT r_db = {.f.s = 0,.f.m = db.f.m,.f.e = FP_TYPE_BIAS };               \
  /* scale factor */                                                          \
  int32_t k = (da.f.e + ra) - (db.f.e + rb);                                  \
                                                                              \
  /* Step 2 & 3 & 4 */                                                        \
  fp_type y3, q_star;                                                         \
  iterate_approx (r_da, r_db, &q_star, &y3);                                  \
                                                                              \
  /* r2 is exact (see paper) */                                               \
  fp_type r2 = __builtin_kvx_ffms (r_db.d, q_star, r_da.d, ".rn.s");          \
                                                                              \
  /* second markstein depends on rounding mode */                             \
  return __divm3_rx_tail[round_mode] (r2, q_star, y3, k, res_sign, &ev);      \
}                                                                             \

#define define_divm3_tail(MODE1,mode1,mode2)                                  \
static inline fp_type                                                         \
CONCAT4(__divm3,_,MODE1,_tail) (fp_type r2, fp_type q_star, fp_type y3,       \
    int k, int res_sign, uint64_t * ev)                                       \
{                                                                             \
  FP_UINT r_dq;                                                               \
  /* Not silent, will detect inexact, use CS rounding mode */                 \
  fp_type q_snd    = __builtin_kvx_ffma (r2, y3, q_star, ""    SILENT_STRING);\
  fp_type q_snd_ru = __builtin_kvx_ffma (r2, y3, q_star, ".ru" SILENT_STRING);\
  fp_type q_snd_rd = __builtin_kvx_ffma (r2, y3, q_star, ".rd" SILENT_STRING);\
                                                                              \
  /* To detect inexact without doubt, compare results in round up             \
   * and round down mode */                                                   \
  int is_inexact;                                                             \
  if (!SILENT)                                                                \
    is_inexact = is_inexact_p (q_snd_ru, q_snd_rd);                           \
                                                                              \
  if (!__builtin_strcmp (#mode1, "rn") || !__builtin_strcmp (#mode1, "rz"))   \
    r_dq.d = q_snd;                                                           \
  else                                                                        \
    r_dq.d = res_sign ? q_snd_ ## mode2 : q_snd;                              \
                                                                              \
  /* Step 5: detect midpoint subnormal */                                     \
  int final_exp = r_dq.f.e + k;                                               \
  int subnormal_midpoint_detected = 0;                                        \
  int subnormal_shift = 0;                                                    \
  if (!__builtin_strcmp (#mode1, "rn"))                                                 \
    {                                                                         \
      /* calculate with explicit bit in case that bit ends up                 \
       * being the round bit */                                               \
      subnormal_shift = ((r_dq.f. m | ONE << FP_TYPE_MANTISSA_SIZE)           \
          & ((ONE << (-final_exp + 1)) - 1)) == (ONE << (-final_exp));        \
        /* result will be subnormal */                                        \
      if (final_exp <= 0 && -final_exp < (FP_TYPE_MANTISSA_SIZE + 2))         \
          subnormal_midpoint_detected = subnormal_shift;                      \
    }                                                                         \
                                                                              \
  /* Step 6: rescale float */                                                 \
  FP_UINT scale = rescale_float (&final_exp, &r_dq, is_inexact, ev);          \
                                                                              \
  /* the rescale operation in the general case */                             \
  FP_UINT res;                                                                \
  fp_type res_r_cs = res.d =                                                  \
    __builtin_kvx_fmul (r_dq.d, scale.d, "" SILENT_STRING);                   \
                                                                              \
  if (!__builtin_strcmp (#mode1, "rn"))                                       \
    {                                                                         \
      /* Step 7: treat midpoint subnormal case, only in round to nearest      \
       * case (here, in case even, if RNA needs another logic) */             \
      fp_type res_rd =                                                        \
        __builtin_kvx_fmul (r_dq.d, scale.d, ".rd" SILENT_STRING);            \
      fp_type res_ru =                                                        \
        __builtin_kvx_fmul (r_dq.d, scale.d, ".ru" SILENT_STRING);            \
                                                                              \
      if (subnormal_midpoint_detected && r2)                                  \
        {                                                                     \
          if (q_star != q_snd)                                                \
            res.d = r2 > 0 ? res_rd : res_ru;                                 \
          else                                                                \
            res.d = r2 > 0 ? res_ru : res_rd;                                 \
        }                                                                     \
    }                                                                         \
                                                                              \
  res.f.s = res_sign;                                                         \
  return res.d;                                                               \
}                                                                             \

#define define_specialized_divm3(MODE1,mode1,mode2)                           \
__attribute__((noinline)) fp_type                                             \
CONCAT2(__divm3,MODE1) (fp_type a, fp_type b)                                 \
{                                                                             \
  uint64_t ev = 0;                                                            \
                                                                              \
  /* Step 1: scale floats */                                                  \
  FP_UINT da = { .d = a}, db = { .d = b}, dres;                               \
  int res_sign = da.f.s ^ db.f.s;                                             \
  UINT res_sign_full = ((UINT) res_sign) << (FP_TYPE_SIZE - 1);               \
                                                                              \
  /* Special cases */                                                         \
  if (!handle_special_cases (da, db, &dres, &ev))                             \
    return dres.d;                                                            \
                                                                              \
  /* normalize when subnormals, computes rescale factor */                    \
  int ra = normalize_fp (&da);                                                \
  int rb = normalize_fp (&db);                                                \
                                                                              \
  /* rescaled a and b */                                                      \
  FP_UINT r_da = {.f.s = 0, .f.m = da.f.m, .f.e = FP_TYPE_BIAS};              \
  FP_UINT r_db = {.f.s = 0, .f.m = db.f.m, .f.e = FP_TYPE_BIAS};              \
  /* scale factor */                                                          \
  int32_t k = (da.f.e + ra) - (db.f.e + rb);                                  \
                                                                              \
  /* Step 2: get seed for 1/b */                                              \
  fp_type y3, q_star;                                                         \
  iterate_approx (r_da, r_db, &y3, &q_star);                                  \
                                                                              \
  fp_type r2 = __builtin_kvx_ffms (r_db.d, q_star, r_da.d,                    \
                                      "." #mode1 ".s");                       \
  /* Not silent, will detect inexact */                                       \
  fp_type q_snd = __builtin_kvx_ffma (r2, y3, q_star,                         \
                                         "." #mode1 SILENT_STRING);           \
                                                                              \
  /* Not silent, will detect inexact */                                       \
  fp_type q_snd_ ## mode2 = __builtin_kvx_ffma (r2, y3, q_star,               \
                                                   "." #mode2 SILENT_STRING); \
                                                                              \
  /* To detect inexact without doubt, compare results in round up and round   \
   * down mode */                                                             \
  int is_inexact = is_inexact_p (                                             \
      __builtin_kvx_ffma (r2, y3, q_star, ".ru.s"),                           \
      __builtin_kvx_ffma (r2, y3, q_star, ".rd.s"));                          \
                                                                              \
  FP_UINT r_dq = {                                                            \
    .d = ((__builtin_strcmp(#mode1, "rn")) || __builtin_strcmp(#mode1, "rz")) \
         ? q_snd                                                              \
         : (res_sign ? q_snd_ ## mode2 : q_snd) };                            \
                                                                              \
  /* Step 5: detect midpoint subnormal */                                     \
  int final_exp = r_dq.f.e + k;                                               \
  int subnormal_midpoint_detected = 0;                                        \
  int subnormal_shift = 0;                                                    \
  if (!__builtin_strcmp (#mode1, "rn"))                                       \
    {                                                                         \
      int subnormal_midpoint_detected = 0;                                    \
      /* calculate with explicit bit in case that bit ends up being the round \
       * bit */                                                               \
      int subnormal_shift = ((r_dq.f.m | ONE << FP_TYPE_MANTISSA_SIZE)        \
           & ((ONE << (-final_exp + 1)) - 1)) == (ONE << (-final_exp));       \
           /* result will be subnormal */                                     \
      if (final_exp <= 0 && -final_exp < (FP_TYPE_MANTISSA_SIZE + 2))         \
        subnormal_midpoint_detected = subnormal_shift;                        \
    }                                                                         \
                                                                              \
  /* Step 6: rescale float */                                                 \
  FP_UINT scale = rescale_float (&final_exp, &r_dq, is_inexact, &ev);         \
                                                                              \
  /* the rescale operation in the general case */                             \
  fp_type res_rn = dres.d =                                                   \
    __builtin_kvx_fmul (r_dq.d, scale.d, "." #mode1 SILENT_STRING);           \
                                                                              \
  if (!__builtin_strcmp (#mode1, "rn"))                                       \
    {                                                                         \
      /* Step 7: treat midpoint subnormal case, only in round to nearest      \
       * case (here in case even, if RNA needs another logic) */              \
      fp_type res_rd =                                                        \
        __builtin_kvx_fmul (r_dq.d, scale.d, ".rd" SILENT_STRING);            \
      fp_type res_ru =                                                        \
        __builtin_kvx_fmul (r_dq.d, scale.d, ".ru" SILENT_STRING);            \
                                                                              \
      if (subnormal_midpoint_detected && r2)                                  \
        {                                                                     \
          if (q_star != q_snd)                                                \
            dres.d = r2 > 0 ? res_rd : res_ru;                                \
          else                                                                \
            dres.d = r2 > 0 ? res_ru : res_rd;                                \
        }                                                                     \
    }                                                                         \
                                                                              \
  dres.u |= res_sign_full;                                                    \
                                                                              \
  return dres.d;                                                              \
}                                                                             \

#define define_handle_special_cases                                           \
static int                                                                    \
handle_special_cases (FP_UINT da, FP_UINT db, FP_UINT * dres, uint64_t * ev)  \
{                                                                             \
  int res_sign = da.f.s ^ db.f.s;                                             \
  UINT res_sign_full = ((UINT) res_sign) << (FP_TYPE_SIZE - 1);               \
                                                                              \
  if (IS_NAN (da.d) || IS_NAN (db.d) || (IS_INF (da) && IS_INF (db)))         \
    {                                                                         \
      if (!SILENT) {                                                          \
        if (IS_SNAN (da) | IS_SNAN (db))                                      \
          *ev |= INVALID_CST;                                                 \
      }                                                                       \
      dres->u = QNAN;                                                         \
      goto NR_div_ret;                                                        \
    }                                                                         \
  if (da.d == 0 || IS_INF (db))                                               \
    {                                                                         \
      if (db.d == 0)                                                          \
        {                                                                     \
          dres->u = QNAN | (ONE << (FP_TYPE_SIZE - 1));                       \
          if (!SILENT)                                                        \
            *ev |= INVALID_CST;                                               \
          goto NR_div_ret;                                                    \
        }                                                                     \
      /* return the right 0 */                                                \
      dres->u = res_sign_full;                                                \
      goto NR_div_ret;                                                        \
    }                                                                         \
  if (IS_INF (da) || db.d == 0)                                               \
    {                                                                         \
      if (!SILENT)                                                            \
        {                                                                     \
          if (db.d == 0)                                                      \
            *ev |= DIVZERO_CST;                                               \
          *ev |= INVALID_CST;                                                 \
        }                                                                     \
      dres->u = res_sign_full | FP_TYPE_INF;                                  \
      goto NR_div_ret;                                                        \
    }                                                                         \
  return 1;                                                                   \
                                                                              \
NR_div_ret:                                                                   \
  if (!SILENT)                                                                \
    OR_SET_EV (*ev);                                                          \
  return 0;                                                                   \
}                                                                             \

#define define_is_inexact_p                                                   \
static inline int                                                             \
is_inexact_p (fp_type q_snd, fp_type q_snd_rd)                                \
{                                                                             \
  /* To detect inexact without doubt, compare results in round up and round   \
   * down mode */                                                             \
  return (q_snd != q_snd_rd);                                                 \
}                                                                             \

#define define_iterate_approx                                                 \
static void                     /* Step 2 & 3 & 4 */                          \
iterate_approx (FP_UINT r_da, FP_UINT r_db, fp_type * q_star,                 \
                fp_type * y3)                                                 \
{                                                                             \
  if (!__builtin_strcmp (PROTECT(fp_type), "double"))                         \
    {                                                                         \
      /* Step 2: get seed for 1/b */                                          \
      FP_UINT y0 = {.d = __builtin_kvx_fsrecd (r_db.d, ".s")};                \
      /* For debug: */                                                        \
      /* if ((y0.u & 1) != 1)                                                 \
         printf ("Special case detected in fsrec for b: %lx, returned %lx.\n",\
         db.u, y0.u); */                                                      \
                                                                              \
      /* (don't) clear payload */                                             \
      /* y0.u &= 0xFFFFFFFFFFFFFE00ULL; */                                    \
                                                                              \
      /* Step 3: 2 goldsmidt + 1 newton-rapson iterations on 1/b. */          \
      fp_type e0 = __builtin_kvx_ffmsd (r_db.d, y0.d, 1.0, ".rn.s");          \
      fp_type y1 = __builtin_kvx_ffmad (y0.d, e0, y0.d, ".rn.s");             \
      fp_type e1 = __builtin_kvx_fmuld (e0, e0, ".rn.s");                     \
      fp_type y2 = __builtin_kvx_ffmad (y1, e1, y1, ".rn.s");                 \
                                                                              \
      fp_type r = __builtin_kvx_ffms (r_db.d, y2, 1.0, ".rn.s");              \
      *y3 = __builtin_kvx_ffma (r, y2, y2, ".rn.s");                          \
    }                                                                         \
  else if (!__builtin_strcmp (PROTECT(fp_type), "float"))                     \
    {                                                                         \
      /* Step 2: no need to get seed since it's now easy to get exact */      \
      /* Step 3: get exact 1/b. */                                            \
      *y3 = __builtin_kvx_frecw (r_db.d, ".rn.s");                            \
    }                                                                         \
                                                                              \
  /* Step 4: 2 markstein on a/b for correct result */                         \
  /* q = approx on a/b */                                                     \
  fp_type q = __builtin_kvx_fmul (r_da.d, *y3, ".rn.s");                      \
  fp_type r1 = __builtin_kvx_ffms (r_db.d, q, r_da.d, ".rn.s");               \
  *q_star = __builtin_kvx_ffma (r1, *y3, q, ".rn.s");                         \
}                                                                             \

#define define_rescale_float                                                  \
static FP_UINT               /* Step 6 */                                     \
rescale_float (int *final_exp, FP_UINT * r_dq, int is_inexact,                \
               uint64_t * ev)                                                 \
{                                                                             \
  FP_UINT scale = {.u = 0 };                                                  \
  UINT r_dq_exp = FP_TYPE_BIAS;                                               \
  int offset = 0;                                                             \
                                                                              \
  /* Pre-rescaling to be sure to have the right rounding and result */        \
  if (*final_exp <= -FP_TYPE_MANTISSA_SIZE)                                   \
    offset = 10;                                                              \
  if (*final_exp >= FP_TYPE_E_MAX - 1)                                        \
    offset = -10;                                                             \
                                                                              \
  r_dq_exp = FP_TYPE_BIAS - offset;                                           \
  *final_exp += offset;                                                       \
  r_dq->f.e = r_dq_exp;                                                       \
                                                                              \
  scale.f.e = *final_exp;                                                     \
  if (*final_exp <= 0)                                                        \
    {                           /* f is subnormal */                          \
      if (!SILENT)                                                            \
        {                                                                     \
          /* If inexact flag is raised, then mult by subnormal should raise   \
           * underflow This flag can be an issue when trailing zeros make the \
           * last mult an exact underflow */                                  \
          if (is_inexact)                                                     \
            *ev |= UNDERFLOW_CST;                                             \
        }                                                                     \
      /* Use smallest subnormal (or omega) instead of 0 (or INF) for round    \
       * up/down/to zero roundings */                                         \
      if (-*final_exp > FP_TYPE_MANTISSA_SIZE)                                \
        scale.u = 1;                                                          \
      else                                                                    \
        scale.u =                                                             \
          (scale.f.m | (ONE << FP_TYPE_MANTISSA_SIZE)) >> (-*final_exp + 1);  \
    }                                                                         \
  if (*final_exp > FP_TYPE_E_MAX)                                             \
    scale.u = FP_TYPE_OMEGA;                                                  \
                                                                              \
  return scale;                                                               \
}                                                                             \
// #define SPECIALIZE

#endif //_DIVXF3_H_
