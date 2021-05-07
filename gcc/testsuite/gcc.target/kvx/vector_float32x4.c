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

float32x4_t __attribute__ ((noinline))
kvx_float32x4_incr(float32x4_t a)
{
    return a + 1.0;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_addto(float32x4_t a)
{
    float32x4_t b = { 1.0, 2.0, 3.0, 4.0 };
    return a + b;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_subfrom(float32x4_t a)
{
    float32x4_t b = { 1.0, 2.0, 3.0, 4.0 };
    return b - a;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_addwhere(float32x4_t a, float32x4_t __bypass *b)
{
    return a + *b;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_add(float32x4_t a, float32x4_t b)
{
    return a + b;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_adds(float32x4_t a, float32_t b)
{
    return a + b;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fadd(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_faddwq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_sub(float32x4_t a, float32x4_t b)
{
    return a - b;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fsbf(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fsbfwq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_mul(float32x4_t a, float32x4_t b)
{
    return a * b;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmul(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fmulwq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fma(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return fma(a, b, c);
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_ffma(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmawq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fms1(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return fma(-a, b, c);
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fms2(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return fma(a, -b, c);
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_ffms(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmswq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmin(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fminwq(a, b);
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmax(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fmaxwq(a, b);
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_neg(float32x4_t a)
{
    return -a;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fneg(float32x4_t a)
{
    return __builtin_kvx_fnegwq(a);
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fabs(float32x4_t a)
{
    return __builtin_kvx_fabswq(a);
}
void __attribute__ ((noinline))
kvx_float32x4_axpyb(int n, float32_t a, float32_t b, float32x4_t x[], float32x4_t y[], float32x4_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float32x4_t __attribute ((noinline))
kvx_float32x4_fmins(float32x4_t a, float32_t b)
{
    return __builtin_kvx_fminwq(a, b-(float32x4_t){});
}
float32x4_t __attribute ((noinline))
kvx_float32x4_fmaxs(float32x4_t a, float32_t b)
{
    return __builtin_kvx_fmaxwq(a, b-(float32x4_t){});
}
float32x4_t __attribute ((noinline))
kvx_float32x4_copysign(float32x4_t a, float32x4_t b)
{
  return __builtin_kvx_copysignwq(a, b);
}
int32x4_t __attribute__ ((noinline))
kvx_float32x4_eq(float32x4_t a, float32x4_t b)
{
    return a == b;
}
int32x4_t __attribute__ ((noinline))
kvx_float32x4_gt(float32x4_t a, float32x4_t b)
{
    return a > b;
}
int32x4_t __attribute__ ((noinline))
kvx_float32x4_nez(float32x4_t a)
{
    return a != 0.0;
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_faddcwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_faddwq(a, b, ".c.rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fsbfcwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fsbfwq(a, b, ".c.rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmulwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fmulwcp(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmulcwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fmulwcp(a, b, ".c.rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_ffmawcp(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmawcp(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_ffmcawcp(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmawcp(a, b, c, ".c.rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_ffmswcp(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmswcp(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_ffmcswcp(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_ffmswcp(a, b, c, ".c.rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fconjwcp(float32x4_t a)
{
    return __builtin_kvx_fconjwcp(a);
}
float32x4_t kvx_float32x4_floatwq(int32x4_t a) {
    return __builtin_kvx_floatwq(a, 20, ".rn.s");
}
float32x4_t kvx_float32x4_floatuwq(int32x4_t a) {
    return __builtin_kvx_floatuwq(a, 21, ".rn.s");
}
int32x4_t kvx_float32x4_fixedwq(float32x4_t a) {
    return __builtin_kvx_fixedwq(a, 22, ".rn.s");
}
int32x4_t kvx_float32x4_fixeduwq(float32x4_t a) {
    return __builtin_kvx_fixeduwq(a, 23, ".rn.s");
}

float32x4_t __attribute__ ((noinline))
kvx_float32x4_frecwq(float32x4_t a)
{
    return __builtin_kvx_frecwq(a, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_frsrwq(float32x4_t a)
{
    return __builtin_kvx_frsrwq(a, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fcdivwq(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fcdivwq(a, b, ".s");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fsdivwq(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fsdivwq(a, b, ".s");
}
float64x4_t __attribute__ ((noinline))
kvx_float32x4_fmulwdq(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fmulwdq(a, b, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float32x4_ffmawdq(float32x4_t a, float32x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmawdq(a, b, c, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float32x4_ffmswdq(float32x4_t a, float32x4_t b, float64x4_t c)
{
    return __builtin_kvx_ffmswdq(a, b, c, ".rn");
}
float64x4_t __attribute__ ((noinline))
kvx_float32x4_fwidenwdq(float32x4_t a)
{
    return __builtin_kvx_fwidenwdq(a, ".s");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fnarrowdwq(float64x4_t a)
{
    return __builtin_kvx_fnarrowdwq(a, ".rn.s");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmm212w(float32x2_t a, float32x2_t b)
{
    return __builtin_kvx_fmm212w(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmma212w(float32x2_t a, float32x2_t b, float32x4_t c)
{
    return __builtin_kvx_fmma212w(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmms212w(float32x2_t a, float32x2_t b, float32x4_t c)
{
    return __builtin_kvx_fmms212w(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmm222w(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fmm222w(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmmt222w(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_fmm222w(a, b, ".nt.rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmma222w(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_fmma222w(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmmta222w(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_fmma222w(a, b, c, ".tn.rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmms222w(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_fmms222w(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
kvx_float32x4_fmmts222w(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_kvx_fmms222w(a, b, c, ".tt.rn");
}

float32x2_t __attribute__ ((noinline))
kvx_float32x4_ffdmawp(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_ffdmawp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x4_ffdmswp(float32x4_t a, float32x4_t b)
{
    return __builtin_kvx_ffdmswp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x4_ffdmdawp(float32x4_t a, float32x4_t b, float32x2_t c)
{
    return __builtin_kvx_ffdmdawp(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x4_ffdmsawp(float32x4_t a, float32x4_t b, float32x2_t c)
{
    return __builtin_kvx_ffdmsawp(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x4_ffdmdswp(float32x4_t a, float32x4_t b, float32x2_t c)
{
    return __builtin_kvx_ffdmdswp(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
kvx_float32x4_ffdmaswp(float32x4_t a, float32x4_t b, float32x2_t c)
{
    return __builtin_kvx_ffdmaswp(a, b, c, ".rn");
}
  
float32x4_t __attribute ((noinline))
kvx_float32x4_select(float32x4_t a, float32x4_t b, int32x4_t c)
{
    return __builtin_kvx_selectfwq(a, b, c, 0);
}
float32x4_t __attribute ((noinline))
kvx_float32x4_shift(float32x4_t a, float32_t b)
{
    return __builtin_kvx_shiftfwq(a, 1, b);
}
float32x4_t __attribute ((noinline))
kvx_float32x4_consfwq(float32x2_t a, float32x2_t b) {
    return __builtin_kvx_consfwq(a, b);
}
float32x4_t __attribute ((noinline))
kvx_float32x4_sconsfwq(float32x2_t a, float32x2_t b) {
    return __builtin_kvx_consfwq(b, a);
}
