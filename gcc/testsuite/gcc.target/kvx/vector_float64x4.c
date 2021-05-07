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

float64x4_t __attribute__ ((noinline))
kvx_float64x4_incr(float64x4_t a)
{
    return a + 1.0;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_addto(float64x4_t a)
{
    float64x4_t b = { 1.0, 2.0, 3.0, 4.0 };
    return a + b;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_subfrom(float64x4_t a)
{
    float64x4_t b = { 1.0, 2.0, 3.0, 4.0 };
    return b - a;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_addwhere(float64x4_t a, float64x4_t __bypass *b)
{
    return a + *b;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_add(float64x4_t a, float64x4_t b)
{
    return a + b;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_adds(float64x4_t a, float64_t b)
{
    return a + b;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fadd(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fadddq(a, b, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_sub(float64x4_t a, float64x4_t b)
{
    return a - b;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fsbf(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fsbfdq(a, b, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_mul(float64x4_t a, float64x4_t b)
{
    return a * b;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fmul(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fmuldq(a, b, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fma(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return fma(a, b, c);
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_ffma(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmadq(a, b, c, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fms1(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return fma(-a, b, c);
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fms2(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return fma(a, -b, c);
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_ffms(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmsdq(a, b, c, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fmin(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fmindq(a, b);
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fmax(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fmaxdq(a, b);
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_neg(float64x4_t a)
{
    return -a;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fneg(float64x4_t a)
{
    return __builtin_kvx_fnegdq(a);
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fabs(float64x4_t a)
{
    return __builtin_kvx_fabsdq(a);
}
void __attribute__ ((noinline))
kvx_float64x4_axpyb(int n, float64_t a, float64_t b, float64x4_t x[], float64x4_t y[], float64x4_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float64x4_t __attribute ((noinline))
kvx_float64x4_fmins(float64x4_t a, float64_t b)
{
    return __builtin_kvx_fmindq(a, b-(float64x4_t){});
}
float64x4_t __attribute ((noinline))
kvx_float64x4_fmaxs(float64x4_t a, float64_t b)
{
    return __builtin_kvx_fmaxdq(a, b-(float64x4_t){});
}
float64x4_t __attribute ((noinline))
kvx_float64x4_copysign(float64x4_t a, float64x4_t b)
{
  return __builtin_kvx_copysigndq(a, b);
}
int64x4_t __attribute__ ((noinline))
kvx_float64x4_eq(float64x4_t a, float64x4_t b)
{
    return a == b;
}
int64x4_t __attribute__ ((noinline))
kvx_float64x4_gt(float64x4_t a, float64x4_t b)
{
    return a > b;
}
int64x4_t __attribute__ ((noinline))
kvx_float64x4_nez(float64x4_t a)
{
    return a != 0.0;
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_faddcdcp(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fadddq(a, b, ".c.rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fsbfcdcp(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fsbfdq(a, b, ".c.rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fmuldcp(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fmuldcp(a, b, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fmulcdcp(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fmuldcp(a, b, ".c.rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_ffmadcp(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmadcp(a, b, c, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_ffmcadcp(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmadcp(a, b, c, ".c.rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_ffmsdcp(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmsdcp(a, b, c, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_ffmcsdcp(float64x4_t a, float64x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmsdcp(a, b, c, ".c.rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fconjdcp(float64x4_t a)
{
    return __builtin_kvx_fconjdcp(a);
}
float64x4_t kvx_float64x4_floatdq(int64x4_t a) {
    return __builtin_kvx_floatdq(a, 20, ".rn.s");
}
float64x4_t kvx_float64x4_floatudq(int64x4_t a) {
    return __builtin_kvx_floatudq(a, 21, ".rn.s");
}
int64x4_t kvx_float64x4_fixeddq(float64x4_t a) {
    return __builtin_kvx_fixeddq(a, 22, ".rn.s");
}
int64x4_t kvx_float64x4_fixedudq(float64x4_t a) {
    return __builtin_kvx_fixedudq(a, 23, ".rn.s");
}

float64x4_t __attribute__ ((noinline))
kvx_float64x4_fcdivdq(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fcdivdq(a, b, ".s");
}
float64x4_t __attribute__ ((noinline))
kvx_float64x4_fsdivdq(float64x4_t a, float64x4_t b)
{
    return __builtin_kvx_fsdivdq(a, b, ".s");
}
float64x4_t __attribute ((noinline))
kvx_float64x4_select(float64x4_t a, float64x4_t b, int64x4_t c)
{
    return __builtin_kvx_selectfdq(a, b, c, 0);
}
float64x4_t __attribute ((noinline))
kvx_float64x4_shift(float64x4_t a, float64_t b)
{
    return __builtin_kvx_shiftfdq(a, 1, b);
}
