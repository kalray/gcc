/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "alclrw \\\$r\[0-9\]+ = \\\[\\\$r\[0-9\]+\\\]" } } */
/* { dg-skip-if "Disabled on KVX CV1" { "kvx-*-*" } { "-march=kv3-1" } { "" } } */

unsigned int f(void *addr) {
  return __builtin_kvx_alclrw(addr);
}

