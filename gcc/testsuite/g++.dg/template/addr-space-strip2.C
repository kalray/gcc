// { dg-do compile { target { i?86-*-* x86_64-*-* } } }

int __seg_fs * fs1;
int __seg_gs * gs1;

template<typename T, typename U>
bool f (T __seg_fs * a, U __seg_gs * b)
{
    return *(T *) a == *(U *) b;
    // { dg-warning "cast to generic address space pointer from disjoint" "" { target *-*-* } .-1 }
}

int
main ()
{
    return f (fs1, gs1);
}
