/* { dg-do  compile } */
/* { dg-options "-O2" } */

#include <math.h>

float k1_fnegw(float a) {
    return -a;
}
/* { dg-final { scan-assembler-times "fnegw \\\$r0 = \\\$r0" 1 } } */

float k1_fabsw(float a) {
    return fabsf(a);
}
/* { dg-final { scan-assembler-times "fabsw \\\$r0 = \\\$r0" 1 } } */

float k1_fminw(float a, float b) {
    return fminf(a, b);
}
/* { dg-final { scan-assembler-times "fminw \\\$r0 = \\\$r0, \\\$r1" 1 } } */

float k1_fmaxw(float a, float b) {
    return fmaxf(a, b);
}
/* { dg-final { scan-assembler-times "fmaxw \\\$r0 = \\\$r0, \\\$r1" 1 } } */

double k1_fnegd(double a) {
    return -a;
}
/* { dg-final { scan-assembler-times "fnegd \\\$r0 = \\\$r0" 1 } } */

double k1_fabsd(double a) {
    return fabs(a);
}
/* { dg-final { scan-assembler-times "fabsd \\\$r0 = \\\$r0" 1 } } */

double k1_fmind(double a, double b) {
    return fmin(a, b);
}
/* { dg-final { scan-assembler-times "fmind \\\$r0 = \\\$r0, \\\$r1" 1 } } */

double k1_fmaxd(double a, double b) {
    return fmax(a, b);
}
/* { dg-final { scan-assembler-times "fmaxd \\\$r0 = \\\$r0, \\\$r1" 1 } } */

