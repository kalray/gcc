#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((int complex) I)

int complex add (int complex a, int complex b) __attribute__ ((noinline));
int complex sub (int complex a, int complex b) __attribute__ ((noinline));
int complex addconj (int complex a, int complex b) __attribute__ ((noinline));
int complex subconj (int complex a, int complex b) __attribute__ ((noinline));
int complex mul (int complex a, int complex b) __attribute__ ((noinline));
int complex mulconj (int complex a, int complex b) __attribute__ ((noinline));
int complex neg (int complex a) __attribute__ ((noinline));
int complex conjugate (int complex a) __attribute__ ((noinline));

int complex
add (int complex a, int complex b)
{
  return a + b;
}

int complex
sub (int complex a, int complex b)
{
  return a - b;
}

int complex
addconj (int complex a, int complex b)
{
  return a + (~b);
}

int complex
subconj (int complex a, int complex b)
{
  return a - (~b);
}

int complex
mul (int complex a, int complex b)
{
  return a * b;
}

int complex
mulconj (int complex a, int complex b)
{
  return a * (~b);
}

int complex
neg (int complex a)
{
  return -a;
}

int complex
conjugate (int complex a)
{
  return ~a;
}


int
main ()
{
  srand (0xbeef);

  for (int i = 0; i < N; i++)
    {
      int ar = rand ();
      int ai = rand ();
      int br = rand ();
      int bi = rand ();

      int complex a = ar + ai * II;
      int complex b = br + bi * II;

      int tmpr = ar + br;
      int tmpi = ai + bi;
      int complex ref = tmpr + tmpi * II;
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
