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

float16x8_t __attribute__ ((noinline))
kvx_float16x8_incr(float16x8_t a)
{
    return a + 1.0;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_addto(float16x8_t a)
{
    float16x8_t b = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
    return a + b;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_subfrom(float16x8_t a)
{
    float16x8_t b = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
    return b - a;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_addwhere(float16x8_t a, float16x8_t __bypass *b)
{
    return a + *b;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_add(float16x8_t a, float16x8_t b)
{
    return a + b;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_adds(float16x8_t a, float16_t b)
{
    return a + b;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fadd(float16x8_t a, float16x8_t b)
{
    return __builtin_kvx_faddho(a, b, ".rn");
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_sub(float16x8_t a, float16x8_t b)
{
    return a - b;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fsbf(float16x8_t a, float16x8_t b)
{
    return __builtin_kvx_fsbfho(a, b, ".rn");
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_mul(float16x8_t a, float16x8_t b)
{
    return a * b;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fmul(float16x8_t a, float16x8_t b)
{
    return __builtin_kvx_fmulho(a, b, ".rn");
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fma(float16x8_t a, float16x8_t b, float16x8_t c)
{
    return fma(a, b, c);
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_ffma(float16x8_t a, float16x8_t b, float16x8_t c)
{
    return __builtin_kvx_ffmaho(a, b, c, ".rn");
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fms1(float16x8_t a, float16x8_t b, float16x8_t c)
{
    return fma(-a, b, c);
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fms2(float16x8_t a, float16x8_t b, float16x8_t c)
{
    return fma(a, -b, c);
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_ffms(float16x8_t a, float16x8_t b, float16x8_t c)
{
    return __builtin_kvx_ffmsho(a, b, c, ".rn");
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fmin(float16x8_t a, float16x8_t b)
{
    return __builtin_kvx_fminho(a, b);
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fmax(float16x8_t a, float16x8_t b)
{
    return __builtin_kvx_fmaxho(a, b);
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_neg(float16x8_t a)
{
    return -a;
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fneg(float16x8_t a)
{
    return __builtin_kvx_fnegho(a);
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fabs(float16x8_t a)
{
    return __builtin_kvx_fabsho(a);
}
void __attribute__ ((noinline))
kvx_float16x8_axpyb(int n, float16_t a, float16_t b, float16x8_t x[], float16x8_t y[], float16x8_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float16x8_t __attribute ((noinline))
kvx_float16x8_fmins(float16x8_t a, float16_t b)
{
    return __builtin_kvx_fminho(a, b-(float16x8_t){});
}
float16x8_t __attribute ((noinline))
kvx_float16x8_fmaxs(float16x8_t a, float16_t b)
{
    return __builtin_kvx_fmaxho(a, b-(float16x8_t){});
}
float16x8_t __attribute ((noinline))
kvx_float16x8_copysign(float16x8_t a, float16x8_t b)
{
  return __builtin_kvx_copysignho(a, b);
}
int16x8_t __attribute__ ((noinline))
kvx_float16x8_eq(float16x8_t a, float16x8_t b)
{
    return a == b;
}
int16x8_t __attribute__ ((noinline))
kvx_float16x8_gt(float16x8_t a, float16x8_t b)
{
    return a > b;
}
int16x8_t __attribute__ ((noinline))
kvx_float16x8_nez(float16x8_t a)
{
    return a != 0.0;
}
float32x8_t __attribute__ ((noinline))
kvx_float16x8_fmulhwo(float16x8_t a, float16x8_t b)
{
    return __builtin_kvx_fmulhwo(a, b, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float16x8_ffmahwo(float16x8_t a, float16x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmahwo(a, b, c, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float16x8_ffmshwo(float16x8_t a, float16x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmshwo(a, b, c, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float16x8_fwidenhwo(float16x8_t a)
{
    return __builtin_kvx_fwidenhwo(a, ".s");
}
float16x8_t __attribute__ ((noinline))
kvx_float16x8_fnarrowwho(float32x8_t a)
{
    return __builtin_kvx_fnarrowwho(a, ".rn.s");
}
float16x8_t __attribute ((noinline))
kvx_float16x8_select(float16x8_t a, float16x8_t b, int16x8_t c)
{
    return __builtin_kvx_selectfho(a, b, c, 0);
}
float16x8_t __attribute ((noinline))
kvx_float16x8_shift(float16x8_t a, float16_t b)
{
    return __builtin_kvx_shiftfho(a, 1, b);
}
float16x8_t __attribute ((noinline))
kvx_float16x8_consfho(float16x4_t a, float16x4_t b) {
    return __builtin_kvx_consfho(a, b);
}
float16x8_t __attribute ((noinline))
kvx_float16x8_sconsfho(float16x4_t a, float16x4_t b) {
    return __builtin_kvx_consfho(b, a);
}
