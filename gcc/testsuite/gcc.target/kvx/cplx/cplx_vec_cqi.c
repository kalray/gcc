#include <complex.h>
#include <time.h>
#include <stdlib.h>

#define N 64
#define II ((char complex) I)

void add (char complex a[restrict N], char complex b[restrict N],
	  char complex c[restrict N]) __attribute__ ((noinline));
void sub (char complex a[restrict N], char complex b[restrict N],
	  char complex c[restrict N]) __attribute__ ((noinline));
void addconj (char complex a[restrict N], char complex b[restrict N],
	      char complex c[restrict N]) __attribute__ ((noinline));
void subconj (char complex a[restrict N], char complex b[restrict N],
	      char complex c[restrict N]) __attribute__ ((noinline));
void mul (char complex a[restrict N], char complex b[restrict N],
	  char complex c[restrict N]) __attribute__ ((noinline));
void mulconj (char complex a[restrict N], char complex b[restrict N],
	      char complex c[restrict N]) __attribute__ ((noinline));
void neg (char complex a[restrict N], char complex b[restrict N])
  __attribute__ ((noinline));
void conjugate (char complex a[restrict N], char complex b[restrict N])
  __attribute__ ((noinline));

void
add (char complex a[restrict N], char complex b[restrict N],
     char complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}

void
sub (char complex a[restrict N], char complex b[restrict N],
     char complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - b[i];
}

void
addconj (char complex a[restrict N], char complex b[restrict N],
	 char complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + ~b[i];
}

void
subconj (char complex a[restrict N], char complex b[restrict N],
	 char complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - ~b[i];
}

void
mul (char complex a[restrict N], char complex b[restrict N],
     char complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * b[i];
}

void
mulconj (char complex a[restrict N], char complex b[restrict N],
	 char complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * ~b[i];
}

void
neg (char complex a[restrict N], char complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = -a[i];
}

void
conjugate (char complex a[restrict N], char complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = ~a[i];
}


int
main ()
{
  srand (0xbeef);

  char complex a[N];
  char complex b[N];
  char complex res[N];

  char complex addref[N];
  char complex subref[N];
  char complex addconjref[N];
  char complex subconjref[N];
  char complex mulref[N];
  char complex mulconjref[N];
  char complex negref[N];
  char complex conjugateref[N];

  for (int i = 0; i < N; i++)
    {
      char ar = rand ();
      char ai = rand ();
      char br = rand ();
      char bi = rand ();

      a[i] = ar + ai * II;
      b[i] = br + bi * II;

      char tmpr = ar + br;
      char tmpi = ai + bi;
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
