/* { dg-do compile } */
/* { dg-options "-O2" } */

typedef float float32x2_t __attribute__((vector_size(8)));


float32x2_t k1_incr(float32x2_t a)
{
    return a + 1.0;
}

float32x2_t k1_addto(float32x2_t a)
{
    float32x2_t b = { 1.0f, 2.0f };
    return a + b;
}


float32x2_t k1_faddwp(float32x2_t a, float32x2_t b) {
    return a + b;
}
/* { dg-final { scan-assembler-times "faddwp \\\$r0 = \\\$r0, \\\$r1" 3 } } */

float32x2_t k1_faddwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_faddwp(a, b, ".rn.s");
}
/* { dg-final { scan-assembler-times "faddwp.rn.s \\\$r0 = \\\$r0, \\\$r1" 1 } } */


float32x2_t k1_fsbfwp(float32x2_t a, float32x2_t b) {
    return a - b;
}
/* { dg-final { scan-assembler-times "fsbfwp \\\$r0 = \\\$r1, \\\$r0" 1 } } */

float32x2_t k1_fsbfwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_fsbfwp(a, b, ".rn.s");
}
/* { dg-final { scan-assembler-times "fsbfwp.rn.s \\\$r0 = \\\$r0, \\\$r1" 1 } } */


float32x2_t k1_fmulwp(float32x2_t a, float32x2_t b) {
    return a * b;
}
/* { dg-final { scan-assembler-times "fmulwp \\\$r0 = \\\$r0, \\\$r1" 1 } } */

float32x2_t k1_fmulwp_builtin(float32x2_t a, float32x2_t b) {
    return __builtin_k1_fmulwp(a, b, ".rn.s");
}
/* { dg-final { scan-assembler-times "fmulwp.rn.s \\\$r0 = \\\$r0, \\\$r1" 1 } } */


float32x2_t k1_ffmawp(float32x2_t a, float32x2_t b, float32x2_t c) {
    return a + b * c;
}
/* { dg-final { scan-assembler-times "ffmawp \\\$r0 = \\\$r1, \\\$r2" 1 } } */

float32x2_t k1_ffmawp_builtin(float32x2_t a, float32x2_t b, float32x2_t c) {
    return __builtin_k1_ffmawp(a, b, c, ".rn.s");
}
/* { dg-final { scan-assembler-times "ffmawp.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */


float32x2_t k1_ffmswp(float32x2_t a, float32x2_t b, float32x2_t c) {
    return a - b * c;
}
/* { dg-final { scan-assembler-times "ffmswp \\\$r0 = \\\$r1, \\\$r2" 1 } } */

float32x2_t k1_ffmswp_builtin(float32x2_t a, float32x2_t b, float32x2_t c) {
    return __builtin_k1_ffmswp(a, b, c, ".rn.s");
}
/* { dg-final { scan-assembler-times "ffmswp.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */

