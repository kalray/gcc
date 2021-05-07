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

float16_t __attribute__ ((noinline))
kvx_float16_incr(float16_t a)
{
    return a + 1.0;
}
float16_t __attribute__ ((noinline))
kvx_float16_addto(float16_t a)
{
    float16_t b = 1.0f;
    return a + b;
}
float16_t __attribute__ ((noinline))
kvx_float16_subfrom(float16_t a)
{
    float16_t b = 1.0f;
    return b - a;
}
float16_t __attribute__ ((noinline))
kvx_float16_addwhere(float16_t a, float16_t __bypass *b)
{
    return a + *b;
}
float16_t __attribute__ ((noinline))
kvx_float16_add(float16_t a, float16_t b)
{
    return a + b;
}
float16_t __attribute__ ((noinline))
kvx_float16_adds(float16_t a, float16_t b)
{
    return a + b;
}
float16_t __attribute__ ((noinline))
kvx_float16_fadd(float16_t a, float16_t b)
{
    return __builtin_kvx_faddh(a, b, ".rn");
}
float16_t __attribute__ ((noinline))
kvx_float16_sub(float16_t a, float16_t b)
{
    return a - b;
}
float16_t __attribute__ ((noinline))
kvx_float16_fsbf(float16_t a, float16_t b)
{
    return __builtin_kvx_fsbfh(a, b, ".rn");
}
float16_t __attribute__ ((noinline))
kvx_float16_mul(float16_t a, float16_t b)
{
    return a * b;
}
float16_t __attribute__ ((noinline))
kvx_float16_fmul(float16_t a, float16_t b)
{
    return __builtin_kvx_fmulh(a, b, ".rn");
}
float16_t __attribute__ ((noinline))
kvx_float16_fma(float16_t a, float16_t b, float16_t c)
{
    return fma(a, b, c);
}
float16_t __attribute__ ((noinline))
kvx_float16_ffma(float16_t a, float16_t b, float16_t c)
{
    return __builtin_kvx_ffmah(a, b, c, ".rn");
}
float16_t __attribute__ ((noinline))
kvx_float16_fms1(float16_t a, float16_t b, float16_t c)
{
    return fma(-a, b, c);
}
float16_t __attribute__ ((noinline))
kvx_float16_fms2(float16_t a, float16_t b, float16_t c)
{
    return fma(a, -b, c);
}
float16_t __attribute__ ((noinline))
kvx_float16_ffms(float16_t a, float16_t b, float16_t c)
{
    return __builtin_kvx_ffmsh(a, b, c, ".rn");
}
float16_t __attribute__ ((noinline))
kvx_float16_fmin(float16_t a, float16_t b)
{
    return __builtin_kvx_fminh(a, b);
}
float16_t __attribute__ ((noinline))
kvx_float16_fmax(float16_t a, float16_t b)
{
    return __builtin_kvx_fmaxh(a, b);
}
float16_t __attribute__ ((noinline))
kvx_float16_neg(float16_t a)
{
    return -a;
}
float16_t __attribute__ ((noinline))
kvx_float16_fneg(float16_t a)
{
    return __builtin_kvx_fnegh(a);
}
float16_t __attribute__ ((noinline))
kvx_float16_fabs(float16_t a)
{
    return __builtin_kvx_fabsh(a);
}
void __attribute__ ((noinline))
kvx_float16_axpyb(int n, float16_t a, float16_t b, float16_t x[], float16_t y[], float16_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float16_t __attribute ((noinline))
kvx_float16_fmins(float16_t a, float16_t b)
{
    return __builtin_kvx_fminh(a, b);
}
float16_t __attribute ((noinline))
kvx_float16_fmaxs(float16_t a, float16_t b)
{
    return __builtin_kvx_fmaxh(a, b);
}
float16_t __attribute ((noinline))
kvx_float16_copysign(float16_t a, float16_t b)
{
  return __builtin_kvx_copysignh(a, b);
}
int16_t __attribute__ ((noinline))
kvx_float16_eq(float16_t a, float16_t b)
{
    return a == b;
}
int16_t __attribute__ ((noinline))
kvx_float16_gt(float16_t a, float16_t b)
{
    return a > b;
}
int16_t __attribute__ ((noinline))
kvx_float16_nez(float16_t a)
{
    return a != 0.0;
}
