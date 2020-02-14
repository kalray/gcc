/* { dg-do compile } */
/* { dg-options "-O3 -fpic" } */
/* { dg-final { scan-assembler "pcrel \\\$r\[0-9\]+ = @pcrel\\(\[A-Z0-9.\]+\\)" } } */

extern int extfoo(void *foo);

int bar(void){
some_label:
   return extfoo(&&some_label);
}
