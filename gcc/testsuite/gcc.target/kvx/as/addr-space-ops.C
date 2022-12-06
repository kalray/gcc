// { dg-do compile { target { kvx-*-* } } }
int __bypass * fs1;
int __speculate * gs1;

int
main ()
{
  fs1 + fs1; // { dg-error "invalid operands of types .__bypass int.. and .__bypass int.. to binary .operator.." }
  return 0;
}
