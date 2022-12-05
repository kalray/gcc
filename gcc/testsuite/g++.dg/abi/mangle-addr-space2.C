// { dg-do compile { target { i?86-*-* x86_64-*-* } } }
// { dg-final { scan-assembler "_Z1fIU8__seg_fsiEiPT_" } }

template <class T>
int f (T *p) { return *p; }
int g (__seg_fs int *p) { return *p; }
__seg_fs int *a;
int main() { f(a); }
