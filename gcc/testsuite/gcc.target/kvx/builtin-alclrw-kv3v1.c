/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "alclrw \\\$r\[0-9\]+ = 0\\\[\\\$r\[0-9\]+\\\]" } } */
/* { dg-skip-if "Disabled on KVX CV2" { "kvx-*-*" }  { "-march=kv3-2" } { "" } } */
/* { dg-skip-if "Disabled on KVX Dolomite" { "kvx-*-*" }  { "-march=kv4-1" } { "" } } */

unsigned int f(void *addr) {
  return __builtin_kvx_alclrw(addr);
}

