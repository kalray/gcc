#include <complex.h>

INNER_TYPE complex add (INNER_TYPE complex a, INNER_TYPE complex b)
  __attribute__ ((noinline));
INNER_TYPE complex sub (INNER_TYPE complex a, INNER_TYPE complex b)
  __attribute__ ((noinline));
INNER_TYPE complex addconj (INNER_TYPE complex a, INNER_TYPE complex b)
  __attribute__ ((noinline));
INNER_TYPE complex subconj (INNER_TYPE complex a, INNER_TYPE complex b)
  __attribute__ ((noinline));
INNER_TYPE complex mul (INNER_TYPE complex a, INNER_TYPE complex b)
  __attribute__ ((noinline));
INNER_TYPE complex mulconj (INNER_TYPE complex a, INNER_TYPE complex b)
  __attribute__ ((noinline));
INNER_TYPE complex neg (INNER_TYPE complex a) __attribute__ ((noinline));
INNER_TYPE complex conjugate (INNER_TYPE complex a) __attribute__ ((noinline));
INNER_TYPE complex fmacplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
  __attribute__ ((noinline));
INNER_TYPE complex fmscplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
  __attribute__ ((noinline));
INNER_TYPE complex fmaconjcplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
  __attribute__ ((noinline));
INNER_TYPE complex fmsconjcplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
  __attribute__ ((noinline));
INNER_TYPE complex cst () __attribute__ ((noinline));
INNER_TYPE complex load () __attribute__ ((noinline));
void store (INNER_TYPE complex a) __attribute__ ((noinline));

volatile INNER_TYPE complex value = ((INNER_TYPE) 1) + ((INNER_TYPE) 2) * ((INNER_TYPE complex) I);

INNER_TYPE complex
add (INNER_TYPE complex a, INNER_TYPE complex b)
{
  return a + b;
}

INNER_TYPE complex
sub (INNER_TYPE complex a, INNER_TYPE complex b)
{
  return a - b;
}

INNER_TYPE complex
addconj (INNER_TYPE complex a, INNER_TYPE complex b)
{
  return a + (~b);
}

INNER_TYPE complex
subconj (INNER_TYPE complex a, INNER_TYPE complex b)
{
  return a - (~b);
}

INNER_TYPE complex
mul (INNER_TYPE complex a, INNER_TYPE complex b)
{
  return a * b;
}

INNER_TYPE complex
mulconj (INNER_TYPE complex a, INNER_TYPE complex b)
{
  return a * (~b);
}

INNER_TYPE complex
neg (INNER_TYPE complex a)
{
  return -a;
}

INNER_TYPE complex
conjugate (INNER_TYPE complex a)
{
  return ~a;
}

INNER_TYPE complex
fmacplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
{
  return c + a * b;
}

INNER_TYPE complex
fmscplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
{
  return c - a * b;
}

INNER_TYPE complex
fmaconjcplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
{
  return c + a * (~b);
}

INNER_TYPE complex
fmsconjcplx (INNER_TYPE complex a, INNER_TYPE complex b, INNER_TYPE complex c)
{
  return c - a * (~b);
}

INNER_TYPE complex
cst ()
{
  return ((INNER_TYPE) 4.0) + ((INNER_TYPE) 2.0) * ((INNER_TYPE complex) I);
}

INNER_TYPE complex
load ()
{
  return value;
}


void
store (INNER_TYPE complex a)
{
  value = a;
}

