#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((short complex) I)

short complex add (short complex a, short complex b)
  __attribute__ ((noinline));
short complex sub (short complex a, short complex b)
  __attribute__ ((noinline));
short complex addconj (short complex a, short complex b)
  __attribute__ ((noinline));
short complex subconj (short complex a, short complex b)
  __attribute__ ((noinline));
short complex mul (short complex a, short complex b)
  __attribute__ ((noinline));
short complex mulconj (short complex a, short complex b)
  __attribute__ ((noinline));
short complex neg (short complex a) __attribute__ ((noinline));
short complex conjugate (short complex a) __attribute__ ((noinline));

short complex
add (short complex a, short complex b)
{
  return a + b;
}

short complex
sub (short complex a, short complex b)
{
  return a - b;
}

short complex
addconj (short complex a, short complex b)
{
  return a + (~b);
}

short complex
subconj (short complex a, short complex b)
{
  return a - (~b);
}

short complex
mul (short complex a, short complex b)
{
  return a * b;
}

short complex
mulconj (short complex a, short complex b)
{
  return a * (~b);
}

short complex
neg (short complex a)
{
  return -a;
}

short complex
conjugate (short complex a)
{
  return ~a;
}


int
main ()
{
  srand (0xbeef);

  for (int i = 0; i < N; i++)
    {
      short ar = rand ();
      short ai = rand ();
      short br = rand ();
      short bi = rand ();

      short complex a = ar + ai * II;
      short complex b = br + bi * II;

      short tmpr = ar + br;
      short tmpi = ai + bi;
      short complex ref = tmpr + tmpi * II;
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
