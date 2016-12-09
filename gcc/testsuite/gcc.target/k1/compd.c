/* { dg-do  run } */
/* { dg-options "-O2 -save-temps"  } */

#include <stdlib.h>

__attribute__((noinline))
int lt() {
	return 0;
}

__attribute__((noinline))
int ge() {
        return 1;
}

__attribute__((noinline))
int eq() {
	return 2;
}

__attribute__((noinline))
int ne() {
        return 3;
}
__attribute__((noinline,noclone))
int compd(long long a, long long b, long long c, long long d)
{
	if (a < 0)
		return lt();
	if (b == 0)
		return eq();
	if (c >= 0)
		return ge();
	if (d != 0)
		return ne();

	return -1;
}

int main()
{
	if (compd(-1,0,0,0) != lt())
		abort();
	if (compd(0,0,0,0) != eq())
		abort();
	if (compd(0,1,0,0) != ge())
		abort();
	if (compd(0,1,1,0) != ge())
		abort();
	if (compd(0,1,-1,1) != ne())
		abort();
	if (compd(0,1,-1,0) != -1)
		abort();

	return 0;
}

/* { dg-final { scan-assembler-not "compdl" } } */
/* { dg-final { cleanup-saved-temps } } */
