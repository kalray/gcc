/* { dg-do  compile } */
/* { dg-options "-O2 -fno-tree-sink -S" } */

int
cmovesi_wnez (int a, int b, int c)
{
  a++, b--;
  return c ? a : b;
}

long long
cmovedi_deqz (long long a, long long b, long long c)
{
  a++, b--;
  return !c ? a : b;
}

long long
cmovedi_wgtz (long long a, long long b, int c)
{
  a++, b--;
  return c > 0 ? a : b;
}

float
cmovesf_dlez (float a, float b, long long c)
{
  a++, b--;
  return c <= 0 ? a : b;
}

float
cmovesf_wgez (float a, float b, int c)
{
  a++, b--;
  return c >= 0 ? a : b;
}

float
cmovesf_fgez (float a, float b, float c)
{
  a++, b--;
  return c >= 0.0 ? a : b;
}

double
cmovedf_dltz (double a, double b, long long c)
{
  a++, b--;
  return c < 0 ? a : b;
}

double
cmovedf_wodd (double a, double b, int c)
{
  a++, b--;
  return c & 1 ? a : b;
}

double
cmovedf_deven (double a, double b, long long c)
{
  a++, b--;
  return !(c & 1) ? a : b;
}

typedef float float32_t;
typedef float32_t float32x2_t
  __attribute__ ((vector_size (2 * sizeof (float32_t))));

float32x2_t
cmovev2sf_wodd (float32x2_t a, float32x2_t b, int c)
{
  a++, b--;
  return (c & 1) ? a : b;
}

typedef float32_t float32x4_t
  __attribute__ ((vector_size (4 * sizeof (float32_t))));

float32x4_t
cmovev4sf_deven (float32x4_t a, float32x4_t b, long long c)
{
  a++, b--;
  return !(c & 1) ? a : b;
}

typedef long long int64_t;
typedef int64_t int64x2_t __attribute__ ((vector_size (2 * sizeof (int64_t))));

int64x2_t
cmovev2di_wodd (int64x2_t a, int64x2_t b, int c)
{
  a++, b--;
  return (c & 1) ? a : b;
}

typedef int64_t int64x4_t __attribute__ ((vector_size (4 * sizeof (int64_t))));

int64x4_t
cmovev4di_deven (int64x4_t a, int64x4_t b, long long c)
{
  a++, b--;
  return !(c & 1) ? a : b;
}

/* { dg-final { scan-assembler-times "cmoved\\." 18 } } */

