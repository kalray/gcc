// { dg-do compile { target { i?86-*-* x86_64-*-* } } }
int __seg_fs * fs1;
int __seg_fs * fs2;
int __seg_gs * gs1;
int __seg_gs * gs2;

int
main ()
{
  fs1 + fs2; // { dg-error "invalid operands of types .__seg_fs int.. and .__seg_fs int.. to binary .operator.." }
  fs1 - fs2;
  fs1 - gs2; // { dg-error "pointers to disjoint address spaces .__seg_fs int.. and .__seg_gs int.. in substraction" }
	     // { dg-error ".__seg_fs int.. and .__seg_gs int.. are in disjoint named address spaces" "" { target *-*-* } .-1 }
  fs1 == fs2;
  fs1 != gs2; // { dg-error ".__seg_fs int.. and .__seg_gs int.. are in disjoint named address spaces" }
  fs1 = fs2;
  fs1 = gs2; // { dg-error "invalid conversion from .__seg_gs int.. to .__seg_fs int.." }
  fs1 > fs2;
  fs1 < gs2; // { dg-error ".__seg_fs int.. and .__seg_gs int.. are in disjoint named address spaces" }
  return 0;
}
