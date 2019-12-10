/* { dg-do  compile } */
/* { dg-options "-O3 -fstack-limit-register=sr"  } */
/* { dg-xfail-if "-fstack-limit-* only works on ClusterOS" { ! k1-*-cos } { "*" } { "" } } */

#include <alloca.h>

extern void g (char *);

void
f (int n)
{
  char *data;

  data = alloca (n);
  data[0] = '@';
  g (data);
}

/* { dg-final { scan-assembler-times "errop" 1 { target { k1-*-cos } } } } */
