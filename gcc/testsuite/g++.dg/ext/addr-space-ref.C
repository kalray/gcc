// { dg-do compile { target { i?86-*-* x86_64-*-* } } }
// { dg-prune-output "does not allow .register. storage class specifier" }
int __seg_fs * outer_b;

struct s {
  __seg_fs int * ok;
  __seg_gs int ko; // { dg-error ".__seg_gs. specified for structure field .ko." }
};

namespace ns_a
{
  int __seg_fs * inner_b;

  template<typename T>
  int f (T &a) { return a; }
  int g (__seg_fs int a) { return a; } // { dg-error ".__seg_fs. specified for parameter .a." }
  int h (__seg_fs int *a) { return *a; }
}

int
main ()
{
  int register __seg_gs reg_gs; // { dg-error ".__seg_gs. specified for variable .reg_gs. with automatic storage" }
  static __seg_gs int static_gs;
  __seg_fs int auto_fs; // { dg-error ".__seg_fs. specified for variable .auto_fs. with automatic storage" }
  __seg_fs int *pa = outer_b;
  __seg_fs int& ra = *ns_a::inner_b;
  return ns_a::f(ra) + ns_a::f(*pa);
}
