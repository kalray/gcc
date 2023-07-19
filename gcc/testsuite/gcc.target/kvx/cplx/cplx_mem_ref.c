/* {dg-do run} */

#include <complex.h>

char complex loadInt8 () __attribute__ ((noinline));
short complex loadInt16 () __attribute__ ((noinline));
int complex loadInt32 () __attribute__ ((noinline));
long complex loadInt64 () __attribute__ ((noinline));
float complex loadFloat32 () __attribute__ ((noinline));
double complex loadFloat64 () __attribute__ ((noinline));

void storeInt8 (char complex a) __attribute__ ((noinline));
void storeInt16 (short complex a) __attribute__ ((noinline));
void storeInt32 (int complex a) __attribute__ ((noinline));
void storeInt64 (long complex a) __attribute__ ((noinline));
void storeFloat32 (float complex a) __attribute__ ((noinline));
void storeFloat64 (double complex a) __attribute__ ((noinline));

volatile char complex valInt8 = 3 + 4 * I;
volatile short complex valInt16 = 3 + 4 * I;
volatile int complex valInt32 = 3 + 4 * I;
volatile long complex valInt64 = 3 + 4 * I;
volatile float complex valFloat32 = 3.0 + 4.0 * I;
volatile double complex valFloat64 = 3.0 + 4.0 * I;

char complex
loadInt8 ()
{
  return valInt8;
}


short complex
loadInt16 ()
{
  return valInt16;
}


int complex
loadInt32 ()
{
  return valInt32;
}


long complex
loadInt64 ()
{
  return valInt64;
}


float complex
loadFloat32 ()
{
  return valFloat32;
}


double complex
loadFloat64 ()
{
  return valFloat64;
}


void
storeInt8 (char complex a)
{
  return valInt8 = a;
}


void
storeInt16 (short complex a)
{
  return valInt16 = a;
}


void
storeInt32 (int complex a)
{
  return valInt32 = a;
}


void
storeInt64 (long complex a)
{
  return valInt64 = a;
}


void
storeFloat32 (float complex a)
{
  return valFloat32 = a;
}


void
storeFloat64 (double complex a)
{
  return valFloat64 = a;
}

int
main ()
{
  char complex r1_i8 = 3 + 4 * I;
  char complex r2_i8 = 4 + 2 * I;
  if (loadInt8 () != r1_i8)
    return 1;
  storeInt8 (r2_i8);
  if (loadInt8 () != r2_i8)
    return 1;

  short complex r1_i16 = 3 + 4 * I;
  short complex r2_i16 = 4 + 2 * I;
  if (loadInt16 () != r1_i16)
    return 1;
  storeInt16 (r2_i16);
  if (loadInt16 () != r2_i16)
    return 1;

  int complex r1_i32 = 3 + 4 * I;
  int complex r2_i32 = 4 + 2 * I;
  if (loadInt32 () != r1_i32)
    return 1;
  storeInt32 (r2_i32);
  if (loadInt32 () != r2_i32)
    return 1;

  long complex r1_i64 = 3 + 4 * I;
  long complex r2_i64 = 4 + 2 * I;
  if (loadInt64 () != r1_i64)
    return 1;
  storeInt64 (r2_i64);
  if (loadInt64 () != r2_i64)
    return 1;

  float complex r1_f32 = 3.0 + 4.0 * I;
  float complex r2_f32 = 4.0 + 2.0 * I;
  if (loadFloat32 () != r1_f32)
    return 1;
  storeFloat32 (r2_f32);
  if (loadFloat32 () != r2_f32)
    return 1;

  double complex r1_f64 = 3.0 + 4.0 * I;
  double complex r2_f64 = 4.0 + 2.0 * I;
  if (loadFloat64 () != r1_f64)
    return 1;
  storeFloat64 (r2_f64);
  if (loadFloat64 () != r2_f64)
    return 1;

  return 0;
}
