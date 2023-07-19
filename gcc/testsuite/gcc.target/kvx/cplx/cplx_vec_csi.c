#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((int complex) I)

void add (int complex a[restrict N], int complex b[restrict N],
	  int complex c[restrict N]) __attribute__ ((noinline));
void sub (int complex a[restrict N], int complex b[restrict N],
	  int complex c[restrict N]) __attribute__ ((noinline));
void addconj (int complex a[restrict N], int complex b[restrict N],
	      int complex c[restrict N]) __attribute__ ((noinline));
void subconj (int complex a[restrict N], int complex b[restrict N],
	      int complex c[restrict N]) __attribute__ ((noinline));
void mul (int complex a[restrict N], int complex b[restrict N],
	  int complex c[restrict N]) __attribute__ ((noinline));
void mulconj (int complex a[restrict N], int complex b[restrict N],
	      int complex c[restrict N]) __attribute__ ((noinline));
void neg (int complex a[restrict N], int complex b[restrict N])
  __attribute__ ((noinline));
void conjugate (int complex a[restrict N], int complex b[restrict N])
  __attribute__ ((noinline));

void
add (int complex a[restrict N], int complex b[restrict N],
     int complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}

void
sub (int complex a[restrict N], int complex b[restrict N],
     int complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - b[i];
}

void
addconj (int complex a[restrict N], int complex b[restrict N],
	 int complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + ~b[i];
}

void
subconj (int complex a[restrict N], int complex b[restrict N],
	 int complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - ~b[i];
}

void
mul (int complex a[restrict N], int complex b[restrict N],
     int complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * b[i];
}

void
mulconj (int complex a[restrict N], int complex b[restrict N],
	 int complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * ~b[i];
}

void
neg (int complex a[restrict N], int complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = -a[i];
}

void
conjugate (int complex a[restrict N], int complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = ~a[i];
}


int
main ()
{
  srand (0xbeef);

  int complex a[N];
  int complex b[N];
  int complex res[N];

  int complex addref[N];
  int complex subref[N];
  int complex addconjref[N];
  int complex subconjref[N];
  int complex mulref[N];
  int complex mulconjref[N];
  int complex negref[N];
  int complex conjugateref[N];

  for (int i = 0; i < N; i++)
    {
      int ar = rand ();
      int ai = rand ();
      int br = rand ();
      int bi = rand ();

      a[i] = ar + ai * II;
      b[i] = br + bi * II;

      int tmpr = ar + br;
      int tmpi = ai + bi;
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
  for (int i = 0; i < N; i++)
    if (res[i] != addref[i])
      return 1;

  sub (a, b, res);
  for (int i = 0; i < N; i++)
    if (res[i] != subref[i])
      return 1;

  addconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (res[i] != addconjref[i])
      return 1;

  subconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (res[i] != subconjref[i])
      return 1;

  mul (a, b, res);
  for (int i = 0; i < N; i++)
    if (res[i] != mulref[i])
      return 1;

  mulconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (res[i] != mulconjref[i])
      return 1;

  neg (a, res);
  for (int i = 0; i < N; i++)
    if (res[i] != negref[i])
      return 1;

  conjugate (a, res);
  for (int i = 0; i < N; i++)
    if (res[i] != conjugateref[i])
      return 1;

  return 0;
}
