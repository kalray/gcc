#define _FP_W_TYPE_SIZE 32
#define _FP_W_TYPE unsigned int
#define _FP_WS_TYPE signed int
#define _FP_I_TYPE int

/* The type of the result of a floating point comparison.  This must
   match `__libgcc_cmp_return__' in GCC for the target.  */
typedef int __gcc_CMPtype __attribute__ ((mode (__libgcc_cmp_return__)));
#define CMPtype __gcc_CMPtype

#define _FP_MUL_MEAT_S(R, X, Y)                                                \
  _FP_MUL_MEAT_1_wide (_FP_WFRACBITS_S, R, X, Y, umul_ppmm)
#define _FP_MUL_MEAT_D(R, X, Y)                                                \
  _FP_MUL_MEAT_2_wide (_FP_WFRACBITS_D, R, X, Y, umul_ppmm)

#define _FP_DIV_MEAT_S(R, X, Y) _FP_DIV_MEAT_1_loop (S, R, X, Y)
#define _FP_DIV_MEAT_D(R, X, Y) _FP_DIV_MEAT_2_udiv (D, R, X, Y)

#define _FP_NANFRAC_S _FP_QNANBIT_S
#define _FP_NANFRAC_D _FP_QNANBIT_D, 0
#define _FP_NANSIGN_S 1
#define _FP_NANSIGN_D 1

#define _FP_KEEPNANFRACP 1
#define _FP_QNANNEGATEDP 0

/* We always choose the first NaN */
#define _FP_CHOOSENAN(fs, wc, R, X, Y, OP)                                     \
  do                                                                           \
    {                                                                          \
      R##_s = X##_s;                                                           \
      _FP_FRAC_COPY_##wc (R, X);                                               \
      R##_c = FP_CLS_NAN;                                                      \
    }                                                                          \
  while (0)

/* Compute Status ($cs) register contains the following bit-fields for
   floating-point execption flags.

   Bit-field Condition of the IEEE 754 binary floating-point standard
   --------- --------------------------------------------------------
   IO        Invalid Operation
   DZ        Divide by Zero
   OV        Overflow
   UN        Underflow
   IN        Inexact
   XIO       Invalid Operation (coprocessor)
   XDZ       Divide by Zero (coprocessor)
   XOV       Overflow (coprocessor)
   XUN       Underflow (coprocessor)
   XIN       Inexact (coprocessor) */

#define _FE_INVALID 0x02
#define _FE_DIVBYZERO 0x04
#define _FE_OVERFLOW 0x08
#define _FE_UNDERFLOW 0x10
#define _FE_INEXACT 0x20

#define _FE_X_INVALID 0x0200
#define _FE_X_DIVBYZERO 0x0400
#define _FE_X_OVERFLOW 0x0800
#define _FE_X_UNDERFLOW 0x1000
#define _FE_X_INEXACT 0x2000

#define FP_EX_INVALID (_FE_INVALID | _FE_X_INVALID)
#define FP_EX_DIVZERO (_FE_DIVBYZERO | _FE_X_DIVBYZERO)
#define FP_EX_OVERFLOW (_FE_OVERFLOW | _FE_X_OVERFLOW)
#define FP_EX_UNDERFLOW (_FE_UNDERFLOW | _FE_X_UNDERFLOW)
#define FP_EX_INEXACT (_FE_INEXACT | _FE_X_INEXACT)

#define FP_EX_ALL                                                              \
  (FP_EX_INVALID | FP_EX_DIVZERO | FP_EX_OVERFLOW | FP_EX_UNDERFLOW            \
   | FP_EX_INEXACT)

#define _FP_TININESS_AFTER_ROUNDING 0

#define FP_HANDLE_EXCEPTIONS                                                   \
  do                                                                           \
    {                                                                          \
      asm volatile("wfxl $cs, %0" : : "r"((long long) _fex << 32));            \
    }                                                                          \
  while (0);

/* Compute Status ($cs) register contains the following bit-fields for
   floating-point rounding modes.

   Following table describes both the RM and XRM (coproc) bit-fields.

   Value Rounding Mode of the IEEE 754 binary floating-point standard
   ----- ------------------------------------------------------------
   0b00  to nearest even
   0b01  toward +inf
   0b10  toward -inf
   0b11  toward zero */

#define _FP_TONEAREST 0
#define _FP_UPWARD 1
#define _FP_DOWNWARD 2
#define _FP_TOWARDZERO 3

#define _FP_X_TONEAREST 0
#define _FP_X_UPWARD 1
#define _FP_X_DOWNWARD 2
#define _FP_X_TOWARDZERO 3

#define FP_RND_NEAREST ((_FP_TONEAREST << 16) | (_FP_X_TONEAREST << 20))
#define FP_RND_PINF ((_FP_UPWARD << 16) | (_FP_X_UPWARD << 20))
#define FP_RND_MINF ((_FP_DOWNWARD << 16) | (_FP_X_DOWNWARD << 20))
#define FP_RND_ZERO ((_FP_TOWARDZERO << 16) | (_FP_X_TOWARDZERO << 20))

#define FP_RND_MASK FP_RND_ZERO

#define _FP_DECL_EX unsigned int _rm __attribute__ ((unused)) = FP_RND_NEAREST

#define FP_INIT_ROUNDMODE                                                      \
  do                                                                           \
    {                                                                          \
      asm volatile("get %0 = $cs" : "=r"(_rm));                                \
    }                                                                          \
  while (0)

#define FP_ROUNDMODE (_rm & FP_RND_MASK)

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321

#define __BYTE_ORDER __LITTLE_ENDIAN

/* Define ALIASNAME as a strong alias for NAME.  */
#define strong_alias(name, aliasname) _strong_alias (name, aliasname)
#define _strong_alias(name, aliasname)                                         \
  extern __typeof (name) aliasname __attribute__ ((alias (#name)));
