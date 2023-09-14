#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 64
#include "complex_operations.h"
#include "vect_complex_operations.h"

#define II ((INNER_TYPE complex) I)
#define CONCAT2_(a,b) a ## b
#define CONCAT2(a,b) CONCAT2_(a,b)
#define TYPE_float   1
#define TYPE_double  1
#define TYPE_char    0
#define TYPE_short   0
#define TYPE_int     0
#define TYPE_long    0
#define xTYPE        CONCAT2(TYPE_,INNER_TYPE)

bool
check (INNER_TYPE complex a, INNER_TYPE complex b)
{
#if xTYPE == 1
#define EPSILON 0.01
  if ((__real__ (a) / __real__ (b)) < (1 - EPSILON))
    return false;
  if ((__real__ (a) / __real__ (b)) > (1 + EPSILON))
    return false;
  if ((__imag__ (a) / __imag__ (b)) < (1 - EPSILON))
    return false;
  if ((__imag__ (a) / __imag__ (b)) > (1 + EPSILON))
    return false;
  return true;
#else
  return (__real__ (a) == __real__ (b)) && (__imag__ (a) == __imag__ (b));
#endif
}

int
test_complex ()
{
  for (int i = 0; i < N; i++)
    {
      INNER_TYPE ar = rand ();
      INNER_TYPE ai = rand ();
      INNER_TYPE br = rand ();
      INNER_TYPE bi = rand ();
      INNER_TYPE cr = rand ();
      INNER_TYPE ci = rand ();

      INNER_TYPE complex a = ar + ai * II;
      INNER_TYPE complex b = br + bi * II;
      INNER_TYPE complex c = cr + ci * II;

      INNER_TYPE tmpr = ar + br;
      INNER_TYPE tmpi = ai + bi;
      INNER_TYPE complex ref = tmpr + tmpi * II;
      if (!check (add (a, b), ref))
	return 1;

      tmpr = ar - br;
      tmpi = ai - bi;
      ref = tmpr + tmpi * II;
      if (!check (sub (a, b), ref))
	return 2;

      tmpr = ar + br;
      tmpi = ai - bi;
      ref = tmpr + tmpi * II;
      if (!check (addconj (a, b), ref))
	return 3;

      tmpr = ar - br;
      tmpi = ai + bi;
      ref = tmpr + tmpi * II;
      if (!check (subconj (a, b), ref))
	return 4;

      tmpr = ar * br - ai * bi;
      tmpi = ai * br + ar * bi;
      ref = tmpr + tmpi * II;
      if (!check (mul (a, b), ref))
	return 5;

      tmpr = ar * br + ai * bi;
      tmpi = ai * br - ar * bi;
      ref = tmpr + tmpi * II;
      if (!check (mulconj (a, b), ref))
	return 6;

      tmpr = -ar;
      tmpi = -ai;
      ref = tmpr + tmpi * II;
      if (!check (neg (a), ref))
	return 7;

      tmpr = ar;
      tmpi = -ai;
      ref = tmpr + tmpi * II;
      if (!check (conjugate (a), ref))
	return 8;

      tmpr = cr + (ar * br - ai * bi);
      tmpi = ci + (ai * br + ar * bi);
      ref = tmpr + tmpi * II;
      if (!check (fmacplx (a, b, c), ref))
	return 9;

      tmpr = cr - (ar * br - ai * bi);
      tmpi = ci - (ai * br + ar * bi);
      ref = tmpr + tmpi * II;
      if (!check (fmscplx (a, b, c), ref))
	return 10;

      tmpr = cr + (ar * br + ai * bi);
      tmpi = ci + (ai * br - ar * bi);
      ref = tmpr + tmpi * II;
      if (!check (fmaconjcplx (a, b, c), ref))
	return 11;

      tmpr = cr - (ar * br + ai * bi);
      tmpi = ci - (ai * br - ar * bi);
      ref = tmpr + tmpi * II;
      if (!check (fmsconjcplx (a, b, c), ref))
	return 12;

      ref = ((INNER_TYPE) 4.0) + ((INNER_TYPE) 2.0) * II;
      if (!check (cst (), ref))
	return 13;

      /*tmpr = (INNER_TYPE) 1.0;
      tmpi = (INNER_TYPE) 2.0;
      ref = tmpr + tmpi * II;
      if (!check (load (), ref))
	return 14;
      store (a);
      if (!check (load (), a))
	return 15;*/
    }

  return 0;
}
  
