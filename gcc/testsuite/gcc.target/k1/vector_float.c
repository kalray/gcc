/* { dg-do  compile } */
/* { dg-options "-O2 -ffp-contract=fast" } */

#include <stdint.h>
#define fma(a, b, c) ((a) * (b) + c)
#define fmaf(a, b, c) ((a) * (b) + c)

typedef int16_t int16x4_t __attribute((vector_size(4*sizeof(int16_t))));
typedef uint16_t uint16x4_t __attribute((vector_size(4*sizeof(int16_t))));
typedef int16_t int16x8_t __attribute((vector_size(8*sizeof(int16_t))));
typedef uint16_t uint16x8_t __attribute((vector_size(8*sizeof(int16_t))));
typedef int32_t int32x2_t __attribute((vector_size(2*sizeof(int32_t))));
typedef uint32_t uint32x2_t __attribute((vector_size(2*sizeof(int32_t))));
typedef int32_t int32x4_t __attribute((vector_size(4*sizeof(int32_t))));
typedef uint32_t uint32x4_t __attribute((vector_size(4*sizeof(int32_t))));
typedef int64_t int64x2_t __attribute((vector_size(2*sizeof(int64_t))));
typedef uint64_t uint64x2_t __attribute((vector_size(2*sizeof(int64_t))));
typedef float float32_t;
typedef float32_t float32x2_t __attribute__((vector_size(2*sizeof(float32_t))));

