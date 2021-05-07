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

float32x2_t __attribute__ ((noinline))
kvx_float32x2_incr(float32x2_t a)
{
    return a + 1.0;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_addto(float32x2_t a)
{
    float32x2_t b = { 1.0, 2.0 };
    return a + b;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_subfrom(float32x2_t a)
{
    float32x2_t b = { 1.0, 2.0 };
    return b - a;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_addwhere(float32x2_t a, float32x2_t __bypass *b)
{
    return a + *b;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_add(float32x2_t a, float32x2_t b)
{
    return a + b;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_adds(float32x2_t a, float32_t b)
{
    return a + b;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fadd(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_faddwp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_sub(float32x2_t a, float32x2_t b)
{
    return a - b;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fsbf(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fsbfwp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_mul(float32x2_t a, float32x2_t b)
{
    return a * b;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fmul(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fmulwp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fma(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return fma(a, b, c);
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_ffma(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_kvx_ffmawp(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fms1(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return fma(-a, b, c);
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fms2(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return fma(a, -b, c);
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_ffms(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_kvx_ffmswp(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fmin(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fminwp(a, b);
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fmax(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fmaxwp(a, b);
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_neg(float32x2_t a)
{
    return -a;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fneg(float32x2_t a)
{
    return __builtin_kvx_fnegwp(a);
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fabs(float32x2_t a)
{
    return __builtin_kvx_fabswp(a);
}
void __attribute__ ((noinline))
kvx_float32x2_axpyb(int n, float32_t a, float32_t b, float32x2_t x[], float32x2_t y[], float32x2_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float32x2_t __attribute ((noinline))
kvx_float32x2_fmins(float32x2_t a, float32_t b)
{
    return __builtin_kvx_fminwp(a, b-(float32x2_t){});
}
float32x2_t __attribute ((noinline))
kvx_float32x2_fmaxs(float32x2_t a, float32_t b)
{
    return __builtin_kvx_fmaxwp(a, b-(float32x2_t){});
}
float32x2_t __attribute ((noinline))
kvx_float32x2_copysign(float32x2_t a, float32x2_t b)
{
  return __builtin_kvx_copysignwp(a, b);
}
int32x2_t __attribute__ ((noinline))
kvx_float32x2_eq(float32x2_t a, float32x2_t b)
{
    return a == b;
}
int32x2_t __attribute__ ((noinline))
kvx_float32x2_gt(float32x2_t a, float32x2_t b)
{
    return a > b;
}
int32x2_t __attribute__ ((noinline))
kvx_float32x2_nez(float32x2_t a)
{
    return a != 0.0;
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_faddcwc(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_faddwp(a, b, ".c.rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fsbfcwc(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fsbfwp(a, b, ".c.rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fmulwc(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fmulwc(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fmulcwc(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fmulwc(a, b, ".c.rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_ffmawc(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_kvx_ffmawc(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_ffmcawc(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_kvx_ffmawc(a, b, c, ".c.rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_ffmswc(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_kvx_ffmswc(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_ffmcswc(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_kvx_ffmswc(a, b, c, ".c.rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fconjwc(float32x2_t a)
{
    return __builtin_kvx_fconjwc(a);
}
float32x2_t kvx_float32x2_floatwp(int32x2_t a) {
    return __builtin_kvx_floatwp(a, 20, ".rn.s");
}
float32x2_t kvx_float32x2_floatuwp(int32x2_t a) {
    return __builtin_kvx_floatuwp(a, 21, ".rn.s");
}
int32x2_t kvx_float32x2_fixedwp(float32x2_t a) {
    return __builtin_kvx_fixedwp(a, 22, ".rn.s");
}
int32x2_t kvx_float32x2_fixeduwp(float32x2_t a) {
    return __builtin_kvx_fixeduwp(a, 23, ".rn.s");
}

float32x2_t __attribute__ ((noinline))
kvx_float32x2_frecwp(float32x2_t a)
{
    return __builtin_kvx_frecwp(a, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_frsrwp(float32x2_t a)
{
    return __builtin_kvx_frsrwp(a, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fcdivwp(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fcdivwp(a, b, ".s");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fsdivwp(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fsdivwp(a, b, ".s");
}
float64x2_t __attribute__ ((noinline))
kvx_float32x2_fmulwdp(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fmulwdp(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float32x2_ffmawdp(float32x2_t a, float32x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmawdp(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float32x2_ffmswdp(float32x2_t a, float32x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmswdp(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float32x2_fwidenwdp(float32x2_t a)
{
    return __builtin_kvx_fwidenwdp(a, ".s");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x2_fnarrowdwp(float64x2_t a)
{
    return __builtin_kvx_fnarrowdwp(a, ".rn.s");
}
float32_t __attribute__ ((noinline))
kvx_float32x2_ffdmaw(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_ffdmaw(a, b, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32x2_ffdmsw(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_ffdmsw(a, b, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32x2_ffdmdaw(float32x2_t a, float32x2_t b, float32_t c)
{
    return __builtin_kvx_ffdmdaw(a, b, c, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32x2_ffdmsaw(float32x2_t a, float32x2_t b, float32_t c)
{
    return __builtin_kvx_ffdmsaw(a, b, c, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32x2_ffdmdsw(float32x2_t a, float32x2_t b, float32_t c)
{
    return __builtin_kvx_ffdmdsw(a, b, c, ".rn");
}
float32_t __attribute__ ((noinline))
kvx_float32x2_ffdmasw(float32x2_t a, float32x2_t b, float32_t c)
{
    return __builtin_kvx_ffdmasw(a, b, c, ".rn");
}
  
float32x2_t __attribute ((noinline))
kvx_float32x2_select(float32x2_t a, float32x2_t b, int32x2_t c)
{
    return __builtin_kvx_selectfwp(a, b, c, 0);
}
float32x2_t __attribute ((noinline))
kvx_float32x2_shift(float32x2_t a, float32_t b)
{
    return __builtin_kvx_shiftfwp(a, 1, b);
}
float32x2_t __attribute ((noinline))
kvx_float32x2_consfwp(float32_t a, float32_t b) {
    return __builtin_kvx_consfwp(a, b);
}
float32x2_t __attribute ((noinline))
kvx_float32x2_sconsfwp(float32_t a, float32_t b) {
    return __builtin_kvx_consfwp(b, a);
}
