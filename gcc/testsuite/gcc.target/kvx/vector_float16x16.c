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

float16x16_t __attribute__ ((noinline))
kvx_float16x16_incr(float16x16_t a)
{
    return a + 1.0;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_addto(float16x16_t a)
{
    float16x16_t b = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };
    return a + b;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_subfrom(float16x16_t a)
{
    float16x16_t b = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0 };
    return b - a;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_addwhere(float16x16_t a, float16x16_t __bypass *b)
{
    return a + *b;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_add(float16x16_t a, float16x16_t b)
{
    return a + b;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_adds(float16x16_t a, float16_t b)
{
    return a + b;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fadd(float16x16_t a, float16x16_t b)
{
    return __builtin_kvx_faddhx(a, b, ".rn");
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_sub(float16x16_t a, float16x16_t b)
{
    return a - b;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fsbf(float16x16_t a, float16x16_t b)
{
    return __builtin_kvx_fsbfhx(a, b, ".rn");
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_mul(float16x16_t a, float16x16_t b)
{
    return a * b;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fmul(float16x16_t a, float16x16_t b)
{
    return __builtin_kvx_fmulhx(a, b, ".rn");
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fma(float16x16_t a, float16x16_t b, float16x16_t c)
{
    return fma(a, b, c);
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_ffma(float16x16_t a, float16x16_t b, float16x16_t c)
{
    return __builtin_kvx_ffmahx(a, b, c, ".rn");
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fms1(float16x16_t a, float16x16_t b, float16x16_t c)
{
    return fma(-a, b, c);
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fms2(float16x16_t a, float16x16_t b, float16x16_t c)
{
    return fma(a, -b, c);
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_ffms(float16x16_t a, float16x16_t b, float16x16_t c)
{
    return __builtin_kvx_ffmshx(a, b, c, ".rn");
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fmin(float16x16_t a, float16x16_t b)
{
    return __builtin_kvx_fminhx(a, b);
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fmax(float16x16_t a, float16x16_t b)
{
    return __builtin_kvx_fmaxhx(a, b);
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_neg(float16x16_t a)
{
    return -a;
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fneg(float16x16_t a)
{
    return __builtin_kvx_fneghx(a);
}
float16x16_t __attribute__ ((noinline))
kvx_float16x16_fabs(float16x16_t a)
{
    return __builtin_kvx_fabshx(a);
}
void __attribute__ ((noinline))
kvx_float16x16_axpyb(int n, float16_t a, float16_t b, float16x16_t x[], float16x16_t y[], float16x16_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float16x16_t __attribute ((noinline))
kvx_float16x16_fmins(float16x16_t a, float16_t b)
{
    return __builtin_kvx_fminhx(a, b-(float16x16_t){});
}
float16x16_t __attribute ((noinline))
kvx_float16x16_fmaxs(float16x16_t a, float16_t b)
{
    return __builtin_kvx_fmaxhx(a, b-(float16x16_t){});
}
float16x16_t __attribute ((noinline))
kvx_float16x16_copysign(float16x16_t a, float16x16_t b)
{
  return __builtin_kvx_copysignhx(a, b);
}
int16x16_t __attribute__ ((noinline))
kvx_float16x16_eq(float16x16_t a, float16x16_t b)
{
    return a == b;
}
int16x16_t __attribute__ ((noinline))
kvx_float16x16_gt(float16x16_t a, float16x16_t b)
{
    return a > b;
}
int16x16_t __attribute__ ((noinline))
kvx_float16x16_nez(float16x16_t a)
{
    return a != 0.0;
}
float16x16_t __attribute ((noinline))
kvx_float16x16_select(float16x16_t a, float16x16_t b, int16x16_t c)
{
    return __builtin_kvx_selectfhx(a, b, c, 0);
}
float16x16_t __attribute ((noinline))
kvx_float16x16_shift(float16x16_t a, float16_t b)
{
    return __builtin_kvx_shiftfhx(a, 1, b);
}
