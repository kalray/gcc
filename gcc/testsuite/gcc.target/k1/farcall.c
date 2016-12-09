/* { dg-do compile } */
/* { dg-options "-O3 -save-temps -mfarcall"  } */

extern int foo(int x);
extern int foofar(int x);

int caller(int x){
  int x1, x2;

  x1 = foo(x);
  x2 = foofar(x1);
  return x1 + x2;
}

/* { dg-final { scan-assembler "make \(\\\$r\\d+\) = foo\\n.*\\n\[ \t\]*icall \\1" } } */
/* { dg-final { scan-assembler "make \(\\\$r\\d+\) = foofar\\n.*\\n\[ \t\]*icall \\1" } } */
/* { dg-final { cleanup-saved-temps } } */
