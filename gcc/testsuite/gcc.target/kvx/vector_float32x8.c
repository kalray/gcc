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

float32x8_t __attribute__ ((noinline))
kvx_float32x8_incr(float32x8_t a)
{
    return a + 1.0;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_addto(float32x8_t a)
{
    float32x8_t b = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
    return a + b;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_subfrom(float32x8_t a)
{
    float32x8_t b = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0 };
    return b - a;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_addwhere(float32x8_t a, float32x8_t __bypass *b)
{
    return a + *b;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_add(float32x8_t a, float32x8_t b)
{
    return a + b;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_adds(float32x8_t a, float32_t b)
{
    return a + b;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fadd(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_faddwo(a, b, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_sub(float32x8_t a, float32x8_t b)
{
    return a - b;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fsbf(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fsbfwo(a, b, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_mul(float32x8_t a, float32x8_t b)
{
    return a * b;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fmul(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fmulwo(a, b, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fma(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return fma(a, b, c);
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_ffma(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmawo(a, b, c, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fms1(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return fma(-a, b, c);
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fms2(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return fma(a, -b, c);
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_ffms(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmswo(a, b, c, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fmin(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fminwo(a, b);
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fmax(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fmaxwo(a, b);
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_neg(float32x8_t a)
{
    return -a;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fneg(float32x8_t a)
{
    return __builtin_kvx_fnegwo(a);
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fabs(float32x8_t a)
{
    return __builtin_kvx_fabswo(a);
}
void __attribute__ ((noinline))
kvx_float32x8_axpyb(int n, float32_t a, float32_t b, float32x8_t x[], float32x8_t y[], float32x8_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float32x8_t __attribute ((noinline))
kvx_float32x8_fmins(float32x8_t a, float32_t b)
{
    return __builtin_kvx_fminwo(a, b-(float32x8_t){});
}
float32x8_t __attribute ((noinline))
kvx_float32x8_fmaxs(float32x8_t a, float32_t b)
{
    return __builtin_kvx_fmaxwo(a, b-(float32x8_t){});
}
float32x8_t __attribute ((noinline))
kvx_float32x8_copysign(float32x8_t a, float32x8_t b)
{
  return __builtin_kvx_copysignwo(a, b);
}
int32x8_t __attribute__ ((noinline))
kvx_float32x8_eq(float32x8_t a, float32x8_t b)
{
    return a == b;
}
int32x8_t __attribute__ ((noinline))
kvx_float32x8_gt(float32x8_t a, float32x8_t b)
{
    return a > b;
}
int32x8_t __attribute__ ((noinline))
kvx_float32x8_nez(float32x8_t a)
{
    return a != 0.0;
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_faddcwcq(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_faddwo(a, b, ".c.rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fsbfcwcq(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fsbfwo(a, b, ".c.rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fmulwcq(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fmulwcq(a, b, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fmulcwcq(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fmulwcq(a, b, ".c.rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_ffmawcq(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmawcq(a, b, c, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_ffmcawcq(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmawcq(a, b, c, ".c.rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_ffmswcq(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmswcq(a, b, c, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_ffmcswcq(float32x8_t a, float32x8_t b, float32x8_t c)
{
    return __builtin_kvx_ffmswcq(a, b, c, ".c.rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fconjwcq(float32x8_t a)
{
    return __builtin_kvx_fconjwcq(a);
}
float32x8_t kvx_float32x8_floatwo(int32x8_t a) {
    return __builtin_kvx_floatwo(a, 20, ".rn.s");
}
float32x8_t kvx_float32x8_floatuwo(int32x8_t a) {
    return __builtin_kvx_floatuwo(a, 21, ".rn.s");
}
int32x8_t kvx_float32x8_fixedwo(float32x8_t a) {
    return __builtin_kvx_fixedwo(a, 22, ".rn.s");
}
int32x8_t kvx_float32x8_fixeduwo(float32x8_t a) {
    return __builtin_kvx_fixeduwo(a, 23, ".rn.s");
}

float32x8_t __attribute__ ((noinline))
kvx_float32x8_frecwo(float32x8_t a)
{
    return __builtin_kvx_frecwo(a, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_frsrwo(float32x8_t a)
{
    return __builtin_kvx_frsrwo(a, ".rn");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fcdivwo(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fcdivwo(a, b, ".s");
}
float32x8_t __attribute__ ((noinline))
kvx_float32x8_fsdivwo(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_fsdivwo(a, b, ".s");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x8_ffdmawq(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_ffdmawq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x8_ffdmswq(float32x8_t a, float32x8_t b)
{
    return __builtin_kvx_ffdmswq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x8_ffdmdawq(float32x8_t a, float32x8_t b, float32x4_t c)
{
    return __builtin_kvx_ffdmdawq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x8_ffdmsawq(float32x8_t a, float32x8_t b, float32x4_t c)
{
    return __builtin_kvx_ffdmsawq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x8_ffdmdswq(float32x8_t a, float32x8_t b, float32x4_t c)
{
    return __builtin_kvx_ffdmdswq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x8_ffdmaswq(float32x8_t a, float32x8_t b, float32x4_t c)
{
    return __builtin_kvx_ffdmaswq(a, b, c, ".rn");
}
  
float32x8_t __attribute ((noinline))
kvx_float32x8_select(float32x8_t a, float32x8_t b, int32x8_t c)
{
    return __builtin_kvx_selectfwo(a, b, c, 0);
}
float32x8_t __attribute ((noinline))
kvx_float32x8_shift(float32x8_t a, float32_t b)
{
    return __builtin_kvx_shiftfwo(a, 1, b);
}
