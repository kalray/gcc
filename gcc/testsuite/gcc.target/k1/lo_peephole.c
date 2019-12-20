/* { dg-do  compile } */
/* { dg-skip-if "Only supported on Coolidge" { *-*-* }  { "*" } { "-march=k1c" } } */
/* { dg-options "-O3 -std=c99" } */

struct test {
  unsigned long long ull1;
  unsigned long long ull2;
  unsigned long long ull3;
  unsigned long long ull4;
};

/* { dg-final { scan-assembler-times "lo \\\$r0r1r2r3" 2 } } */
struct test should_pack (unsigned long long *ptr64bits) {
  struct test ret;
  
  ret.ull1 = ptr64bits[0];
  ret.ull2 = ptr64bits[1];
  ret.ull3 = ptr64bits[2];
  ret.ull4 = ptr64bits[3];
  return ret;
}

struct test
should_pack_2 (unsigned long long *ptr64bits)
{
  struct test ret;

  ret.ull1 = ptr64bits[-1];
  ret.ull2 = ptr64bits[0];
  ret.ull3 = ptr64bits[1];
  ret.ull4 = ptr64bits[2];
  return ret;
}
