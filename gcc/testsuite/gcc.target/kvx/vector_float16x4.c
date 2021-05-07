/* { dg-do compile } */
/* { dg-options "-O2 -ffp-contract=fast" } */

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef _Float16 float16_t;
typedef float float32_t;
typedef double float64_t;
#define fma(a, b, c) ((a) * (b) + c)

typedef int8_t int8x8_t __attribute((vector_size(8*sizeof(int8_t))));
typedef uint8_t uint8x8_t __attribute((vector_size(8*sizeof(int8_t))));
typedef int8_t int8x16_t __attribute((vector_size(16*sizeof(int8_t))));
typedef uint8_t uint8x16_t __attribute((vector_size(16*sizeof(int8_t))));
typedef int8_t int8x32_t __attribute((vector_size(32*sizeof(int8_t))));
typedef uint8_t uint8x32_t __attribute((vector_size(32*sizeof(int8_t))));

typedef int16_t int16x4_t __attribute((vector_size(4*sizeof(int16_t))));
typedef uint16_t uint16x4_t __attribute((vector_size(4*sizeof(int16_t))));
typedef int16_t int16x8_t __attribute((vector_size(8*sizeof(int16_t))));
typedef uint16_t uint16x8_t __attribute((vector_size(8*sizeof(int16_t))));
typedef int16_t int16x16_t __attribute((vector_size(16*sizeof(int16_t))));
typedef uint16_t uint16x16_t __attribute((vector_size(16*sizeof(int16_t))));

typedef int32_t int32x2_t __attribute((vector_size(2*sizeof(int32_t))));
typedef uint32_t uint32x2_t __attribute((vector_size(2*sizeof(int32_t))));
typedef int32_t int32x4_t __attribute((vector_size(4*sizeof(int32_t))));
typedef uint32_t uint32x4_t __attribute((vector_size(4*sizeof(int32_t))));
typedef int32_t int32x8_t __attribute((vector_size(8*sizeof(int32_t))));
typedef uint32_t uint32x8_t __attribute((vector_size(8*sizeof(int32_t))));

typedef int64_t int64x2_t __attribute((vector_size(2*sizeof(int64_t))));
typedef uint64_t uint64x2_t __attribute((vector_size(2*sizeof(int64_t))));
typedef int64_t int64x4_t __attribute((vector_size(4*sizeof(int64_t))));
typedef uint64_t uint64x4_t __attribute((vector_size(4*sizeof(int64_t))));

typedef float16_t float16x4_t __attribute__((vector_size(4*sizeof(float16_t))));
typedef float16_t float16x8_t __attribute__((vector_size(8*sizeof(float16_t))));
typedef float16_t float16x16_t __attribute__((vector_size(16*sizeof(float16_t))));

typedef float32_t float32x2_t __attribute__((vector_size(2*sizeof(float32_t))));
typedef float32_t float32x4_t __attribute__((vector_size(4*sizeof(float32_t))));
typedef float32_t float32x8_t __attribute__((vector_size(8*sizeof(float32_t))));

typedef float64_t float64x2_t __attribute__((vector_size(2*sizeof(float64_t))));
typedef float64_t float64x4_t __attribute__((vector_size(4*sizeof(float64_t))));

