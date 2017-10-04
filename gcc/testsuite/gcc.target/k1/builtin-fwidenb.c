/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fwidenbw \\\$r\[0-9\]+ = \\\$r\[0-9\]+" } } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

float f_widenb(unsigned int packed_half_float){
  return __builtin_k1_fwidenbw(packed_half_float);
}
