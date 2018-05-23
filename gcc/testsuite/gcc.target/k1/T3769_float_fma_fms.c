/* { dg-do  compile } */
/* { dg-options "-O3"  } */

#include <math.h>

/*
 * Conforming compiler should not use fma without fast-math.
 * We are not a conforming compiler...
 */


/* This should not be optimized as ffma */
float k1_fma(float a, float b, float c){
  return a * b + c;
}

float k1_fma_gcc_builtin(float a, float b, float c){
  return fmaf(a, b, c);
}

float k1_fma_k1_builtin(float a, float b, float c){
  return __builtin_k1_ffmaw(c, a, b);
}
/* { dg-final { scan-assembler-times "ffmaw \\\$r2 = \\\$r0, \\\$r1" 3 } } */


/* This should not be optimized as ffma */
double k1_fmad(double a, double b, double c){
  return a * b + c;
}

double k1_fmad_gcc_builtin(double a, double b, double c){
  return fma(a, b, c);
}

double k1_fmad_k1_builtin(double a, double b, double c){
  return __builtin_k1_ffmad(c, a, b);
}
/* { dg-final { scan-assembler-times "ffmad \\\$r2 = \\\$r0, \\\$r1" 3 } } */


float ffmarn(float a, float b, float c){
  return __builtin_k1_ffmawrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmaw.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double ffmadrn(double a, double b, double c){
  return __builtin_k1_ffmadrn(a, b, c);
}
 /* { dg-final { scan-assembler-times "ffmad.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */

float ffmsrn(float a, float b, float c){
  return __builtin_k1_ffmswrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsw.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double ffmsdrn(double a, double b, double c){
  return __builtin_k1_ffmsdrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsd.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */

float k1_fms2(float c, float a, float b){
  return fmaf(-a /* r1 */, b /* r2 */, c /* r0 */);
}
 /* { dg-final { scan-assembler-times "ffmsw \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_fmsd2(double c, double a, double b){
  return fma(-a /* r1 */, b /* r2 */, c /* r0 */);
}
/* { dg-final { scan-assembler-times "ffmsd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmawd(double a, float b, float c){
  return __builtin_k1_ffmawd(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmawd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmawdrn(double a, float b, float c){
  return __builtin_k1_ffmawdrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmawd.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmswd(double a, float b, float c){
  return __builtin_k1_ffmswd(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmswd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmswdrn(double a, float b, float c){
  return __builtin_k1_ffmswdrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmswd.rn.s \\\$r0 = \\\$r1, \\\$r2" 1 } } */

