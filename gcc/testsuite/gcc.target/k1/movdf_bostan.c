/* { dg-do compile } */
/* { dg-skip-if "Only supported on Bostan" { *-*-* }  { "*" } { "-march=k1b" } } */
/* { dg-options "-O3 -save-temps -fdump-rtl-reload"  } */

double h16_double (void) {
  double tmp = 4.94065645841e-324; /* 0x01 */
  return tmp;
}

double h43_double(void){
  double tmp = 6.47581723317e-319; /* 0x020000 */
  return tmp;
}

double full_double(void){
  double tmp = 1.0f;
  return tmp;
}

/* { dg-final { scan-assembler-times "maked \\\$r\\dr\\d = 0x020000" 1 } } */
/* { dg-final { scan-assembler-times "maked \\\$r\\dr\\d = 0x01" 1 } } */
/* { dg-final { scan-rtl-dump ".*Choosing alt 0 in insn .*:.*H16.*movdf_real_imm_k1b" "reload"} } */
/* { dg-final { scan-rtl-dump ".*Choosing alt 1 in insn .*:.*H43.*movdf_real_imm_k1b" "reload"} } */
/* { dg-final { scan-rtl-dump ".*Choosing alt 2 in insn .*:.*F.*movdf_real_imm_k1b" "reload"} } */
