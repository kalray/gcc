/* { dg-do  compile } */
/* { dg-options "-O2 -fno-if-conversion -S" } */

// Branch conditions:
// .DNEZ, .DEQZ, .DLTZ, .DGEZ, .DLEZ, .DGTZ, .ODD, .EVEN,
// .WNEZ, .WEQZ, .WLTZ, .WGEZ, .WLEZ, .WGTZ

long long
branch_dnez (long long a, long long b, long long c)
{
  if (c)
    return ~a;
  return a << b;
}

long long
branch_deqz (long long a, long long b, long long c)
{
  if (!c)
    return ~a;
  return a << b;
}

long long
branch_dltz (long long a, long long b, long long c)
{
  if (c < 0)
    return ~a;
  return a << b;
}

long long
branch_dgez (long long a, long long b, long long c)
{
  if (c >= 0)
    return ~a;
  return a << b;
}

long long
branch_dlez (long long a, long long b, long long c)
{
  if (c <= 0)
    return ~a;
  return a << b;
}

long long
branch_dgtz (long long a, long long b, long long c)
{
  if (c > 0)
    return ~a;
  return a << b;
}

long long
branch_dodd (long long a, long long b, long long c)
{
  if (c & 1)
    return ~a;
  return a << b;
}

long long
branch_deven (long long a, long long b, long long c)
{
  if (!(c & 1))
    return ~a;
  return a << b;
}

long long
branch_wnez (long long a, long long b, int c)
{
  if (c)
    return ~a;
  return a << b;
}

long long
branch_weqz (long long a, long long b, int c)
{
  if (!c)
    return ~a;
  return a << b;
}

long long
branch_wltz (long long a, long long b, int c)
{
  if (c < 0)
    return ~a;
  return a << b;
}

long long
branch_wgez (long long a, long long b, int c)
{
  if (c >= 0)
    return ~a;
  return a << b;
}

long long
branch_wlez (long long a, long long b, int c)
{
  if (c <= 0)
    return ~a;
  return a << b;
}

long long
branch_wgtz (long long a, long long b, int c)
{
  if (c > 0)
    return ~a;
  return a << b;
}

long long
branch_wodd (long long a, long long b, int c)
{
  if (c & 1)
    return ~a;
  return a << b;
}

long long
branch_weven (long long a, long long b, int c)
{
  if (!(c & 1))
    return ~a;
  return a << b;
}

/* { dg-final { scan-assembler-times "cb\\.d" 6 } } */
/* { dg-final { scan-assembler-times "cb\\.w" 6 } } */
