/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-not "bswapsi2" } } */

long long foo (long long x)
{
  return __builtin_bswap32 (x);
}

