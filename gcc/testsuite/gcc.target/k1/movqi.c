/* { dg-do  compile } */
/* { dg-options "-O3 -save-temps -fdump-rtl-reload"  } */

char movqi_reg_reg(char x, char y) { 
  return y;
}

char movqi_reg_imm(char x, char y) { 
  return 0x88;
}

/* { dg-final { scan-assembler "copy \\\$r\[0-9\]+ = \\\$r\[0-9\]+" } } */
/* { dg-final { scan-assembler "make \\\$r\[0-9\]+ = -120" } } */
/* { dg-final { scan-rtl-dump " Choosing alt 0 in insn .*:.*I08.*movqi_real" "reload"} } */
