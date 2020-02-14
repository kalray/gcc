/* { dg-do  compile } */
/* { dg-options "-O3 -std=c99" } */

struct test {
  unsigned long long ull1;
  unsigned long long ull2;
};

/* { dg-final { scan-assembler-times "lq \\\$r0r1" 2 } } */

struct test should_pack (unsigned long long *ptr64bits) {
  struct test ret;
  
  ret.ull1 = ptr64bits[0];
  ret.ull2 = ptr64bits[1];
  return ret;
}

struct test
should_pack_2 (unsigned long long *ptr64bits)
{
  struct test ret;

  ret.ull1 = ptr64bits[-1];
  ret.ull2 = ptr64bits[0];
  return ret;
}