float16x4_t __attribute__ ((noinline))
kvx_float16x4_incr(float16x4_t a)
{
    return a + 1.0;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_addto(float16x4_t a)
{
    float16x4_t b = { 1.0, 2.0, 3.0, 4.0 };
    return a + b;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_subfrom(float16x4_t a)
{
    float16x4_t b = { 1.0, 2.0, 3.0, 4.0 };
    return b - a;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_addwhere(float16x4_t a, float16x4_t __bypass *b)
{
    return a + *b;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_add(float16x4_t a, float16x4_t b)
{
    return a + b;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_adds(float16x4_t a, float16_t b)
{
    return a + b;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fadd(float16x4_t a, float16x4_t b)
{
    return __builtin_kvx_faddhq(a, b, ".rn");
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_sub(float16x4_t a, float16x4_t b)
{
    return a - b;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fsbf(float16x4_t a, float16x4_t b)
{
    return __builtin_kvx_fsbfhq(a, b, ".rn");
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_mul(float16x4_t a, float16x4_t b)
{
    return a * b;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fmul(float16x4_t a, float16x4_t b)
{
    return __builtin_kvx_fmulhq(a, b, ".rn");
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fma(float16x4_t a, float16x4_t b, float16x4_t c)
{
    return fma(a, b, c);
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_ffma(float16x4_t a, float16x4_t b, float16x4_t c)
{
    return __builtin_kvx_ffmahq(a, b, c, ".rn");
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fms1(float16x4_t a, float16x4_t b, float16x4_t c)
{
    return fma(-a, b, c);
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fms2(float16x4_t a, float16x4_t b, float16x4_t c)
{
    return fma(a, -b, c);
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_ffms(float16x4_t a, float16x4_t b, float16x4_t c)
{
    return __builtin_kvx_ffmshq(a, b, c, ".rn");
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fmin(float16x4_t a, float16x4_t b)
{
    return __builtin_kvx_fminhq(a, b);
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fmax(float16x4_t a, float16x4_t b)
{
    return __builtin_kvx_fmaxhq(a, b);
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_neg(float16x4_t a)
{
    return -a;
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fneg(float16x4_t a)
{
    return __builtin_kvx_fneghq(a);
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fabs(float16x4_t a)
{
    return __builtin_kvx_fabshq(a);
}
void __attribute__ ((noinline))
kvx_float16x4_axpyb(int n, float16_t a, float16_t b, float16x4_t x[], float16x4_t y[], float16x4_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float16x4_t __attribute ((noinline))
kvx_float16x4_fmins(float16x4_t a, float16_t b)
{
    return __builtin_kvx_fminhq(a, b-(float16x4_t){});
}
float16x4_t __attribute ((noinline))
kvx_float16x4_fmaxs(float16x4_t a, float16_t b)
{
    return __builtin_kvx_fmaxhq(a, b-(float16x4_t){});
}
float16x4_t __attribute ((noinline))
kvx_float16x4_copysign(float16x4_t a, float16x4_t b)
{
  return __builtin_kvx_copysignhq(a, b);
}
int16x4_t __attribute__ ((noinline))
kvx_float16x4_eq(float16x4_t a, float16x4_t b)
{
    return a == b;
}
int16x4_t __attribute__ ((noinline))
kvx_float16x4_gt(float16x4_t a, float16x4_t b)
{
    return a > b;
}
int16x4_t __attribute__ ((noinline))
kvx_float16x4_nez(float16x4_t a)
{
    return a != 0.0;
}
float32x4_t __attribute__ ((noinline))
kvx_float16x4_fmulhwq(float16x4_t a, float16x4_t b)
{
    return __builtin_kvx_fmulhwq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float16x4_ffmahwq(float16x4_t a, float16x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmahwq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float16x4_ffmshwq(float16x4_t a, float16x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmshwq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float16x4_fwidenhwq(float16x4_t a)
{
    return __builtin_kvx_fwidenhwq(a, ".s");
}
float16x4_t __attribute__ ((noinline))
kvx_float16x4_fnarrowwhq(float32x4_t a)
{
    return __builtin_kvx_fnarrowwhq(a, ".rn.s");
}
float16x4_t __attribute ((noinline))
kvx_float16x4_select(float16x4_t a, float16x4_t b, int16x4_t c)
{
    return __builtin_kvx_selectfhq(a, b, c, 0);
}
float16x4_t __attribute ((noinline))
kvx_float16x4_shift(float16x4_t a, float16_t b)
{
    return __builtin_kvx_shiftfhq(a, 1, b);
}
