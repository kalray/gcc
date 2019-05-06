/* { dg-do  compile } */
/* { dg-options "-O2 -fcx-fortran-rules" } */

#include <complex.h>

complex float k1_faddwc(complex float a, complex float b) {
    return a + b;
}

complex float k1_faddcwc(complex float a, complex float b) {
    return conjf(a + b);
}

complex float k1_fsbfwc(complex float a, complex float b) {
    return a - b;
}

complex float k1_fsbfcwc(complex float a, complex float b) {
    return conjf(a - b);
}

complex float k1_fmulwc(complex float a, complex float b) {
    return a * b;
}

complex float k1_fmulcwc(complex float a, complex float b) {
    return conjf(a * b);
}

complex float k1_ffmawc(complex float a, complex float b, complex float c) {
    return a + b * c;
}

complex float k1_ffmswc(complex float a, complex float b, complex float c) {
    return a - b * c;
}

complex double k1_fadddc(complex double a, complex double b) {
    return a + b;
}

complex double k1_faddcdc(complex double a, complex double b) {
    return conj(a + b);
}

complex double k1_fsbfdc(complex double a, complex double b) {
    return a - b;
}

complex double k1_fsbfcdc(complex double a, complex double b) {
    return conj(a - b);
}

complex double k1_fmuldc(complex double a, complex double b) {
    return a * b;
}

complex double k1_fmulcdc(complex double a, complex double b) {
    return conj(a * b);
}

complex double k1_ffmadc(complex double a, complex double b, complex double c) {
    return a + b * c;
}

complex double k1_ffmsdc(complex double a, complex double b, complex double c) {
    return a - b * c;
}
