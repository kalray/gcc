/* { dg-do  compile } */
/* { dg-options "-O2"  } */

void
f (void)
{
	__builtin_trap();
}

/* { dg-final { scan-assembler "errop" } } */
