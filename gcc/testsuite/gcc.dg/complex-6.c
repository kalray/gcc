/* PR tree-optimization/70291.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-cplxlower" } */

__complex float
foo (__complex float a, __complex float b)
{
  return a * b;
}

/* { dg-final { scan-tree-dump-times "unord" 1 "cplxlower1" { target { ! { rx*-*-* kvx*-*-* } } } } } */
/* { dg-final { scan-tree-dump-times "__(?:gnu_)?mulsc3" 1 "cplxlower1" { target { ! { rx*-*-* kvx*-*-* } } } } } */
/* { dg-final { scan-tree-dump-times "__(?:gnu_)?mulsc3" 0 "cplxlower1" { target { kvx*-*-* } } } } */
