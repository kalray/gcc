long
scall_4 (char *arg1, double arg2, int arg3, long arg4, int scallno)
{
  return __builtin_kvx_scall (scallno, arg1, arg2, arg3, arg4);
}

/* { dg-final { scan-assembler-times "scall \\\$r4" 1 } } */

long
scall_1 (int scallno)
{
  return __builtin_kvx_scall (scallno, 1);
}

/* { dg-final { scan-assembler-times "make \\\$r0 = 1" 1 } } */
/* { dg-final { scan-assembler-times "scall \\\$r1" 1 } } */

/* { dg-final { scan-assembler-times "nop" 2 } } */
