/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fwidenb \\\$r\[0-9\]+ = \\\$r\[0-9\]+" } } */

float f_widenb(unsigned int packed_half_float){
  return __builtin_k1_fwidenb(packed_half_float);
}
