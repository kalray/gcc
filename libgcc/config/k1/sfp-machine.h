/*
 * FIXME AUTO: part of HAL duplicated here for coolidge
 */
#ifdef __CLUSTER_OS__
#include "config/k1/cos/sfp-machine.h"
#else
#include "config/k1/elf/sfp-machine.h"
#endif

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

/* We always choose the first NaN */
#define _FP_CHOOSENAN(fs, wc, R, X, Y, OP)                                     \
  do                                                                           \
    {                                                                          \
      R##_s = X##_s;                                                           \
      _FP_FRAC_COPY_##wc (R, X);                                               \
      R##_c = FP_CLS_NAN;                                                      \
    }                                                                          \
  while (0)

#define _FP_DECL_EX unsigned int _rm __attribute__ ((unused)) = FP_RND_NEAREST

#define FP_ROUNDMODE (_rm)

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321

#define __BYTE_ORDER __LITTLE_ENDIAN

/* Define ALIASNAME as a strong alias for NAME.  */
#define strong_alias(name, aliasname) _strong_alias (name, aliasname)
#define _strong_alias(name, aliasname)                                         \
  extern __typeof (name) aliasname __attribute__ ((alias (#name)));

#define _FP_QNANNEGATEDP 0

#define _FP_TININESS_AFTER_ROUNDING 0
