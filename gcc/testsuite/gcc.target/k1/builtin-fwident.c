/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fwidenmhw \\\$r\[0-9\]+ = \\\$r\[0-9\]+" } } */

float f_widenthw(unsigned int packed_half_float){
  return __builtin_k1_fwidenmhw(packed_half_float);
}
