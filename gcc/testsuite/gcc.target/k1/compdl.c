/* { dg-do  run } */
/* { dg-options "-O2 -save-temps"  } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

#include <stdlib.h>

#define COMP_F(name, operator) __attribute__((noinline,noclone))	\
int compdl_##name(long long a, long long b) \
{ \
  return a  operator  b; \
} \
__attribute__((noinline,noclone)) \
int compdl_## name ##_0(long long a) \
{ \
  return a  operator  0; \
}

COMP_F(eq,==)
COMP_F(neq,!=)
COMP_F(lt,<)
COMP_F(gt,>)
COMP_F(le,<=)
COMP_F(ge,>=)

int main()
{
  if(! (!compdl_eq( 19, 25) && compdl_eq(19, 19))){
    abort();
  }

  if( ! (!compdl_eq_0(19) && compdl_eq_0(0))){
    abort();
  }

  if( ! (!compdl_neq( 19, 19) && compdl_neq(19, 25))){
    abort();
  }

  if( ! (compdl_neq_0(19) && !compdl_neq_0(0))){
    abort();
  }

  if( ! (!compdl_lt( 19, 19) && compdl_lt(19, 25) && !compdl_lt(25, 19))){
    abort();
  }

  if( !(!compdl_lt_0(19) && compdl_lt_0(-3))){
    abort();
  }

  return 0;
}

/* { dg-final { scan-assembler-times "compdl\.eq " 1 } } */
/* { dg-final { scan-assembler-times "compdl\.ne " 1 } } */
/* { dg-final { scan-assembler-times "compdl\.lt " 1 } } */
/* { dg-final { scan-assembler-times "compdl\.gt " 2 } } */
/* { dg-final { scan-assembler-times "compdl\.le " 2 } } */
/* { dg-final { scan-assembler-times "compdl\.ge " 1 } } */

/* { dg-final { cleanup-saved-temps } } */
