/* { dg-do  compile } */
/* { dg-options "-O2 -S" } */

#include <stdio.h>
#include <math.h>

int __attribute__ ((noinline))
compsf_eq(float x, float y)
{
  return x == y;
}

int __attribute__ ((noinline))
compsf_ne(float x, float y)
{
  return x != y;
}

int __attribute__ ((noinline))
compsf_ge(float x, float y)
{
  return x >= y;
}

int __attribute__ ((noinline))
compsf_lt(float x, float y)
{
  return x < y;
}

int __attribute__ ((noinline))
compsf_gt(float x, float y)
{
  return x > y;
}

int __attribute__ ((noinline))
compsf_le(float x, float y)
{
  return x <= y;
}

int __attribute__ ((noinline))
compsf_greater(float x, float y)
{
  return isgreater(x, y);
}

int __attribute__ ((noinline))
compsf_greaterequal(float x, float y)
{
  return isgreaterequal(x, y);
}

int __attribute__ ((noinline))
compsf_less(float x, float y)
{
  return isless(x, y);
}

int __attribute__ ((noinline))
compsf_lessequal(float x, float y)
{
  return islessequal(x, y);
}

int __attribute__ ((noinline))
compsf_lessgreater(float x, float y)
{
  return islessgreater(x, y);
}

int __attribute__ ((noinline))
compsf_unordered(float x, float y)
{
  return isunordered(x, y);
}

#define PRINT(compare) \
  printf(#compare "(NAN,1.0) = %d\n", compare(NAN,1.0)); \
  printf(#compare "(1.0,NAN) = %d\n", compare(1.0,NAN)); \
  printf(#compare "(NAN,NAN) = %d\n", compare(NAN,NAN)); \
  printf(#compare "(0.0,1.0) = %d\n", compare(0.0,1.0)); \
  printf(#compare "(1.0,0.0) = %d\n", compare(1.0,0.0)); \
  printf(#compare "(1.0,1.0) = %d\n", compare(1.0,1.0)); \
  printf("\n")

int main(void)
{
  PRINT(compsf_eq);
  PRINT(compsf_unordered);
  PRINT(compsf_ne);
  PRINT(compsf_lessgreater);
  PRINT(compsf_ge);
  PRINT(compsf_greaterequal);
  PRINT(compsf_lt);
  PRINT(compsf_less);
  PRINT(compsf_gt);
  PRINT(compsf_greater);
  PRINT(compsf_le);
  PRINT(compsf_lessequal);
  return 0;
}

/* { dg-final { scan-assembler-times "fcompw\\." 13 } } */

