/* { dg-do  compile } */
/* { dg-options "-O3 -save-temps"  } */

extern int f();
extern int g();

void cdb_eq (long long cond) {
  if (cond == 0)
    f();

  if (cond != 0)
    g();
}
/* { dg-final { scan-assembler-times "cdb.*" 1 } } */
