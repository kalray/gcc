/* { dg-do compile } */
/* { dg-options "-O1" } */

/*volatile int x, y, z;*/

int
test_cmove_even(int p, int y, int z)
{
  int x = y;
  if ((p & 1)) x = z;      /* { dg-final { scan-assembler "cmove.even" } } */
  return x;
}
int

test_cmove_odd(int p, int y, int z)
{
  int x = y;
  if ((p & 1) == 0) x = z; /* { dg-final { scan-assembler "cmove.odd" } } */

/*  x = ((p & 1) == 0) ? y : x;*/
  return x;
}

int
test_cmove_cond(int p, int y, int z)
{
/*  return ((p & 1) == 0) ? y : z; |+ { dg-final { scan-assembler "cmove.odd" } } +|*/
/*  return ((p & 1)) ? y : z; |+ { dg-final { scan-assembler "cmove.odd" } } +|*/
/*  return p ? y : z; |+ { dg-final { scan-assembler "cmove.odd" } } +|*/
  return !p ? y : z; /* { dg-final { scan-assembler "cmove.odd" } } */
}
