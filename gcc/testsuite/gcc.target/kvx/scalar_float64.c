/* { dg-do compile } */
/* { dg-options "-O2 -ffp-contract=fast" } */

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long int64_t;
typedef __int128 int128_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef unsigned __int128 uint128_t;
typedef _Float16 float16_t;
typedef float float32_t;
typedef double float64_t;
#define fma(a, b, c) ((a) * (b) + c)

float64_t __attribute__ ((noinline))
kvx_float64_incr(float64_t a)
{
    return a + 1.0;
}
float64_t __attribute__ ((noinline))
kvx_float64_addto(float64_t a)
{
    float64_t b = 1.0f;
    return a + b;
}
float64_t __attribute__ ((noinline))
kvx_float64_subfrom(float64_t a)
{
    float64_t b = 1.0f;
    return b - a;
}
float64_t __attribute__ ((noinline))
kvx_float64_addwhere(float64_t a, float64_t __bypass *b)
{
    return a + *b;
}
float64_t __attribute__ ((noinline))
kvx_float64_add(float64_t a, float64_t b)
{
    return a + b;
}
float64_t __attribute__ ((noinline))
kvx_float64_adds(float64_t a, float64_t b)
{
    return a + b;
}
float64_t __attribute__ ((noinline))
kvx_float64_fadd(float64_t a, float64_t b)
{
    return __builtin_kvx_faddd(a, b, ".rn");
}
float64_t __attribute__ ((noinline))
kvx_float64_sub(float64_t a, float64_t b)
{
    return a - b;
}
float64_t __attribute__ ((noinline))
kvx_float64_fsbf(float64_t a, float64_t b)
{
    return __builtin_kvx_fsbfd(a, b, ".rn");
}
float64_t __attribute__ ((noinline))
kvx_float64_mul(float64_t a, float64_t b)
{
    return a * b;
}
float64_t __attribute__ ((noinline))
kvx_float64_fmul(float64_t a, float64_t b)
{
    return __builtin_kvx_fmuld(a, b, ".rn");
}
float64_t __attribute__ ((noinline))
kvx_float64_fma(float64_t a, float64_t b, float64_t c)
{
    return fma(a, b, c);
}
float64_t __attribute__ ((noinline))
kvx_float64_ffma(float64_t a, float64_t b, float64_t c)
{
    return __builtin_kvx_ffmad(a, b, c, ".rn");
}
float64_t __attribute__ ((noinline))
kvx_float64_fms1(float64_t a, float64_t b, float64_t c)
{
    return fma(-a, b, c);
}
float64_t __attribute__ ((noinline))
kvx_float64_fms2(float64_t a, float64_t b, float64_t c)
{
    return fma(a, -b, c);
}
float64_t __attribute__ ((noinline))
kvx_float64_ffms(float64_t a, float64_t b, float64_t c)
{
    return __builtin_kvx_ffmsd(a, b, c, ".rn");
}
float64_t __attribute__ ((noinline))
kvx_float64_fmin(float64_t a, float64_t b)
{
    return __builtin_kvx_fmind(a, b);
}
float64_t __attribute__ ((noinline))
kvx_float64_fmax(float64_t a, float64_t b)
{
    return __builtin_kvx_fmaxd(a, b);
}
float64_t __attribute__ ((noinline))
kvx_float64_neg(float64_t a)
{
    return -a;
}
float64_t __attribute__ ((noinline))
kvx_float64_fneg(float64_t a)
{
    return __builtin_kvx_fnegd(a);
}
float64_t __attribute__ ((noinline))
kvx_float64_fabs(float64_t a)
{
    return __builtin_kvx_fabsd(a);
}
void __attribute__ ((noinline))
kvx_float64_axpyb(int n, float64_t a, float64_t b, float64_t x[], float64_t y[], float64_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float64_t __attribute ((noinline))
kvx_float64_fmins(float64_t a, float64_t b)
{
    return __builtin_kvx_fmind(a, b);
}
float64_t __attribute ((noinline))
kvx_float64_fmaxs(float64_t a, float64_t b)
{
    return __builtin_kvx_fmaxd(a, b);
}
float64_t __attribute ((noinline))
kvx_float64_copysign(float64_t a, float64_t b)
{
  return __builtin_kvx_copysignd(a, b);
}
int64_t __attribute__ ((noinline))
kvx_float64_eq(float64_t a, float64_t b)
{
    return a == b;
}
int64_t __attribute__ ((noinline))
kvx_float64_gt(float64_t a, float64_t b)
{
    return a > b;
}
int64_t __attribute__ ((noinline))
kvx_float64_nez(float64_t a)
{
    return a != 0.0;
}
float64_t kvx_float64_floatd(int64_t a) {
    return __builtin_kvx_floatd(a, 20, ".rn.s");
}
float64_t kvx_float64_floatud(int64_t a) {
    return __builtin_kvx_floatud(a, 21, ".rn.s");
}
int64_t kvx_float64_fixedd(float64_t a) {
    return __builtin_kvx_fixedd(a, 22, ".rn.s");
}
int64_t kvx_float64_fixedud(float64_t a) {
    return __builtin_kvx_fixedud(a, 23, ".rn.s");
}

float64_t __attribute__ ((noinline))
kvx_float64_fcdivd(float64_t a, float64_t b)
{
    return __builtin_kvx_fcdivd(a, b, ".s");
}
float64_t __attribute__ ((noinline))
kvx_float64_fsdivd(float64_t a, float64_t b)
{
    return __builtin_kvx_fsdivd(a, b, ".s");
}