float32x2_t __attribute__ ((noinline))
k1_float32x2_incr(float32x2_t a)
{
    return a + 1.0f;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_addto(float32x2_t a)
{
    float32x2_t b = { 1.0f, 2.0f };
    return a + b;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_subfrom(float32x2_t a)
{
    float32x2_t b = { 1.0f, 2.0f };
    return b - a;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_addwhere(float32x2_t a, float32x2_t __bypass *b)
{
    return a + *b;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_add(float32x2_t a, float32x2_t b)
{
    return a + b;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fadd(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_faddwp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_sub(float32x2_t a, float32x2_t b)
{
    return a - b;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fsbf(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fsbfwp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_mul(float32x2_t a, float32x2_t b)
{
    return a * b;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fmul(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fmulwp(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fma(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return fmaf(a, b, c);
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_ffma(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_k1_ffmawp(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fms1(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return fmaf(-a, b, c);
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fms2(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return fmaf(a, -b, c);
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_ffms(float32x2_t a, float32x2_t b, float32x2_t c)
{
    return __builtin_k1_ffmswp(a, b, c, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fmin(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fminwp(a, b);
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fmax(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fmaxwp(a, b);
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_neg(float32x2_t a)
{
    return -a;
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fneg(float32x2_t a)
{
    return __builtin_k1_fnegwp(a);
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fabs(float32x2_t a)
{
    return __builtin_k1_fabswp(a);
}

float32x2_t __attribute__ ((noinline))
k1_float32x2_faddcwc(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_faddcwc(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fsbfcwc(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fsbfcwc(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fmulwc(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fmulwc(a, b, ".rn");
}
float32x2_t __attribute__ ((noinline))
k1_float32x2_fmulcwc(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fmulcwc(a, b, ".rn");
}
void __attribute__ ((noinline))
k1_float32x2_axpyb(int n, float32_t a, float32_t b, float32x2_t x[], float32x2_t y[], float32x2_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}

typedef float32_t float32x4_t __attribute__((vector_size(4*sizeof(float32_t))));

float32x4_t __attribute__ ((noinline))
k1_float32x4_incr(float32x4_t a)
{
    return a + 1.0f;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_addto(float32x4_t a)
{
    float32x4_t b = { 1.0f, 2.0f, 3.0f, 4.0f };
    return a + b;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_subfrom(float32x4_t a)
{
    float32x4_t b = { 1.0f, 2.0f, 3.0f, 4.0f };
    return b - a;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_addwhere(float32x4_t a, float32x4_t __bypass *b)
{
    return a + *b;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_add(float32x4_t a, float32x4_t b)
{
    return a + b;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fadd(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_faddwq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_sub(float32x4_t a, float32x4_t b)
{
    return a - b;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fsbf(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_fsbfwq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_mul(float32x4_t a, float32x4_t b)
{
    return a * b;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fmul(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_fmulwq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fma(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return fmaf(a, b, c);
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_ffma(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_k1_ffmawq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fms1(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return fmaf(-a, b, c);
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fms2(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return fmaf(a, -b, c);
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_ffms(float32x4_t a, float32x4_t b, float32x4_t c)
{
    return __builtin_k1_ffmswq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fmin(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_fminwq(a, b);
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fmax(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_fmaxwq(a, b);
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_neg(float32x4_t a)
{
    return -a;
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fneg(float32x4_t a)
{
    return __builtin_k1_fnegwq(a);
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fabs(float32x4_t a)
{
    return __builtin_k1_fabswq(a);
}

float32x4_t __attribute__ ((noinline))
k1_float32x4_faddcwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_faddcwcp(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fsbfcwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_fsbfcwcp(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fmulwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_fmulwcp(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x4_fmulcwcp(float32x4_t a, float32x4_t b)
{
    return __builtin_k1_fmulcwcp(a, b, ".rn");
}
void __attribute__ ((noinline))
k1_float32x4_axpyb(int n, float32_t a, float32_t b, float32x4_t x[], float32x4_t y[], float32x4_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}

typedef double float64_t;
typedef float64_t float64x2_t __attribute__((vector_size(2*sizeof(float64_t))));

float64x2_t __attribute__ ((noinline))
k1_float64x2_incr(float64x2_t a)
{
    return a + 1.0;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_addto(float64x2_t a)
{
    float64x2_t b = { 1.0, 2.0 };
    return a + b;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_subfrom(float64x2_t a)
{
    float64x2_t b = { 1.0, 2.0 };
    return b - a;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_addwhere(float64x2_t a, float64x2_t __bypass *b)
{
    return a + *b;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_add(float64x2_t a, float64x2_t b)
{
    return a + b;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fadd(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fadddp(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_sub(float64x2_t a, float64x2_t b)
{
    return a - b;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fsbf(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fsbfdp(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_mul(float64x2_t a, float64x2_t b)
{
    return a * b;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fmul(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fmuldp(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fma(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return fma(a, b, c);
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_ffma(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_k1_ffmadp(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fms1(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return fma(-a, b, c);
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fms2(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return fma(a, -b, c);
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_ffms(float64x2_t a, float64x2_t b, float64x2_t c)
{
    return __builtin_k1_ffmsdp(a, b, c, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fmin(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fmindp(a, b);
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fmax(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fmaxdp(a, b);
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_neg(float64x2_t a)
{
    return -a;
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fneg(float64x2_t a)
{
    return __builtin_k1_fnegdp(a);
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fabs(float64x2_t a)
{
    return __builtin_k1_fabsdp(a);
}

float64x2_t __attribute__ ((noinline))
k1_float64x2_faddcdc(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_faddcdc(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fsbfcdc(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fsbfcdc(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fmuldc(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fmuldc(a, b, ".rn");
}
float64x2_t __attribute__ ((noinline))
k1_float64x2_fmulcdc(float64x2_t a, float64x2_t b)
{
    return __builtin_k1_fmulcdc(a, b, ".rn");
}
void __attribute__ ((noinline))
k1_float64x2_axpyb(int n, float64_t a, float64_t b, float64x2_t x[], float64x2_t y[], float64x2_t z[])
{
   int i;
   for (i = 0; i < n/4; i++) {
       z[i+0] = a*x[i+0] + (y[i+0]+b);
       z[i+1] = a*x[i+1] + (y[i+1]+b);
       z[i+2] = a*x[i+2] + (y[i+2]+b);
       z[i+3] = a*x[i+3] + (y[i+3]+b);
    }
}

float32x4_t __attribute__ ((noinline))
k1_float32x2_fmm2wq(float32x2_t a, float32x2_t b)
{
    return __builtin_k1_fmm2wq(a, b, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x2_fmm2awq(float32x2_t a, float32x2_t b, float32x4_t c)
{
    return __builtin_k1_fmm2awq(a, b, c, ".rn");
}
float32x4_t __attribute__ ((noinline))
k1_float32x2_fmm2swq(float32x2_t a, float32x2_t b, float32x4_t c)
{
    return __builtin_k1_fmm2swq(a, b, c, ".rn");
}

float32_t stof(signed int a) {
    return (float32_t)a;
}
float32_t utof(unsigned int a) {
    return (float32_t)a;
}
signed int f2s(float32_t a) {
    return (signed int)a;
}
unsigned int f2u(float32_t a) {
    return (unsigned int)a;
}
float64_t stod(signed long long a) {
    return (float64_t)a;
}
float64_t utod(unsigned long long a) {
    return (float64_t)a;
}
signed long long d2s(float64_t a) {
    return (signed long long)a;
}
unsigned long long d2u(float64_t a) {
    return (unsigned long long)a;
}

float32_t floatw(int32_t a) {
    return __builtin_k1_floatw(a, 20, ".rn.s");
}
float32_t floatuw(uint32_t a) {
    return __builtin_k1_floatuw(a, 21, ".rn.s");
}
int32_t fixedw(float32_t a) {
    return __builtin_k1_fixedw(a, 22, ".rn.s");
}
uint32_t fixeduw(float32_t a) {
    return __builtin_k1_fixeduw(a, 23, ".rn.s");
}

float64_t floatd(int64_t a) {
    return __builtin_k1_floatd(a, 20, ".rn.s");
}
float64_t floatud(uint64_t a) {
    return __builtin_k1_floatud(a, 21, ".rn.s");
}
int64_t fixedd(float64_t a) {
    return __builtin_k1_fixedd(a, 22, ".rn.s");
}
uint64_t fixedud(float64_t a) {
    return __builtin_k1_fixedud(a, 23, ".rn.s");
}

float32x2_t floatwp(int32x2_t a) {
    return __builtin_k1_floatwp(a, 20, ".rn.s");
}
float32x2_t floatuwp(uint32x2_t a) {
    return __builtin_k1_floatuwp(a, 21, ".rn.s");
}
int32x2_t fixedwp(float32x2_t a) {
    return __builtin_k1_fixedwp(a, 22, ".rn.s");
}
uint32x2_t fixeduwp(float32x2_t a) {
    return __builtin_k1_fixeduwp(a, 23, ".rn.s");
}

float32x4_t floatwq(int32x4_t a) {
    return __builtin_k1_floatwq(a, 20, ".rn.s");
}
float32x4_t floatuwq(uint32x4_t a) {
    return __builtin_k1_floatuwq(a, 21, ".rn.s");
}
int32x4_t fixedwq(float32x4_t a) {
    return __builtin_k1_fixedwq(a, 22, ".rn.s");
}
uint32x4_t fixeduwq(float32x4_t a) {
    return __builtin_k1_fixeduwq(a, 23, ".rn.s");
}

float64x2_t floatdp(int64x2_t a) {
    return __builtin_k1_floatdp(a, 20, ".rn.s");
}
float64x2_t floatudp(uint64x2_t a) {
    return __builtin_k1_floatudp(a, 21, ".rn.s");
}
int64x2_t fixeddp(float64x2_t a) {
    return __builtin_k1_fixeddp(a, 22, ".rn.s");
}
uint64x2_t fixedudp(float64x2_t a) {
    return __builtin_k1_fixedudp(a, 23, ".rn.s");
}

/* { dg-final { scan-assembler-times "faddwq \\\$r0r1" 8 } } */
