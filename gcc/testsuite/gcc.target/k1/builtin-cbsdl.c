/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "cbsdl \\\$r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+" } } */

 int f ( unsigned long long x)
{
  return __builtin_k1_cbsdl (x);
}

