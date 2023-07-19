#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((char complex) I)

char complex add (char complex a, char complex b) __attribute__ ((noinline));
char complex sub (char complex a, char complex b) __attribute__ ((noinline));
char complex addconj (char complex a, char complex b)
  __attribute__ ((noinline));
char complex subconj (char complex a, char complex b)
  __attribute__ ((noinline));
char complex mul (char complex a, char complex b) __attribute__ ((noinline));
char complex mulconj (char complex a, char complex b)
  __attribute__ ((noinline));
char complex neg (char complex a) __attribute__ ((noinline));
char complex conjugate (char complex a) __attribute__ ((noinline));

char complex
add (char complex a, char complex b)
{
  return a + b;
}

char complex
sub (char complex a, char complex b)
{
  return a - b;
}

char complex
addconj (char complex a, char complex b)
{
  return a + (~b);
}

char complex
subconj (char complex a, char complex b)
{
  return a - (~b);
}

char complex
mul (char complex a, char complex b)
{
  return a * b;
}

char complex
mulconj (char complex a, char complex b)
{
  return a * (~b);
}

char complex
neg (char complex a)
{
  return -a;
}

char complex
conjugate (char complex a)
{
  return ~a;
}


int
main ()
{
  srand (0xbeef);

  for (int i = 0; i < N; i++)
    {
      char ar = rand ();
      char ai = rand ();
      char br = rand ();
      char bi = rand ();

      char complex a = ar + ai * II;
      char complex b = br + bi * II;

      char tmpr = ar + br;
      char tmpi = ai + bi;
      char complex ref = tmpr + tmpi * II;
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
