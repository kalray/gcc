/* { dg-do  compile } */
/* { dg-options "-save-temps -Os"  } */

void a(char *b, unsigned c) {
  int d;
  do
    d = b[0] = d;
  while (--c);
}
/* { dg-final { scan-assembler "loopdo" } } */
