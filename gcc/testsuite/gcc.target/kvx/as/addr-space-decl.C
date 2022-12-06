// { dg-do compile { target { kvx-*-* } } }
__bypass char a, b, c;
__bypass const int *p;
static /* gives internal linkage to variable q */
__bypass struct { int a; char b; } * __speculate q;
