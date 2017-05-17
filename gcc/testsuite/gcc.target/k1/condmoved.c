/* { dg-do  run } */
/* { dg-options "-O2 -save-temps"  } */
/* { dg-skip-if "Test valid only on Andey/Bostan" { *-*-* }  {  "*" } { "-march=k1a" "-march=k1b" } } */

#include <stdlib.h>

__attribute__((noclone,noinline))
long long cmove1(long long a, long long b, long long c, long long d)
{
	long long res;

	res = c;

	if (a < 0)
		res = b+1;
	
	if (c >= 0)
		res = d+1;

	return res;
}

__attribute__((noclone,noinline))
long long cmove2(long long a, long long b, long long c, long long d)
{
        long long res;

        res = d;

	if (a == 0)
		res = b+1;
 
	if (c != 0)
		res = d+1;

	return res; 
}

int main()
{
	long long res = 0;

	res += cmove1(0,-1,1,0);   /* no cond => return c => 1 */
	res += cmove1(-1,2,0,1);   /* a<0 && c>=0 => return d+1 => 2*/
	res += cmove1(-1,3,-1,0);  /* a<0 => return b+1 => 4 */
	res += cmove1(1,0,1,7);    /* c>=0 => return d+1 => 8 */

	res += cmove2(1,0,0,16);   /* no cond => return b => 16 */
	res += cmove2(0,2,1,31);   /* a==0 && c!=0 => return d+1 => 32 */
	res += cmove2(0,63,0,0);   /* a==0 => return b+1 => 64 */
	res += cmove2(1,0,1,127);  /* c!=0 => return d+1 => 128 */

	return res - 0xFF;
}

/* { dg-final { scan-assembler-times "cmoved" 4 } } */
/* { dg-final { scan-assembler-not "cb\\." } } */
/* { dg-final { cleanup-saved-temps } } */
