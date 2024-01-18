/* { dg-do compile } */
/* FIXME: Should be dg-do run but it can't for now because it is run during the
   short valid in step1 and the simulator is not available.  */
#include <assert.h>

int g
  (int b1, int b2, int b3, int b4, int b5, int b6, int b7, int b8,
   int b9, int b10, int b11, int b12, int b13, int b14)
__attribute__((weak));

extern int* zero __attribute__((weak));

long __attribute__((noinline))
f (long a0, long a1, long a2, long a3, long a4, long a5, long a6,
   long a7, long a8, long a9, long a10, long a11, long a12, long n)
{
  long b0 = 0;
  long b1 = a0 + a1;
  long b2 = a2 + a10;
  long b3 = a3 + a9;
  long b4 = a4 + a8;
  long b5 = a5 + a7;
  long b6 = a6 + a6;
  long b7 = b0 + a0 + a1 + a2 + a10;
  long b8 = b1 + a1 + a2 + a3 + a11;
  long b9 = b2 + a2 + a3 + a4 + a12;
  long b10 = b3 + a3 + a4 + a5;
  long b11 = b4 + a4 + a5 + a6;
  long b12 = b5 + a5 + a6 + a7;
  long b13 = b6 + a6 + a7 + a8;
  long b14 = b7 + a7 + a8 + a9;
  if (&g)
    g (b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14);
  b0 = __builtin_kvx_scall (n, b5, b1, b2, b3, b4, "str"); /* = -1 */
  return (((a0 + b0) + (b7 + b8)) + (b9 + (b10 + b11) + (b12 + n)));
  /* 157 =   0   -1     13   18      33     24    27      30  13 */
}

int
main ()
{
  assert (f (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13) == 157);
  return 0;
}
