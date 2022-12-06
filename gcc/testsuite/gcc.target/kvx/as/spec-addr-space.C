// { dg-do compile { target { kvx-*-* } } }

template <class T>
int f (T __speculate *p) { return *p; }

__bypass int *a;
// __bypass and __speculate are the same address space,
// hence __bypass int *a is implicitely converted to
// __speculate int*.
int main() { f(a); }
