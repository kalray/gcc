/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-times "cbs \\\$r\[0-9\]+ = \\\$r\[0-9\]+" 2 } } */
/* { dg-final { scan-assembler-times "cbsdl \\\$r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+" 1 } } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  { "*"  } { "-march=k1a" "-march=k1b" } } */

int f_l_l(unsigned long long bob){
  return __builtin_popcountll(bob);
}

int f_l(unsigned long bob){
  return __builtin_popcountl(bob);
}

int f(unsigned int bob){
  return __builtin_popcount(bob);
}


