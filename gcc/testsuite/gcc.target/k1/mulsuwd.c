/* { dg-do  compile } */
/* { dg-options "-O3 -save-temps"  } */

#define UNSIGNED_32BIT (0xffffffffU)
#define UNSIGNED_BIG_SEX10BIT 0xfffff3ff

long long mulsuwd_reg(int reg1, unsigned int reg2){
  return ((long long)reg1) * reg2;
}

/*
 * The immediate variants are never selected by GCC yet, leading to
 * bigger/slower code...
 */

/* long long mulsuwd_imm10bits(int reg1){ */
/*   return (long long)reg1 * (UNSIGNED_BIG_SEX10BIT); */
/* } */

/* long long mulsuwd_imm32bits(int reg1){ */
/*   return (long long)reg1 * (UNSIGNED_32BIT); */
/* } */

/* { dg-final { scan-assembler-times "mulsuwd " 1 } } */
