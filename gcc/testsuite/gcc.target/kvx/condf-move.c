/* { dg-do  compile } */
/* { dg-options "-O2 -S" } */

#include <stdio.h>
#include <math.h>

long
compsf_eq(long a, long b, float x, float y)
{
  return x == y? a: b;
}

long
compsf_ne(long a, long b, float x, float y)
{
  return x != y? a: b;
}

long
compsf_ge(long a, long b, float x, float y)
{
  return x >= y? a: b;
}

long
compsf_lt(long a, long b, float x, float y)
{
  return x < y? a: b;
}

long
compsf_gt(long a, long b, float x, float y)
{
  return x > y? a: b;
}

long
compsf_le(long a, long b, float x, float y)
{
  return x <= y? a: b;
}

long
compsf_greater(long a, long b, float x, float y)
{
  return isgreater(x, y)? a: b;
}

long
compsf_greaterequal(long a, long b, float x, float y)
{
  return isgreaterequal(x, y)? a: b;
}

long
compsf_less(long a, long b, float x, float y)
{
  return isless(x, y)? a: b;
}

long
compsf_lessequal(long a, long b, float x, float y)
{
  return islessequal(x, y)? a: b;
}

long
compsf_lessgreater(long a, long b, float x, float y)
{
  return islessgreater(x, y)? a: b;
}

long
compsf_unordered(long a, long b, float x, float y)
{
  return isunordered(x, y)? a: b;
}

/* { dg-final { scan-assembler-times "fcompw\\." 13 } } */
/* { dg-final { scan-assembler-times "cmoved\\." 12 } } */

