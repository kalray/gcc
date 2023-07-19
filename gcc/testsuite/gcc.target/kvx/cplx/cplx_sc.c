#include <complex.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 64
#define EPSILON 0.01

float complex add (float complex a, float complex b)
  __attribute__ ((noinline));
float complex sub (float complex a, float complex b)
  __attribute__ ((noinline));
float complex addconj (float complex a, float complex b)
  __attribute__ ((noinline));
float complex subconj (float complex a, float complex b)
  __attribute__ ((noinline));
float complex mul (float complex a, float complex b)
  __attribute__ ((noinline));
float complex mulconj (float complex a, float complex b)
  __attribute__ ((noinline));
float complex neg (float complex a) __attribute__ ((noinline));
float complex conjugate (float complex a) __attribute__ ((noinline));
float complex fmacplx (float complex a, float complex b, float complex c)
  __attribute__ ((noinline));
float complex fmscplx (float complex a, float complex b, float complex c)
  __attribute__ ((noinline));
float complex fmaconjcplx (float complex a, float complex b, float complex c)
  __attribute__ ((noinline));
float complex fmsconjcplx (float complex a, float complex b, float complex c)
  __attribute__ ((noinline));

float complex
add (float complex a, float complex b)
{
  return a + b;
}

float complex
sub (float complex a, float complex b)
{
  return a - b;
}

float complex
addconj (float complex a, float complex b)
{
  return a + (~b);
}

float complex
subconj (float complex a, float complex b)
{
  return a - (~b);
}

float complex
mul (float complex a, float complex b)
{
  return a * b;
}

float complex
mulconj (float complex a, float complex b)
{
  return a * (~b);
}

float complex
neg (float complex a)
{
  return -a;
}

float complex
conjugate (float complex a)
{
  return ~a;
}

float complex
fmacplx (float complex a, float complex b, float complex c)
{
  return c + a * b;
}

float complex
fmscplx (float complex a, float complex b, float complex c)
{
  return c - a * b;
}

float complex
fmaconjcplx (float complex a, float complex b, float complex c)
{
  return c + a * (~b);
}

float complex
fmsconjcplx (float complex a, float complex b, float complex c)
{
  return c - a * (~b);
}

bool
check (float complex a, float complex b)
{
  if ((crealf (a) / crealf (b)) < (1 - EPSILON))
    return false;
  if ((crealf (a) / crealf (b)) > (1 + EPSILON))
    return false;
  if ((cimagf (a) / cimagf (b)) < (1 - EPSILON))
    return false;
  if ((cimagf (a) / cimagf (b)) > (1 + EPSILON))
    return false;
  return true;
}

int
main ()
{
  srand (0xbeef);

  for (int i = 0; i < N; i++)
    {
      float ar = rand ();
      float ai = rand ();
      float br = rand ();
      float bi = rand ();
      float cr = rand ();
      float ci = rand ();

      float complex a = ar + ai * I;
      float complex b = br + bi * I;
      float complex c = cr + ci * I;

      float tmpr = ar + br;
      float tmpi = ai + bi;
      float complex ref = tmpr + tmpi * I;
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

      tmpr = cr + (ar * br - ai * bi);
      tmpi = ci + (ai * br + ar * bi);
      ref = tmpr + tmpi * I;
      if (!check (fmacplx (a, b, c), ref))
	return 9;

      tmpr = cr - (ar * br - ai * bi);
      tmpi = ci - (ai * br + ar * bi);
      ref = tmpr + tmpi * I;
      if (!check (fmscplx (a, b, c), ref))
	return 10;

      tmpr = cr + (ar * br + ai * bi);
      tmpi = ci + (ai * br - ar * bi);
      ref = tmpr + tmpi * I;
      if (!check (fmaconjcplx (a, b, c), ref))
	return 11;

      tmpr = cr - (ar * br + ai * bi);
      tmpi = ci - (ai * br - ar * bi);
      ref = tmpr + tmpi * I;
      if (!check (fmsconjcplx (a, b, c), ref))
	return 12;
    }

  return 0;
}
