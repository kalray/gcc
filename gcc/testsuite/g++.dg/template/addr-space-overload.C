// { dg-do compile { target { i?86-*-* x86_64-*-* } } }

int __seg_fs * fs1;
int __seg_gs * gs1;

template<typename T, typename U>
__seg_fs T* f (T __seg_fs * a, U __seg_gs * b) { return a; }
template<typename T, typename U>
__seg_gs T* f (T __seg_gs * a, U __seg_fs * b) { return a; }

int
main ()
{
    f (fs1, gs1);
    f (gs1, fs1);
    return 0;
}
