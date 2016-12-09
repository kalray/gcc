/* { dg-do  compile } */
/* { dg-options "-O3 -save-temps"  } */


unsigned sbfx8(unsigned a, unsigned b) {
  return a - (b << 3);
}

unsigned sbfx4(unsigned a, unsigned b) {
  return a - (b << 2);
}
unsigned sbfx2(unsigned a, unsigned b) {
  return a - (b << 1);
}

/* { dg-final { scan-assembler "sbfx2 \\\$r\[0-9\]+ = \\\$r\[0-9\]+, \\\$r\[0-9\]+" } } */
/* { dg-final { scan-assembler "sbfx4 \\\$r\[0-9\]+ = \\\$r\[0-9\]+, \\\$r\[0-9\]+" } } */
/* { dg-final { scan-assembler "sbfx8 \\\$r\[0-9\]+ = \\\$r\[0-9\]+, \\\$r\[0-9\]+" } } */
