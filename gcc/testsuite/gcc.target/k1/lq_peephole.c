/* { dg-do  compile } */
/* { dg-skip-if "Only supported on Coolidge" { *-*-* }  { "*" } { "-march=k1c" } } */
/* { dg-options "-O3 -std=c99" } */

struct test {
  unsigned long long ull1;
  unsigned long long ull2;
};

/* { dg-final { scan-assembler "lq \\\$r0r1" } } */
struct test should_pack (unsigned long long *ptr64bits) {
  struct test ret;
  
  ret.ull1 = ptr64bits[0];
  ret.ull2 = ptr64bits[1];
  return ret;
}
