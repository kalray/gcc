#include <complex.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define EPSILON 0.01
#define N 64

void add (float complex a[restrict N], float complex b[restrict N],
	  float complex c[restrict N]) __attribute__ ((noinline));
void sub (float complex a[restrict N], float complex b[restrict N],
	  float complex c[restrict N]) __attribute__ ((noinline));
void addconj (float complex a[restrict N], float complex b[restrict N],
	      float complex c[restrict N]) __attribute__ ((noinline));
void subconj (float complex a[restrict N], float complex b[restrict N],
	      float complex c[restrict N]) __attribute__ ((noinline));
void mul (float complex a[restrict N], float complex b[restrict N],
	  float complex c[restrict N]) __attribute__ ((noinline));
void mulconj (float complex a[restrict N], float complex b[restrict N],
	      float complex c[restrict N]) __attribute__ ((noinline));
void neg (float complex a[restrict N], float complex b[restrict N])
  __attribute__ ((noinline));
void conjugate (float complex a[restrict N], float complex b[restrict N])
  __attribute__ ((noinline));
void fmacplx (float complex a[restrict N], float complex b[restrict N],
	      float complex c[restrict N], float complex d[restrict N])
  __attribute__ ((noinline));
void fmscplx (float complex a[restrict N], float complex b[restrict N],
	      float complex c[restrict N], float complex d[restrict N])
  __attribute__ ((noinline));
void fmaconjcplx (float complex a[restrict N], float complex b[restrict N],
		  float complex c[restrict N], float complex d[restrict N])
  __attribute__ ((noinline));
void fmsconjcplx (float complex a[restrict N], float complex b[restrict N],
		  float complex c[restrict N], float complex d[restrict N])
  __attribute__ ((noinline));

void
add (float complex a[restrict N], float complex b[restrict N],
     float complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}

void
sub (float complex a[restrict N], float complex b[restrict N],
     float complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - b[i];
}

void
addconj (float complex a[restrict N], float complex b[restrict N],
	 float complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + ~b[i];
}

void
subconj (float complex a[restrict N], float complex b[restrict N],
	 float complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - ~b[i];
}

void
mul (float complex a[restrict N], float complex b[restrict N],
     float complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * b[i];
}

void
mulconj (float complex a[restrict N], float complex b[restrict N],
	 float complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * ~b[i];
}

void
neg (float complex a[restrict N], float complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = -a[i];
}

void
conjugate (float complex a[restrict N], float complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = ~a[i];
}

void
fmacplx (float complex a[restrict N], float complex b[restrict N],
	 float complex c[restrict N], float complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] + a[i] * b[i];
}

void
fmscplx (float complex a[restrict N], float complex b[restrict N],
	 float complex c[restrict N], float complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] - a[i] * b[i];
}

void
fmaconjcplx (float complex a[restrict N], float complex b[restrict N],
	     float complex c[restrict N], float complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] + a[i] * (~b[i]);
}

void
fmsconjcplx (float complex a[restrict N], float complex b[restrict N],
	     float complex c[restrict N], float complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] - a[i] * (~b[i]);
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

  float complex a[N];
  float complex b[N];
  float complex c[N];
  float complex res[N];

  float complex addref[N];
  float complex subref[N];
  float complex addconjref[N];
  float complex subconjref[N];
  float complex mulref[N];
  float complex mulconjref[N];
  float complex negref[N];
  float complex conjugateref[N];
  float complex fmaref[N];
  float complex fmsref[N];
  float complex fmaconjref[N];
  float complex fmsconjref[N];

  for (int i = 0; i < N; i++)
    {
      float ar = (float) rand ();
      float ai = (float) rand ();
      float br = (float) rand ();
      float bi = (float) rand ();
      float cr = (float) rand ();
      float ci = (float) rand ();

      a[i] = ar + ai * I;
      b[i] = br + bi * I;
      c[i] = cr + ci * I;

      float tmpr = ar + br;
      float tmpi = ai + bi;
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

      tmpr = cr + crealf (mulref[i]);
      tmpi = ci + cimagf (mulref[i]);
      fmaref[i] = tmpr + tmpi * I;

      tmpr = cr - crealf (mulref[i]);
      tmpi = ci - cimagf (mulref[i]);
      fmsref[i] = tmpr + tmpi * I;

      tmpr = cr + crealf (mulconjref[i]);
      tmpi = ci + cimagf (mulconjref[i]);
      fmaconjref[i] = tmpr + tmpi * I;

      tmpr = cr - crealf (mulconjref[i]);
      tmpi = ci - cimagf (mulconjref[i]);
      fmsconjref[i] = tmpr + tmpi * I;
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

  fmacplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmaref[i]))
      return 1;

  fmscplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmsref[i]))
      return 1;

  fmaconjcplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmaconjref[i]))
      return 1;

  fmsconjcplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmsconjref[i]))
      return 1;

  return 0;
}
