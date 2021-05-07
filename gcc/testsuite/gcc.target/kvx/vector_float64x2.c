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

float64x2_t __attribute__ ((noinline))
kvx_float64x2_incr(float64x2_t a)
{
    return a + 1.0;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_addto(float64x2_t a)
{
    float64x2_t b = { 1.0, 2.0 };
    return a + b;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_subfrom(float64x2_t a)
{
    float64x2_t b = { 1.0, 2.0 };
    return b - a;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_addwhere(float64x2_t a, float64x2_t __bypass *b)
{
    return a + *b;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_add(float64x2_t a, float64x2_t b)
{
    return a + b;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_adds(float64x2_t a, float64_t b)
{
    return a + b;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fadd(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fadddp(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_sub(float64x2_t a, float64x2_t b)
{
    return a - b;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fsbf(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fsbfdp(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_mul(float64x2_t a, float64x2_t b)
{
    return a * b;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fmul(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fmuldp(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fma(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return fma(a, b, c);
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_ffma(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmadp(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fms1(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return fma(-a, b, c);
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fms2(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return fma(a, -b, c);
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_ffms(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmsdp(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fmin(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fmindp(a, b);
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fmax(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fmaxdp(a, b);
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_neg(float64x2_t a)
{
    return -a;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fneg(float64x2_t a)
{
    return __builtin_kvx_fnegdp(a);
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fabs(float64x2_t a)
{
    return __builtin_kvx_fabsdp(a);
}
void __attribute__ ((noinline))
kvx_float64x2_axpyb(int n, float64_t a, float64_t b, float64x2_t x[], float64x2_t y[], float64x2_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}
float64x2_t __attribute ((noinline))
kvx_float64x2_fmins(float64x2_t a, float64_t b)
{
    return __builtin_kvx_fmindp(a, b-(float64x2_t){});
}
float64x2_t __attribute ((noinline))
kvx_float64x2_fmaxs(float64x2_t a, float64_t b)
{
    return __builtin_kvx_fmaxdp(a, b-(float64x2_t){});
}
float64x2_t __attribute ((noinline))
kvx_float64x2_copysign(float64x2_t a, float64x2_t b)
{
  return __builtin_kvx_copysigndp(a, b);
}
int64x2_t __attribute__ ((noinline))
kvx_float64x2_eq(float64x2_t a, float64x2_t b)
{
    return a == b;
}
int64x2_t __attribute__ ((noinline))
kvx_float64x2_gt(float64x2_t a, float64x2_t b)
{
    return a > b;
}
int64x2_t __attribute__ ((noinline))
kvx_float64x2_nez(float64x2_t a)
{
    return a != 0.0;
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_faddcdc(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fadddp(a, b, ".c.rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fsbfcdc(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fsbfdp(a, b, ".c.rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fmuldc(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fmuldc(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fmulcdc(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fmuldc(a, b, ".c.rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_ffmadc(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmadc(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_ffmcadc(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmadc(a, b, c, ".c.rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_ffmsdc(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmsdc(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_ffmcsdc(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_kvx_ffmsdc(a, b, c, ".c.rn");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fconjdc(float64x2_t a)
{
    return __builtin_kvx_fconjdc(a);
}
float64x2_t kvx_float64x2_floatdp(int64x2_t a) {
    return __builtin_kvx_floatdp(a, 20, ".rn.s");
}
float64x2_t kvx_float64x2_floatudp(int64x2_t a) {
    return __builtin_kvx_floatudp(a, 21, ".rn.s");
}
int64x2_t kvx_float64x2_fixeddp(float64x2_t a) {
    return __builtin_kvx_fixeddp(a, 22, ".rn.s");
}
int64x2_t kvx_float64x2_fixedudp(float64x2_t a) {
    return __builtin_kvx_fixedudp(a, 23, ".rn.s");
}

float64x2_t __attribute__ ((noinline))
kvx_float64x2_fcdivdp(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fcdivdp(a, b, ".s");
}
float64x2_t __attribute__ ((noinline))
kvx_float64x2_fsdivdp(float64x2_t a, float64x2_t b)
{
    return __builtin_kvx_fsdivdp(a, b, ".s");
}
float64x2_t __attribute ((noinline))
kvx_float64x2_select(float64x2_t a, float64x2_t b, int64x2_t c)
{
    return __builtin_kvx_selectfdp(a, b, c, 0);
}
float64x2_t __attribute ((noinline))
kvx_float64x2_shift(float64x2_t a, float64_t b)
{
    return __builtin_kvx_shiftfdp(a, 1, b);
}
float64x2_t __attribute ((noinline))
kvx_float64x2_consfdp(float64_t a, float64_t b) {
    return __builtin_kvx_consfdp(a, b);
}
float64x2_t __attribute ((noinline))
kvx_float64x2_sconsfdp(float64_t a, float64_t b) {
    return __builtin_kvx_consfdp(b, a);
}
