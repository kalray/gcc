// { dg-do compile { target { i?86-*-* x86_64-*-* } } }

__seg_fs __seg_gs int *a; // { dg-error "incompatible address space qualifiers" }

int
main ()
{
	return 0;
}
