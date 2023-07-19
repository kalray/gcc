#include <complex.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 64
#define EPSILON 0.01

double complex add (double complex a, double complex b)
  __attribute__ ((noinline));
double complex sub (double complex a, double complex b)
  __attribute__ ((noinline));
double complex addconj (double complex a, double complex b)
  __attribute__ ((noinline));
double complex subconj (double complex a, double complex b)
  __attribute__ ((noinline));
double complex mul (double complex a, double complex b)
  __attribute__ ((noinline));
double complex mulconj (double complex a, double complex b)
  __attribute__ ((noinline));
double complex neg (double complex a) __attribute__ ((noinline));
double complex conjugate (double complex a) __attribute__ ((noinline));
double complex fmacplx (double complex a, double complex b, double complex c)
  __attribute__ ((noinline));
double complex fmscplx (double complex a, double complex b, double complex c)
  __attribute__ ((noinline));

double complex
add (double complex a, double complex b)
{
  return a + b;
}

double complex
sub (double complex a, double complex b)
{
  return a - b;
}

double complex
addconj (double complex a, double complex b)
{
  return a + (~b);
}

double complex
subconj (double complex a, double complex b)
{
  return a - (~b);
}

double complex
mul (double complex a, double complex b)
{
  return a * b;
}

double complex
mulconj (double complex a, double complex b)
{
  return a * (~b);
}

double complex
neg (double complex a)
{
  return -a;
}

double complex
conjugate (double complex a)
{
  return ~a;
}

double complex
fmacplx (double complex a, double complex b, double complex c)
{
  return c + a * b;
}

double complex
fmscplx (double complex a, double complex b, double complex c)
{
  return c - a * b;
}

bool
check (double complex a, double complex b)
{
  if ((creal (a) / creal (b)) < (1 - EPSILON))
    return false;
  if ((creal (a) / creal (b)) > (1 + EPSILON))
    return false;
  if ((cimag (a) / cimag (b)) < (1 - EPSILON))
    return false;
  if ((cimag (a) / cimag (b)) > (1 + EPSILON))
    return false;
  return true;
}

int
main ()
{
  srand (0xbeef);

  for (int i = 0; i < N; i++)
    {
      double ar = rand ();
      double ai = rand ();
      double br = rand ();
      double bi = rand ();

      double complex a = ar + ai * I;
      double complex b = br + bi * I;

      double tmpr = ar + br;
      double tmpi = ai + bi;
      double complex ref = tmpr + tmpi * I;
      if (!check (add (a, b), ref))
	return 1;

      tmpr = ar - br;
      tmpi = ai - bi;
      ref = tmpr + tmpi * I;
      if (!check (sub (a, b), ref))
	return 2;

      tmpr = ar + br;
      tmpi = ai - bi;
      ref = tmpr + tmpi * I;
      if (!check (addconj (a, b), ref))
	return 3;

      tmpr = ar - br;
      tmpi = ai + bi;
      ref = tmpr + tmpi * I;
      if (!check (subconj (a, b), ref))
	return 4;

      tmpr = ar * br - ai * bi;
      tmpi = ai * br + ar * bi;
      ref = tmpr + tmpi * I;
      if (!check (mul (a, b), ref))
	return 5;

      tmpr = ar * br + ai * bi;
      tmpi = ai * br - ar * bi;
      ref = tmpr + tmpi * I;
      if (!check (mulconj (a, b), ref))
	return 6;

      tmpr = -ar;
      tmpi = -ai;
      ref = tmpr + tmpi * I;
      if (!check (neg (a), ref))
	return 7;

      tmpr = ar;
      tmpi = -ai;
      ref = tmpr + tmpi * I;
      if (!check (conjugate (a), ref))
	return 8;
    }

  return 0;
}
