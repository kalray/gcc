#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((short complex) I)

void add (short complex a[restrict N], short complex b[restrict N],
	  short complex c[restrict N]) __attribute__ ((noinline));
void sub (short complex a[restrict N], short complex b[restrict N],
	  short complex c[restrict N]) __attribute__ ((noinline));
void addconj (short complex a[restrict N], short complex b[restrict N],
	      short complex c[restrict N]) __attribute__ ((noinline));
void subconj (short complex a[restrict N], short complex b[restrict N],
	      short complex c[restrict N]) __attribute__ ((noinline));
void mul (short complex a[restrict N], short complex b[restrict N],
	  short complex c[restrict N]) __attribute__ ((noinline));
void mulconj (short complex a[restrict N], short complex b[restrict N],
	      short complex c[restrict N]) __attribute__ ((noinline));
void neg (short complex a[restrict N], short complex b[restrict N])
  __attribute__ ((noinline));
void conjugate (short complex a[restrict N], short complex b[restrict N])
  __attribute__ ((noinline));

void
add (short complex a[restrict N], short complex b[restrict N],
     short complex c[restrict N])
{
  for (short i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}

void
sub (short complex a[restrict N], short complex b[restrict N],
     short complex c[restrict N])
{
  for (short i = 0; i < N; i++)
    c[i] = a[i] - b[i];
}

void
addconj (short complex a[restrict N], short complex b[restrict N],
	 short complex c[restrict N])
{
  for (short i = 0; i < N; i++)
    c[i] = a[i] + ~b[i];
}

void
subconj (short complex a[restrict N], short complex b[restrict N],
	 short complex c[restrict N])
{
  for (short i = 0; i < N; i++)
    c[i] = a[i] - ~b[i];
}

void
mul (short complex a[restrict N], short complex b[restrict N],
     short complex c[restrict N])
{
  for (short i = 0; i < N; i++)
    c[i] = a[i] * b[i];
}

void
mulconj (short complex a[restrict N], short complex b[restrict N],
	 short complex c[restrict N])
{
  for (short i = 0; i < N; i++)
    c[i] = a[i] * ~b[i];
}

void
neg (short complex a[restrict N], short complex b[restrict N])
{
  for (short i = 0; i < N; i++)
    b[i] = -a[i];
}

void
conjugate (short complex a[restrict N], short complex b[restrict N])
{
  for (short i = 0; i < N; i++)
    b[i] = ~a[i];
}


int
main ()
{
  srand (0xbeef);

  short complex a[N];
  short complex b[N];
  short complex res[N];

  short complex addref[N];
  short complex subref[N];
  short complex addconjref[N];
  short complex subconjref[N];
  short complex mulref[N];
  short complex mulconjref[N];
  short complex negref[N];
  short complex conjugateref[N];

  for (short i = 0; i < N; i++)
    {
      short ar = rand ();
      short ai = rand ();
      short br = rand ();
      short bi = rand ();

      a[i] = ar + ai * II;
      b[i] = br + bi * II;

      short tmpr = ar + br;
      short tmpi = ai + bi;
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
  for (short i = 0; i < N; i++)
    if (res[i] != addref[i])
      return 1;

  sub (a, b, res);
  for (short i = 0; i < N; i++)
    if (res[i] != subref[i])
      return 1;

  addconj (a, b, res);
  for (short i = 0; i < N; i++)
    if (res[i] != addconjref[i])
      return 1;

  subconj (a, b, res);
  for (short i = 0; i < N; i++)
    if (res[i] != subconjref[i])
      return 1;

  mul (a, b, res);
  for (short i = 0; i < N; i++)
    if (res[i] != mulref[i])
      return 1;

  mulconj (a, b, res);
  for (short i = 0; i < N; i++)
    if (res[i] != mulconjref[i])
      return 1;

  neg (a, res);
  for (short i = 0; i < N; i++)
    if (res[i] != negref[i])
      return 1;

  conjugate (a, res);
  for (short i = 0; i < N; i++)
    if (res[i] != conjugateref[i])
      return 1;

  return 0;
}
