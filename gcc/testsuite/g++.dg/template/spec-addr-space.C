// { dg-do compile { target { i?86-*-* x86_64-*-* } } }

template <class T>
int f (T __seg_gs *p) { return *p; } // { dg-message "note: candidate: 'template<class T> int f.__seg_gs T\*." }
				     // { dg-message "note: template argument deduction/substitution failed:" "" { target *-*-* } .-1 }
__seg_fs int *a;
int main() { f(a); } // { dg-error "no matching" }
// { dg-message "note: types .__seg_gs T. and .__seg_fs int. have incompatible cv-qualifiers" "" { target *-*-* } .-1 }
