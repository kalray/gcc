// { dg-do compile { target { i?86-*-* x86_64-*-* } } }
// { dg-require-effective-target c++11 }
// decltype is only available since c++11

int __seg_fs * fs1;
int __seg_gs * gs1;

template<typename T> struct strip;
template<typename T> struct strip<__seg_fs T *> { typedef T type; };
template<typename T> struct strip<__seg_gs T *> { typedef T type; };

int
main ()
{
    *(strip<decltype(fs1)>::type *) fs1 == *(strip<decltype(gs1)>::type *) gs1;
    // { dg-warning "cast to generic address space pointer from disjoint" "" { target *-*-* } .-1 }
    return 0;
}
