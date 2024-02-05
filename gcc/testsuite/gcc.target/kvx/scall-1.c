long scall_with_imm ()
{
  return __builtin_kvx_scall (12);
}

/* { dg-final { scan-assembler-times "scall 12" 1 } } */

long scall_with_reg (int scall_no)
{
  return __builtin_kvx_scall (scall_no);
}

/* { dg-final { scan-assembler-times "scall \\\$r0" 1 } } */

/* { dg-final { scan-assembler-times "nop" 2 } } */
