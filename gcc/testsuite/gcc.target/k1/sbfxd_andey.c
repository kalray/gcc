/* { dg-do  compile } */
/* { dg-skip-if "Test valid only on Andey" { *-*-* }  { "*" } { "-march=k1a" } } */
/* { dg-options "-O3 -save-temps"  } */

unsigned long long sbfx8d(unsigned long long a, unsigned long long b) {
  return a - (b << 3);
}

unsigned long long sbfx4d(unsigned long long a, unsigned long long b) {
  return a - (b << 2);
}
unsigned long long sbfx2d(unsigned long long a, unsigned long long b) {
  return a - (b << 1);
}

/* { dg-final { scan-assembler "sbfx2d \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+, \\\$r\[0-9\]+:\\\$r\[0-9\]+" } } */
/* { dg-final { scan-assembler "sbfx4d \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+, \\\$r\[0-9\]+:\\\$r\[0-9\]+" } } */
/* { dg-final { scan-assembler "sbfx8d \\\$r\[0-9\]+:\\\$r\[0-9\]+ = \\\$r\[0-9\]+:\\\$r\[0-9\]+, \\\$r\[0-9\]+:\\\$r\[0-9\]+" } } */
