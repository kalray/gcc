#include <complex.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define EPSILON 0.01
#define N 64

void add (double complex a[restrict N], double complex b[restrict N],
	  double complex c[restrict N]) __attribute__ ((noinline));
void sub (double complex a[restrict N], double complex b[restrict N],
	  double complex c[restrict N]) __attribute__ ((noinline));
void addconj (double complex a[restrict N], double complex b[restrict N],
	      double complex c[restrict N]) __attribute__ ((noinline));
void subconj (double complex a[restrict N], double complex b[restrict N],
	      double complex c[restrict N]) __attribute__ ((noinline));
void mul (double complex a[restrict N], double complex b[restrict N],
	  double complex c[restrict N]) __attribute__ ((noinline));
void mulconj (double complex a[restrict N], double complex b[restrict N],
	      double complex c[restrict N]) __attribute__ ((noinline));
void neg (double complex a[restrict N], double complex b[restrict N])
  __attribute__ ((noinline));
void conjugate (double complex a[restrict N], double complex b[restrict N])
  __attribute__ ((noinline));

void
add (double complex a[restrict N], double complex b[restrict N],
     double complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}

void
sub (double complex a[restrict N], double complex b[restrict N],
     double complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - b[i];
}

void
addconj (double complex a[restrict N], double complex b[restrict N],
	 double complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + ~b[i];
}

void
subconj (double complex a[restrict N], double complex b[restrict N],
	 double complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - ~b[i];
}

void
mul (double complex a[restrict N], double complex b[restrict N],
     double complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * b[i];
}

void
mulconj (double complex a[restrict N], double complex b[restrict N],
	 double complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * ~b[i];
}

void
neg (double complex a[restrict N], double complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = -a[i];
}

void
conjugate (double complex a[restrict N], double complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = ~a[i];
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

  double complex a[N];
  double complex b[N];
  double complex res[N];

  double complex addref[N];
  double complex subref[N];
  double complex addconjref[N];
  double complex subconjref[N];
  double complex mulref[N];
  double complex mulconjref[N];
  double complex negref[N];
  double complex conjugateref[N];

  for (int i = 0; i < N; i++)
    {
      double ar = (double) rand ();
      double ai = (double) rand ();
      double br = (double) rand ();
      double bi = (double) rand ();

      a[i] = ar + ai * I;
      b[i] = br + bi * I;

      double tmpr = ar + br;
      double tmpi = ai + bi;
      addref[i] = tmpr + tmpi * I;

      tmpr = ar - br;
      tmpi = ai - bi;
      subref[i] = tmpr + tmpi * I;

      tmpr = ar + br;
      tmpi = ai - bi;
      addconjref[i] = tmpr + tmpi * I;

      tmpr = ar - br;
      tmpi = ai + bi;
      subconjref[i] = tmpr + tmpi * I;

      tmpr = ar * br - ai * bi;
      tmpi = ai * br + ar * bi;
      mulref[i] = tmpr + tmpi * I;

      tmpr = ar * br + ai * bi;
      tmpi = ai * br - ar * bi;
      mulconjref[i] = tmpr + tmpi * I;

      tmpr = -ar;
      tmpi = -ai;
      negref[i] = tmpr + tmpi * I;

      tmpr = ar;
      tmpi = -ai;
      conjugateref[i] = tmpr + tmpi * I;
    }

  add (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], addref[i]))
      return 1;

  sub (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], subref[i]))
      return 1;

  addconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], addconjref[i]))
      return 1;

  subconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], subconjref[i]))
      return 1;

  mul (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], mulref[i]))
      return 1;

  mulconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], mulconjref[i]))
      return 1;

  neg (a, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], negref[i]))
      return 1;

  conjugate (a, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], conjugateref[i]))
      return 1;

  return 0;
}
