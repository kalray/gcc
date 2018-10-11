/* { dg-do compile } */
/* { dg-options "-O3 -save-temps"  } */

extern void foo(int *x);
extern int foofar(int x) __attribute__((farcall));

int caller(int x){
  int x2;

  foo(&x2);
  x2 = foofar(x2);
  return x2;
}

/* { dg-final { scan-assembler-times "call foo" 1 } } */
/* { dg-final { scan-assembler "make \(\\\$r\\d+\) = foofar\\n.*\\n\[ \t\]*icall \\1" } } */
/* { dg-final { cleanup-saved-temps } } */
