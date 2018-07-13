/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "alclrw \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */
/* { dg-skip-if "Test valid only on Coolidge" { *-*-* }  {  "*" } { "-march=k1c" } } */

unsigned int f(void *addr) {
  return __builtin_k1_alclrw(addr);
}

