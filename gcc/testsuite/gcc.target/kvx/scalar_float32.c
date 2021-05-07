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

float32_t __attribute__ ((noinline))
kvx_float32_incr(float32_t a)
{
    return a + 1.0;
}
float32_t __attribute__ ((noinline))
kvx_float32_addto(float32_t a)
{
    float32_t b = 1.0f;
    return a + b;
}
float32_t __attribute__ ((noinline))
kvx_float32_subfrom(float32_t a)
{
    float32_t b = 1.0f;
    return b - a;
}
float32_t __attribute__ ((noinline))
kvx_float32_addwhere(float32_t a, float32_t __bypass *b)
{
    return a + *b;
}
float32_t __attribute__ ((noinline))
kvx_float32_add(float32_t a, float32_t b)
{
    return a + b;
}
float32_t __attribute__ ((noinline))
kvx_float32_adds(float32_t a, float32_t b)
{
    return a + b;
}
float32_t __attribute__ ((noinline))
kvx_float32_fadd(float32_t a, float32_t b)
{
    return __builtin_kvx_faddw(a, b, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32_sub(float32_t a, float32_t b)
{
    return a - b;
}
float32_t __attribute__ ((noinline))
kvx_float32_fsbf(float32_t a, float32_t b)
{
    return __builtin_kvx_fsbfw(a, b, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32_mul(float32_t a, float32_t b)
{
    return a * b;
}
float32_t __attribute__ ((noinline))
kvx_float32_fmul(float32_t a, float32_t b)
{
    return __builtin_kvx_fmulw(a, b, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32_fma(float32_t a, float32_t b, float32_t c)
{
    return fma(a, b, c);
}
float32_t __attribute__ ((noinline))
kvx_float32_ffma(float32_t a, float32_t b, float32_t c)
{
    return __builtin_kvx_ffmaw(a, b, c, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32_fms1(float32_t a, float32_t b, float32_t c)
{
    return fma(-a, b, c);
}
float32_t __attribute__ ((noinline))
kvx_float32_fms2(float32_t a, float32_t b, float32_t c)
{
    return fma(a, -b, c);
}
float32_t __attribute__ ((noinline))
kvx_float32_ffms(float32_t a, float32_t b, float32_t c)
{
    return __builtin_kvx_ffmsw(a, b, c, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32_fmin(float32_t a, float32_t b)
{
    return __builtin_kvx_fminw(a, b);
}
float32_t __attribute__ ((noinline))
kvx_float32_fmax(float32_t a, float32_t b)
{
    return __builtin_kvx_fmaxw(a, b);
}
float32_t __attribute__ ((noinline))
kvx_float32_neg(float32_t a)
{
    return -a;
}
float32_t __attribute__ ((noinline))
kvx_float32_fneg(float32_t a)
{
    return __builtin_kvx_fnegw(a);
}
float32_t __attribute__ ((noinline))
kvx_float32_fabs(float32_t a)
{
    return __builtin_kvx_fabsw(a);
}
void __attribute__ ((noinline))
kvx_float32_axpyb(int n, float32_t a, float32_t b, float32_t x[], float32_t y[], float32_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float32_t __attribute ((noinline))
kvx_float32_fmins(float32_t a, float32_t b)
{
    return __builtin_kvx_fminw(a, b);
}
float32_t __attribute ((noinline))
kvx_float32_fmaxs(float32_t a, float32_t b)
{
    return __builtin_kvx_fmaxw(a, b);
}
float32_t __attribute ((noinline))
kvx_float32_copysign(float32_t a, float32_t b)
{
  return __builtin_kvx_copysignw(a, b);
}
int32_t __attribute__ ((noinline))
kvx_float32_eq(float32_t a, float32_t b)
{
    return a == b;
}
int32_t __attribute__ ((noinline))
kvx_float32_gt(float32_t a, float32_t b)
{
    return a > b;
}
int32_t __attribute__ ((noinline))
kvx_float32_nez(float32_t a)
{
    return a != 0.0;
}
float32_t kvx_float32_floatw(int32_t a) {
    return __builtin_kvx_floatw(a, 20, ".rn.s");
}
float32_t kvx_float32_floatuw(int32_t a) {
    return __builtin_kvx_floatuw(a, 21, ".rn.s");
}
int32_t kvx_float32_fixedw(float32_t a) {
    return __builtin_kvx_fixedw(a, 22, ".rn.s");
}
int32_t kvx_float32_fixeduw(float32_t a) {
    return __builtin_kvx_fixeduw(a, 23, ".rn.s");
}

float32_t __attribute__ ((noinline))
kvx_float32_frecw(float32_t a)
{
    return __builtin_kvx_frecw(a, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32_frsrw(float32_t a)
{
    return __builtin_kvx_frsrw(a, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32_fcdivw(float32_t a, float32_t b)
{
    return __builtin_kvx_fcdivw(a, b, ".s");
}
float32_t __attribute__ ((noinline))
kvx_float32_fsdivw(float32_t a, float32_t b)
{
    return __builtin_kvx_fsdivw(a, b, ".s");
}
