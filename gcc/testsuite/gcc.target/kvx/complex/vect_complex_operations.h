#include <complex.h>

void vadd (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	   INNER_TYPE complex c[restrict N]) __attribute__ ((noinline));
void vsub (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	   INNER_TYPE complex c[restrict N]) __attribute__ ((noinline));
void vaddconj (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	       INNER_TYPE complex c[restrict N]) __attribute__ ((noinline));
void vsubconj (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	       INNER_TYPE complex c[restrict N]) __attribute__ ((noinline));
void vmul (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	   INNER_TYPE complex c[restrict N]) __attribute__ ((noinline));
void vmulconj (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	       INNER_TYPE complex c[restrict N]) __attribute__ ((noinline));
void vneg (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N])
  __attribute__ ((noinline));
void vconjugate (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N])
  __attribute__ ((noinline));
void vfmacplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	       INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
  __attribute__ ((noinline));
void vfmscplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	       INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
  __attribute__ ((noinline));
void vfmaconjcplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
		   INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
  __attribute__ ((noinline));
void vfmsconjcplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
		   INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
  __attribute__ ((noinline));
void vcst (INNER_TYPE complex a[restrict N]) __attribute__ ((noinline));
void vload (INNER_TYPE complex a[restrict N]) __attribute__ ((noinline));
void vstore (INNER_TYPE complex a[restrict N]) __attribute__ ((noinline));

volatile INNER_TYPE complex values[N];

void
vadd (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
      INNER_TYPE complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + b[i];
}

void
vsub (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
      INNER_TYPE complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - b[i];
}

void
vaddconj (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	  INNER_TYPE complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] + ~b[i];
}

void
vsubconj (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	  INNER_TYPE complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] - ~b[i];
}

void
vmul (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
      INNER_TYPE complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * b[i];
}

void
vmulconj (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	  INNER_TYPE complex c[restrict N])
{
  for (int i = 0; i < N; i++)
    c[i] = a[i] * ~b[i];
}

void
vneg (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = -a[i];
}

void
vconjugate (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N])
{
  for (int i = 0; i < N; i++)
    b[i] = ~a[i];
}

void
vfmacplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	  INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] + a[i] * b[i];
}

void
vfmscplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	  INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] - a[i] * b[i];
}

void
vfmaconjcplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	      INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] + a[i] * (~b[i]);
}

void
vfmsconjcplx (INNER_TYPE complex a[restrict N], INNER_TYPE complex b[restrict N],
	      INNER_TYPE complex c[restrict N], INNER_TYPE complex d[restrict N])
{
  for (int i = 0; i < N; i++)
    d[i] = c[i] - a[i] * (~b[i]);
}

void
vcst (INNER_TYPE complex a[restrict N])
{
  for (int i = 0; i < N; i++)
    a[i] = ((INNER_TYPE) 4.0) + ((INNER_TYPE) 2.0) * ((INNER_TYPE complex) I);
}

void
vload (INNER_TYPE complex a[restrict N])
{
  for (int i = 0; i < N; i++)
    a[i] = values[i];
}

void
vstore (INNER_TYPE complex a[restrict N])
{
  for (int i = 0; i < N; i++)
    values[i] = a[i];
}
