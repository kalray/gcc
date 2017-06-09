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
  return __builtin_k1_ffma(c, a, b);
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
  return __builtin_k1_ffmarn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmarn \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double ffmadrn(double a, double b, double c){
  return __builtin_k1_ffmadrn(a, b, c);
}
 /* { dg-final { scan-assembler-times "ffmadrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */

float k1_fms(float c, float a, float b){
  return fmaf(a /* r1 */, b /* r2 */, -c /* r0 */);
}
/* { dg-final { scan-assembler-times "ffmsnw \\\$r0 = \\\$r1, \\\$r2" 1 } } */
  
double k1_fmsd(double c, double a, double b){
  return fma(a /* r1 */, b /* r2 */, -c /* r0 */);
}
 /* { dg-final { scan-assembler-times "ffmsnd \\\$r0 = \\\$r1, \\\$r2" 1 } } */
  
float ffmsrn(float a, float b, float c){
  return __builtin_k1_ffmsrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */
  
double ffmsdrn(double a, double b, double c){
  return __builtin_k1_ffmsdrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsdrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */
  
float k1_fms2(float c, float a, float b){
  return fmaf(-a /* r1 */, b /* r2 */, c /* r0 */);
}
 /* { dg-final { scan-assembler-times "ffmsw \\\$r0 = \\\$r1, \\\$r2" 1 } } */
  
double k1_fmsd2(double c, double a, double b){
  return fma(-a /* r1 */, b /* r2 */, c /* r0 */);
}
/* { dg-final { scan-assembler-times "ffmsd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

  
float k1_fman(float c, float a, float b){
  return fmaf(-a /* r1 */, b /* r2 */, -c /* r0 */);
}
/* { dg-final { scan-assembler-times "ffmanw \\\$r0 = \\\$r1, \\\$r2" 1 } } */

  
double k1_fmand(double c, double a, double b){
  return fma(-a /* r1 */, b /* r2 */, -c /* r0 */);
}
/* { dg-final { scan-assembler-times "ffmand \\\$r0 = \\\$r1, \\\$r2" 1 } } */

  
float k1_b_ffmanrn(float a, float b, float c){
  return __builtin_k1_ffmanrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmanrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */
  
double k1_b_ffmandrn(double a, double b, double c){
  return __builtin_k1_ffmandrn(a, b, c);
}

/* { dg-final { scan-assembler-times "ffmandrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmanwd(double a, float b, float c){
  return __builtin_k1_ffmanwd(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmanwd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmawd(double a, float b, float c){
  return __builtin_k1_ffmawd(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmawd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmawdrn(double a, float b, float c){
  return __builtin_k1_ffmawdrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmawdrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */


double k1_b_ffmswd(double a, float b, float c){
  return __builtin_k1_ffmswd(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmswd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmswdrn(double a, float b, float c){
  return __builtin_k1_ffmswdrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmswdrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */


double k1_b_ffmsnwd(double a, float b, float c){
  return __builtin_k1_ffmsnwd(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsnwd \\\$r0 = \\\$r1, \\\$r2" 1 } } */

double k1_b_ffmsnwdrn(double a, float b, float c){
  return __builtin_k1_ffmsnwdrn(a, b, c);
}
/* { dg-final { scan-assembler-times "ffmsnwdrn \\\$r0 = \\\$r1, \\\$r2" 1 } } */
