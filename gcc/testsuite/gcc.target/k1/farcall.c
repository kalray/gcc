/* { dg-do compile } */
/* { dg-options "-O3 -save-temps -mfarcall"  } */

extern void foo(int *x);
extern int foofar(int x);

int caller(int x){
  int x2;

  foo(&x2);
  x2 = foofar(x2);
  return x2;
}

/* { dg-final { scan-assembler "make \(\\\$r\\d+\) = foo\\n.*\\n\[ \t\]*icall \\1" } } */
/* { dg-final { scan-assembler "make \(\\\$r\\d+\) = foofar\\n.*\\n\[ \t\]*icall \\1" } } */
/* { dg-final { cleanup-saved-temps } } */
