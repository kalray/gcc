/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "ctzdl \\\$r\[0-9\]+ = \\\$r\[0-9\]+r\[0-9\]+" } } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

int f_ctzdl(unsigned long long a){
  int v = __builtin_k1_ctzdl(a); 
  if ( v < 64) 
    {   
      return 1;
    }   
  else
    {   
      return 0;
    }   
}
