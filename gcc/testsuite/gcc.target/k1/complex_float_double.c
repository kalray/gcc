/* { dg-do  compile } */
/* { dg-options "-O2 -fcx-fortran-rules" } */

#include <complex.h>


complex float k1_faddwc(complex float a, complex float b) {
    return a + b;
}

complex float k1_faddcwc(complex float a, complex float b) {
    return conjf(a + b);
}

complex float k1_faddwc_builtin(complex float a, complex float b) {
    return __builtin_k1_faddwc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "faddwp \\\$r0 = \\\$r0, \\\$r1" 2 } } */

complex float k1_faddcwc_builtin(complex float a, complex float b) {
    return __builtin_k1_faddcwc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "faddcwc \\\$r0 = \\\$r0, \\\$r1" 1 } } */


complex float k1_fsbfwc(complex float a, complex float b) {
    return a - b;
}

complex float k1_fsbfcwc(complex float a, complex float b) {
    return conjf(a - b);
}

complex float k1_fsbfwc_builtin(complex float a, complex float b) {
    return __builtin_k1_fsbfwc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "fsbfwp \\\$r0 = \\\$r0, \\\$r1" 1 } } */

complex float k1_fsbfcwc_builtin(complex float a, complex float b) {
    return __builtin_k1_fsbfcwc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "fsbfcwc \\\$r0 = \\\$r0, \\\$r1" 1 } } */


complex float k1_fmulwc(complex float a, complex float b) {
    return a * b;
}

complex float k1_fmulcwc(complex float a, complex float b) {
    return conjf(a * b);
}

complex float k1_fmulwc_builtin(complex float a, complex float b) {
    return __builtin_k1_fmulwc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "fmulwc \\\$r0 = \\\$r0, \\\$r1" 1 } } */

complex float k1_fmulcwc_builtin(complex float a, complex float b) {
    return __builtin_k1_fmulcwc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "fmulcwc \\\$r0 = \\\$r0, \\\$r1" 1 } } */


complex float k1_ffmawc(complex float a, complex float b, complex float c) {
    return a + b * c;
}

complex float k1_ffmawc_builtin(complex float a, complex float b, complex float c) {
    return __builtin_k1_ffmawc(a, b, c, -1, 0);
}


complex float k1_ffmswc(complex float a, complex float b, complex float c) {
    return a - b * c;
}

complex float k1_ffmswc_builtin(complex float a, complex float b, complex float c) {
    return __builtin_k1_ffmswc(a, b, c, -1, 0);
}
/* { dg-final { scan-assembler-times "fsbfwp \\\$r0 = \\\$r1, \\\$r0" 1 } } */


complex double k1_fadddc(complex double a, complex double b) {
    return a + b;
}

complex double k1_faddcdc(complex double a, complex double b) {
    return conj(a + b);
}

complex double k1_fadddc_builtin(complex double a, complex double b) {
    return __builtin_k1_fadddc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "fadddp \\\$r4r5 = \\\$r4r5, \\\$r6r7" 1 } } */

complex double k1_faddcdc_builtin(complex double a, complex double b) {
    return __builtin_k1_faddcdc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "faddcdc \\\$r4r5 = \\\$r4r5, \\\$r6r7" 1 } } */


complex double k1_fsbfdc(complex double a, complex double b) {
    return a - b;
}

complex double k1_fsbfcdc(complex double a, complex double b) {
    return conj(a - b);
}

complex double k1_fsbfdc_builtin(complex double a, complex double b) {
    return __builtin_k1_fsbfdc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "fsbfdp \\\$r4r5 = \\\$r4r5, \\\$r6r7" 1 } } */

complex double k1_fsbfcdc_builtin(complex double a, complex double b) {
    return __builtin_k1_fsbfcdc(a, b, -1, 0);
}
/* { dg-final { scan-assembler-times "fsbfcdc \\\$r4r5 = \\\$r4r5, \\$r6r7" 1 } } */


complex double k1_fmuldc(complex double a, complex double b) {
    return a * b;
}

complex double k1_fmulcdc(complex double a, complex double b) {
    return conj(a * b);
}

complex double k1_fmuldc_builtin(complex double a, complex double b) {
    return __builtin_k1_fmuldc(a, b, -1, 0);
}

complex double k1_fmulcdc_builtin(complex double a, complex double b) {
    return __builtin_k1_fmulcdc(a, b, -1, 0);
}


complex double k1_ffmadc(complex double a, complex double b, complex double c) {
    return a + b * c;
}

complex double k1_ffmadc_builtin(complex double a, complex double b, complex double c) {
    return __builtin_k1_ffmadc(a, b, c, -1, 0);
}


complex double k1_ffmsdc(complex double a, complex double b, complex double c) {
    return a - b * c;
}

complex double k1_ffmsdc_builtin(complex double a, complex double b, complex double c) {
    return __builtin_k1_ffmsdc(a, b, c, -1, 0);
}

