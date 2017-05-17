/* { dg-do  compile } */
/* { dg-options "-O3 -save-temps -fdump-rtl-reload"  } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  { "*" } { "-march=k1a" "-march=k1b" } } */

#define UNSIGNED_9BIT 512U
#define SIGNED_32BIT -150994942
#define UNSIGNED_32BIT_THAT_FITS_IN_SIGNED10 (0xffffffffU)
#define UNSIGNED_32BIT 0x0ffffffU

unsigned long long muluuwd_reg(unsigned int reg1, unsigned int reg2){
  return (unsigned long long)reg1 * reg2;
}

/* This one does not work yet */
/* unsigned long long muluuwd_smallint_signed10(unsigned int reg1){ */
/*   return (unsigned long long)reg1 * UNSIGNED_9BIT; */
/* } */

unsigned long long muluuwd_largeint_signed10(unsigned int reg1){
  return (unsigned long long)reg1 * UNSIGNED_32BIT_THAT_FITS_IN_SIGNED10;
}

unsigned long long muluuwd_unsigned32(unsigned int reg1){
  return (unsigned long long)reg1 * UNSIGNED_32BIT;
}

extern unsigned int external_symbol;

unsigned long long muluuwd_symbol(unsigned int reg1){
  return (unsigned long long)reg1 * external_symbol;
}

/* { dg-final { scan-assembler-times "muluuwd " 4 } } */
/* { dg-final { scan-rtl-dump-times "Choosing alt 0 in insn ..:  \\(0\\) =r  \\(1\\) r  \\(2\\).r...umulsidi3_reg" 2 "reload"} } */
/* { dg-final { scan-rtl-dump ".*Choosing alt 0 in insn .*:.*\(0\).*=r.*\(1\).*r.*\(2\).*J10.*{\*umulsidi3_imm}" "reload"} } */
/* { dg-final { scan-rtl-dump ".*Choosing alt 1 in insn .*:.*\(0\).*r.*\(1\).*r.*\(2\).*U32.*{\*umulsidi3_imm}" "reload"} } */
