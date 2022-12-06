// { dg-do compile { target { kvx-*-* } } }

int __bypass * fs1;
int __speculate * gs1;

template<typename T, typename U>
__bypass T* f (T __bypass * a, U __speculate * b) { return a; }
template<typename T, typename U>
__speculate T* f (T __speculate * a, U __bypass * b) { return a; }

int
main ()
{
    f (fs1, gs1);
    f (gs1, fs1);
    return 0;
}
