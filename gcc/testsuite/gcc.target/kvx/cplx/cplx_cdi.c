#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((long complex) I)

long complex add (long complex a, long complex b) __attribute__ ((noinline));
long complex sub (long complex a, long complex b) __attribute__ ((noinline));
long complex addconj (long complex a, long complex b)
  __attribute__ ((noinline));
long complex subconj (long complex a, long complex b)
  __attribute__ ((noinline));
long complex mul (long complex a, long complex b) __attribute__ ((noinline));
long complex mulconj (long complex a, long complex b)
  __attribute__ ((noinline));
long complex neg (long complex a) __attribute__ ((noinline));
long complex conjugate (long complex a) __attribute__ ((noinline));

long complex
add (long complex a, long complex b)
{
  return a + b;
}

long complex
sub (long complex a, long complex b)
{
  return a - b;
}

long complex
addconj (long complex a, long complex b)
{
  return a + (~b);
}

long complex
subconj (long complex a, long complex b)
{
  return a - (~b);
}

long complex
mul (long complex a, long complex b)
{
  return a * b;
}

long complex
mulconj (long complex a, long complex b)
{
  return a * (~b);
}

long complex
neg (long complex a)
{
  return -a;
}

long complex
conjugate (long complex a)
{
  return ~a;
}


int
main ()
{
  srand (0xbeef);

  for (int i = 0; i < N; i++)
    {
      long ar = rand ();
      long ai = rand ();
      long br = rand ();
      long bi = rand ();

      long complex a = ar + ai * II;
      long complex b = br + bi * II;

      long tmpr = ar + br;
      long tmpi = ai + bi;
      long complex ref = tmpr + tmpi * II;
      if (add (a, b) != ref)
	return 1;

      tmpr = ar - br;
      tmpi = ai - bi;
      ref = tmpr + tmpi * II;
      if (sub (a, b) != ref)
	return 2;

      tmpr = ar + br;
      tmpi = ai - bi;
      ref = tmpr + tmpi * II;
      if (addconj (a, b) != ref)
	return 3;

      tmpr = ar - br;
      tmpi = ai + bi;
      ref = tmpr + tmpi * II;
      if (subconj (a, b) != ref)
	return 4;

      tmpr = ar * br - ai * bi;
      tmpi = ai * br + ar * bi;
      ref = tmpr + tmpi * II;
      if (mul (a, b) != ref)
	return 5;

      tmpr = ar * br + ai * bi;
      tmpi = ai * br - ar * bi;
      ref = tmpr + tmpi * II;
      if (mulconj (a, b) != ref)
	return 6;

      tmpr = -ar;
      tmpi = -ai;
      ref = tmpr + tmpi * II;
      if (neg (a) != ref)
	return 7;

      tmpr = ar;
      tmpi = -ai;
      ref = tmpr + tmpi * II;
      if (conjugate (a) != ref)
	return 8;
    }

  return 0;
}
