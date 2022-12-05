// { dg-do compile { target { i?86-*-* x86_64-*-* } } }
__seg_fs char a, b, c;
__seg_fs const int *p;
static /* gives internal linkage to variable q */
__seg_fs struct { int a; char b; } * __seg_gs q;
