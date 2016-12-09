/* { dg-do  compile } */
/* { dg-options "-O3 -save-temps"  } */

long long reg_reg(int a, int b) {
  return (long long)a * b;
}

#define SIGNED_10BIT -10
#define SIGNED_32BIT -150994942

long long mulwd_imm10bits(int reg1){
  return (long long)reg1*SIGNED_10BIT;
}

long long mulwd_imm32bits(int reg1){
  return (long long)reg1*SIGNED_32BIT;
}

/* { dg-final { scan-assembler-times "mulwd " 3 } } */
