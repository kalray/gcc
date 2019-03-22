/* { dg-do  compile } */
/* { dg-options "-O1"  } */

#include <math.h>


/* This should not be optimized as ffmaw */
float k1_no_ffmaw(float a, float b, float c) {
  return a * b + c;
}
/* { dg-final { scan-assembler-times "faddw \\\$r0 = \\\$r0, \\\$r2" 1 } } */

/* This should not be optimized as ffmad */
double k1_no_ffmad(double a, double b, double c) {
  return a * b + c;
}
/* { dg-final { scan-assembler-times "faddd \\\$r0 = \\\$r0, \\\$r2" 1 } } */


/* This should be optimized as ffmaw */
float k1_ffmaw_math_builtin(float a, float b, float c) {
  return fmaf(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmaw \\\$r2 = \\\$r0, \\\$r1" 1 } } */

/* This should be optimized as ffmad */
double k1_ffmad_math_builtin(double a, double b, double c) {
  return fma(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmad \\\$r2 = \\\$r0, \\\$r1" 1 } } */

/* This should be optimized as ffmawd */
double k1_ffmawd_math_builtin(float a, float b, double c) {
  return fma((double)a, (double)b, c);
}
/* { dg-final { scan-assembler-times "ffmawd \\\$r2 = \\\$r1, \\\$r0" 1 } } */


/* This should be optimized as ffmsw */
float k1_ffmsw_math_builtin(float a, float b, float c) {
  return fmaf(-a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsw \\\$r2 = \\\$r0, \\\$r1" 1 } } */

/* This should be optimized as ffmsw */
float k1_ffmsw2_math_builtin(float a, float b, float c) {
  return fmaf(a, -b, c);
}
/* { dg-final { scan-assembler-times "ffmsw \\\$r2 = \\\$r1, \\\$r0" 1 } } */

/* This should be optimized as ffmsd */
double k1_ffmsd_math_builtin(double a, double b, double c) {
  return fma(-a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsd \\\$r2 = \\\$r0, \\\$r1" 1 } } */

/* This should be optimized as ffmsd */
double k1_ffmsd2_math_builtin(double a, double b, double c) {
  return fma(a, -b, c);
}
/* { dg-final { scan-assembler-times "ffmsd \\\$r2 = \\\$r1, \\\$r0" 1 } } */

/* This should be optimized as ffmswd */
double k1_ffmswd_math_builtin(float a, float b, double c) {
  return fma((double)-a, (double)b, c);
}
/* This should be optimized as ffmswd */
double k1_ffmswd2_math_builtin(float a, float b, double c) {
  return fma((double)a, (double)-b, c);
}
/* { dg-final { scan-assembler-times "ffmswd \\\$r2 = \\\$r1, \\\$r0" 2 } } */


float k1_ffmaw_k1_builtin(float a, float b, float c) {
  return __builtin_k1_ffmaw(a, b, c, -1, 0);
}
/* { dg-final { scan-assembler-times "ffmaw \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_ffmad_k1_builtin(double a, double b, double c) {
  return __builtin_k1_ffmad(a, b, c, -1, 0);
}
/* { dg-final { scan-assembler-times "ffmad \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_ffmawd_k1_builtin(double a, float b, float c) {
  return __builtin_k1_ffmawd(a, b, c, -1, 0);
}
/* { dg-final { scan-assembler-times "ffmawd \\\$r0 = \\\$r1, \\\$r2" 1 } } */


float k1_ffmsw_k1_builtin(float a, float b, float c) {
  return __builtin_k1_ffmsw(a, b, c, -1, 0);
}
/* { dg-final { scan-assembler-times "ffmsw \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_ffmsd_k1_builtin(double a, double b, double c) {
  return __builtin_k1_ffmsd(a, b, c, -1, 0);
}
/* { dg-final { scan-assembler-times "ffmsd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_ffmswd_k1_builtin(double a, float b, float c) {
  return __builtin_k1_ffmswd(a, b, c, -1, 0);
}
/* { dg-final { scan-assembler-times "ffmswd \\\$r0 = \\\$r1, \\\$r2" 1 } } */


