/* { dg-do  compile } */
/* { dg-skip-if "Only supported on Bostan" { *-*-* }  { "*" } { "-march=k1a" } } */
/* { dg-options "-O3 -save-temps"  } */

#define U6 17

long long slld_u6(long long reg){
  return reg << U6;
}
/* { dg-final { scan-assembler "slld \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+,17" } } */

long long slld_reg(long long reg, int shift_count){
  return reg << shift_count;
}
/* { dg-final { scan-assembler "slld \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+,\\\$r\[0-9\]+" } } */

long long srad_u6(long long reg){
  return reg >> U6;
}
/* { dg-final { scan-assembler "srad \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+,17" } } */

long long srad_reg(long long reg, int shift_count){
  return reg >> shift_count;
}
/* { dg-final { scan-assembler "srad \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+,\\\$r\[0-9\]+" } } */

unsigned long long srld_u6(unsigned long long reg){
  return reg >> U6;
}
/* { dg-final { scan-assembler "srld \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+,17" } } */

unsigned long long srld_reg(unsigned long long reg, int shift_count){
  return reg >> shift_count;
}
/* { dg-final { scan-assembler "srld \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+,\\\$r\[0-9\]+" } } */
