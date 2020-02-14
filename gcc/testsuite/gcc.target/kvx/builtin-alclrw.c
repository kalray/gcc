/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "alclrw \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */

unsigned int f(void *addr) {
  return __builtin_kvx_alclrw(addr);
}

