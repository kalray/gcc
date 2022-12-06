// { dg-do compile { target { kvx-*-* } } }
// { dg-prune-output "does not allow .register. storage class specifier" }
int __bypass * outer_b;

struct s {
  __bypass int * ok;
  __speculate int ko; // { dg-error ".__speculate. specified for structure field .ko." }
};

namespace ns_a
{
  int __bypass * inner_b;

  template<typename T>
  int f (T &a) { return a; }
  int g (__bypass int a) { return a; } // { dg-error ".__bypass. specified for parameter .a." }
  int h (__bypass int *a) { return *a; }
}

int
main ()
{
  int register __speculate reg_gs; // { dg-error ".__speculate. specified for variable .reg_gs. with automatic storage" }
  static __speculate int static_gs;
  __bypass int auto_fs; // { dg-error ".__bypass. specified for variable .auto_fs. with automatic storage" }
  __bypass int *pa = outer_b;
  __bypass int& ra = *ns_a::inner_b;
  return ns_a::f(ra) + ns_a::f(*pa);
}
