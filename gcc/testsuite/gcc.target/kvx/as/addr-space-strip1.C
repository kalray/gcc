// { dg-do compile { target { kvx-*-* } } }
// { dg-options "-std=c++11" }
// decltype is only available since c++11

int __bypass * fs1;
int __speculate * gs1;

template<typename T> struct strip;
template<typename T> struct strip<__bypass T *> { typedef T type; };
template<typename T> struct strip<__speculate T *> { typedef T type; };

int
main ()
{
    *(strip<decltype(fs1)>::type *) fs1 == *(strip<decltype(gs1)>::type *) gs1;
    // no warning since the address space are the same.
    return 0;
}
