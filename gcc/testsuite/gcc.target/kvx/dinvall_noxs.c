/* { dg-do compile } */
/* { dg-options "-O2 -march=kv3-1" } */

void
dinvall_noxs_byte (int n, char *a, unsigned m)
{
  int i = n - 1;
  a[i] &= m;
  __builtin_kvx_dinvall (&a[i]);
}

void
dinvall_noxs_short (int n, short *a, unsigned m)
{
  int i = n - 1;
  a[i] &= m;
  __builtin_kvx_dinvall (&a[i]);
}

void
dinvall_noxs_int (int n, int *a, unsigned m)
{
  int i = n - 1;
  a[i] &= m;
  __builtin_kvx_dinvall (&a[i]);
}

void
dinvall_noxs_long (int n, long *a, unsigned long m)
{
  int i = n - 1;
  a[i] &= m;
  __builtin_kvx_dinvall (&a[i]);
}

void
dinvall_noxs_longlong (int n, long long *a, unsigned long long m)
{
  int i = n - 1;
  a[i] &= m;
  __builtin_kvx_dinvall (&a[i]);
}
