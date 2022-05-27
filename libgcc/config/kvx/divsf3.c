#include "divxf.h"

#define fp_type             float
#define FP_TYPE             FLOAT
#define fp_suffix           w
#define fp_mode             sf
#define UINT                uint32_t
#define ONE                 1U
#define clz                 __builtin_clz

#define FLOAT_BIAS          (127)
#define FLOAT_E_MAX         (254)
#define FLOAT_MANTISSA_SIZE (23)
#define FLOAT_SIZE          (32)
#define FLOAT_QNAN          (0x7FC00000U)
#define FLOAT_INF           (0x7F800000U)
#define FLOAT_OMEGA         (0x7F7FFFFFU)

#define IS_FLOAT_SNAN(X)                                                       \
  (IS_NAN (X.d) && (X.u & (1ULL << (FLOAT_MANTISSA_SIZE - 1))) == 0)
#define IS_FLOAT_INF(X) (X.u << 1 == FLOAT_INF << 1)

#include "divxf_epilogue.c"
