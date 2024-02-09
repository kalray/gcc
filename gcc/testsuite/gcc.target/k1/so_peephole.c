/* { dg-do  compile } */
/* { dg-skip-if "Only supported on Coolidge" { *-*-* }  { "*" } { "-march=k1c" } } */
/* { dg-options "-O3 -std=c99" } */

/* { dg-final { scan-assembler-times "so -?\[0-9a-fA-F\]+\\\[\\\$r\[0-9\]+\\\] = \\\$r\[0-9\]r\[0-9\]r\[0-9\]r\[0-9\]" 2 } } */
void should_pack (unsigned long long r0, 
		  unsigned long long r1, 
		  unsigned long long r2, 
		  unsigned long long r3, 
		  unsigned long long *ptr64bits) {
  ptr64bits[0] = r0;
  ptr64bits[1] = r1;
  ptr64bits[2] = r2;
  ptr64bits[3] = r3;
}

void should_pack_2 (unsigned long long r0, 
		    unsigned long long r1, 
		    unsigned long long r2, 
		    unsigned long long r3, 
		    unsigned long long *ptr64bits) {
  ptr64bits[-1] = r0;
  ptr64bits[0] = r1;
  ptr64bits[1] = r2;
  ptr64bits[2] = r3;
}
