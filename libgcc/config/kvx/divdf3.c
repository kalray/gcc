#include "divxf.h"

#define fp_type              double
#define FP_TYPE              DOUBLE
#define fp_suffix            d
#define fp_mode              df
#define UINT                 uint64_t
#define ONE                  1ULL
#define clz                  __builtin_clzll

#define DOUBLE_BIAS          (1023)
#define DOUBLE_E_MAX         (2046)
#define DOUBLE_MANTISSA_SIZE (52)
#define DOUBLE_SIZE          (64)
#define DOUBLE_QNAN          (0x7FF8000000000000ULL)
#define DOUBLE_INF           (0x7FF0000000000000ULL)
#define DOUBLE_OMEGA         (0x7FEFFFFFFFFFFFFFULL)

#define IS_DOUBLE_SNAN(X)                                                      \
  (IS_NAN (X.d) && (X.u & (1ULL << (DOUBLE_MANTISSA_SIZE - 1))) == 0)
#define IS_DOUBLE_INF(X) (X.u << 1 == DOUBLE_INF << 1)

#include "divxf_epilogue.c"
