/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fwident \\\$r\[0-9\]+ = \\\$r\[0-9\]+" } } */

float f_wident(unsigned int packed_half_float){
  return __builtin_k1_fwident(packed_half_float);
}
