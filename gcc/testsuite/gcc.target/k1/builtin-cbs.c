/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "cbs \\\$r\[0-9\]+ = \\\$r\[0-9\]+" } } */

 int f ( int x)
{
  return __builtin_k1_cbs (x);
}

