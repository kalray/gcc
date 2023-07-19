#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((long complex) I)

void add (long complex a[restrict N], long complex b[restrict N],
	  long complex c[restrict N]) __attribute__ ((noinline));
void sub (long complex a[restrict N], long complex b[restrict N],
	  long complex c[restrict N]) __attribute__ ((noinline));
void addconj (long complex a[restrict N], long complex b[restrict N],
	      long complex c[restrict N]) __attribute__ ((noinline));
void subconj (long complex a[restrict N], long complex b[restrict N],
	      long complex c[restrict N]) __attribute__ ((noinline));
void mul (long complex a[restrict N], long complex b[restrict N],
	  long complex c[restrict N]) __attribute__ ((noinline));
void mulconj (long complex a[restrict N], long complex b[restrict N],
	      long complex c[restrict N]) __attribute__ ((noinline));
void neg (long complex a[restrict N], long complex b[restrict N])
  __attribute__ ((noinline));
void conjugate (long complex a[restrict N], long complex b[restrict N])
  __attribute__ ((noinline));

void
add (long complex a[restrict N], long complex b[restrict N],
     long complex c[restrict N])
{
  for (long i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}

void
sub (long complex a[restrict N], long complex b[restrict N],
     long complex c[restrict N])
{
  for (long i = 0; i < N; i++)
    c[i] = a[i] - b[i];
}

void
addconj (long complex a[restrict N], long complex b[restrict N],
	 long complex c[restrict N])
{
  for (long i = 0; i < N; i++)
    c[i] = a[i] + ~b[i];
}

void
subconj (long complex a[restrict N], long complex b[restrict N],
	 long complex c[restrict N])
{
  for (long i = 0; i < N; i++)
    c[i] = a[i] - ~b[i];
}

void
mul (long complex a[restrict N], long complex b[restrict N],
     long complex c[restrict N])
{
  for (long i = 0; i < N; i++)
    c[i] = a[i] * b[i];
}

void
mulconj (long complex a[restrict N], long complex b[restrict N],
	 long complex c[restrict N])
{
  for (long i = 0; i < N; i++)
    c[i] = a[i] * ~b[i];
}

void
neg (long complex a[restrict N], long complex b[restrict N])
{
  for (long i = 0; i < N; i++)
    b[i] = -a[i];
}

void
conjugate (long complex a[restrict N], long complex b[restrict N])
{
  for (long i = 0; i < N; i++)
    b[i] = ~a[i];
}


int
main ()
{
  srand (0xbeef);

  long complex a[N];
  long complex b[N];
  long complex res[N];

  long complex addref[N];
  long complex subref[N];
  long complex addconjref[N];
  long complex subconjref[N];
  long complex mulref[N];
  long complex mulconjref[N];
  long complex negref[N];
  long complex conjugateref[N];

  for (long i = 0; i < N; i++)
    {
      long ar = rand ();
      long ai = rand ();
      long br = rand ();
      long bi = rand ();

      a[i] = ar + ai * II;
      b[i] = br + bi * II;

      long tmpr = ar + br;
      long tmpi = ai + bi;
      addref[i] = tmpr + tmpi * II;

      tmpr = ar - br;
      tmpi = ai - bi;
      subref[i] = tmpr + tmpi * II;

      tmpr = ar + br;
      tmpi = ai - bi;
      addconjref[i] = tmpr + tmpi * II;

      tmpr = ar - br;
      tmpi = ai + bi;
      subconjref[i] = tmpr + tmpi * II;

      tmpr = ar * br - ai * bi;
      tmpi = ai * br + ar * bi;
      mulref[i] = tmpr + tmpi * II;

      tmpr = ar * br + ai * bi;
      tmpi = ai * br - ar * bi;
      mulconjref[i] = tmpr + tmpi * II;

      tmpr = -ar;
      tmpi = -ai;
      negref[i] = tmpr + tmpi * II;

      tmpr = ar;
      tmpi = -ai;
      conjugateref[i] = tmpr + tmpi * II;
    }

  add (a, b, res);
  for (long i = 0; i < N; i++)
    if (res[i] != addref[i])
      return 1;

  sub (a, b, res);
  for (long i = 0; i < N; i++)
    if (res[i] != subref[i])
      return 1;

  addconj (a, b, res);
  for (long i = 0; i < N; i++)
    if (res[i] != addconjref[i])
      return 1;

  subconj (a, b, res);
  for (long i = 0; i < N; i++)
    if (res[i] != subconjref[i])
      return 1;

  mul (a, b, res);
  for (long i = 0; i < N; i++)
    if (res[i] != mulref[i])
      return 1;

  mulconj (a, b, res);
  for (long i = 0; i < N; i++)
    if (res[i] != mulconjref[i])
      return 1;

  neg (a, res);
  for (long i = 0; i < N; i++)
    if (res[i] != negref[i])
      return 1;

  conjugate (a, res);
  for (long i = 0; i < N; i++)
    if (res[i] != conjugateref[i])
      return 1;

  return 0;
}
