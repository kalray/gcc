/* {dg-do run} */

#include <complex.h>

char complex cstInt8 () __attribute__ ((noinline));
short complex cstInt16 () __attribute__ ((noinline));
int complex cstInt32 () __attribute__ ((noinline));
long complex cstInt64 () __attribute__ ((noinline));
float complex cstFloat32 () __attribute__ ((noinline));
double complex cstFloat64 () __attribute__ ((noinline));

char complex
cstInt8 ()
{
  return 3 + 4 * I;
}


short complex
cstInt16 ()
{
  return 3 + 4 * I;
}


int complex
cstInt32 ()
{
  return 3 + 4 * I;
}


long complex
cstInt64 ()
{
  return 3 + 4 * I;
}


float complex
cstFloat32 ()
{
  return 3.0 + 4.0 * I;
}


double complex
cstFloat64 ()
{
  return 3.0 + 4.0 * I;
}

int
main ()
{
  char complex r_i8 = 3 + 4 * I;
  if (cstInt8 () != r_i8)
    return 1;

  short complex r_i16 = 3 + 4 * I;
  if (cstInt16 () != r_i16)
    return 1;

  int complex r_i32 = 3 + 4 * I;
  if (cstInt32 () != r_i32)
    return 1;

  long complex r_i64 = 3 + 4 * I;
  if (cstInt64 () != r_i64)
    return 1;

  float complex r_f32 = 3.0 + 4.0 * I;
  if (cstFloat32 () != r_f32)
    return 1;

  double complex r_f64 = 3.0 + 4.0 * I;
  if (cstFloat64 () != r_f64)
    return 1;

  return 0;
}
