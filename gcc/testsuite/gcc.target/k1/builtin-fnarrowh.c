/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "fnarrowh \\\$r\[0-9\]+ = \\\$r\[0-9\]+" } } */

short f_narrowh(float input){
  return __builtin_k1_fnarrowh(input);
}
