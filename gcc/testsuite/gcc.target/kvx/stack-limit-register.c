/* { dg-do  compile } */
/* { dg-options "-O3 -fstack-limit-register=sr" } */
/* { dg-xfail-if "-fstack-limit-* only works on ClusterOS" { ! kvx-*-cos } { "*" } { "" } } */

extern void g (char *);

void
f (void)
{
  char data[10];

  data[0] = '@';
  g (data);
}

/* { dg-final { scan-assembler-times "call __stack_overflow_detected" 1 { target { kvx-*-cos } } } } */
