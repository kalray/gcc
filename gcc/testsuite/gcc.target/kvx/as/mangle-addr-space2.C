// { dg-do compile { target { kvx-*-* } } }
// { dg-final { scan-assembler "_Z1fIU8__bypassiEiPT_" } }

template <class T>
int f (T *p) { return *p; }
int g (__bypass int *p) { return *p; }
__bypass int *a;
int main() { f(a); }