int
test_vect_complex ()
{
  INNER_TYPE complex a[N];
  INNER_TYPE complex b[N];
  INNER_TYPE complex c[N];
  INNER_TYPE complex res[N];

  INNER_TYPE complex addref[N];
  INNER_TYPE complex subref[N];
  INNER_TYPE complex addconjref[N];
  INNER_TYPE complex subconjref[N];
  INNER_TYPE complex mulref[N];
  INNER_TYPE complex mulconjref[N];
  INNER_TYPE complex negref[N];
  INNER_TYPE complex conjugateref[N];
  INNER_TYPE complex fmaref[N];
  INNER_TYPE complex fmsref[N];
  INNER_TYPE complex fmaconjref[N];
  INNER_TYPE complex fmsconjref[N];
  INNER_TYPE complex ref;

  for (int i = 0; i < N; i++)
    {
      INNER_TYPE ar = (INNER_TYPE) rand ();
      INNER_TYPE ai = (INNER_TYPE) rand ();
      INNER_TYPE br = (INNER_TYPE) rand ();
      INNER_TYPE bi = (INNER_TYPE) rand ();
      INNER_TYPE cr = (INNER_TYPE) rand ();
      INNER_TYPE ci = (INNER_TYPE) rand ();

      a[i] = ar + ai * II;
      b[i] = br + bi * II;
      c[i] = cr + ci * II;

      INNER_TYPE tmpr = ar + br;
      INNER_TYPE tmpi = ai + bi;
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

      tmpr = cr + __real__ (mulref[i]);
      tmpi = ci + __imag__ (mulref[i]);
      fmaref[i] = tmpr + tmpi * II;

      tmpr = cr - __real__ (mulref[i]);
      tmpi = ci - __imag__ (mulref[i]);
      fmsref[i] = tmpr + tmpi * II;

      tmpr = cr + __real__ (mulconjref[i]);
      tmpi = ci + __imag__ (mulconjref[i]);
      fmaconjref[i] = tmpr + tmpi * II;

      tmpr = cr - __real__ (mulconjref[i]);
      tmpi = ci - __imag__ (mulconjref[i]);
      fmsconjref[i] = tmpr + tmpi * II;
    }

  vadd (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], addref[i]))
      return 100;

  vsub (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], subref[i]))
      return 101;

  vaddconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], addconjref[i]))
      return 102;

  vsubconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], subconjref[i]))
      return 103;

  vmul (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], mulref[i]))
      return 104;

  vmulconj (a, b, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], mulconjref[i]))
      return 105;

  vneg (a, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], negref[i]))
      return 106;

  vconjugate (a, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], conjugateref[i]))
      return 107;

  vfmacplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmaref[i]))
      return 108;

  vfmscplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmsref[i]))
      return 109;

  vfmaconjcplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmaconjref[i]))
      return 110;

  vfmsconjcplx (a, b, c, res);
  for (int i = 0; i < N; i++)
    if (!check (res[i], fmsconjref[i]))
      return 111;

  vcst (res);
  ref = ((INNER_TYPE) 4.0) + ((INNER_TYPE) 2.0) * II;
  for (int i = 0; i < N; i++)
    if (!check (res[i], ref))
      return 112;

  vstore (a);
  vload (res);
  for (int i = 0; i < N; i++)
  if (!check (res[i], a[i]))
    return 113;
  vstore (b);
  vload (res);
  for (int i = 0; i < N; i++)
  if (!check (res[i], b[i]))
    return 114;

  return 0;
}

int 
main()
{
  srand (0xbeef);

  int res1 = test_complex ();
  return (res1) ? res1 : test_vect_complex ();
}
