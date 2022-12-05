// { dg-do compile { target { i?86-*-* x86_64-*-* } } }

__seg_fs struct foo; // { dg-error "address space can only be specified for objects and functions" }

int
main ()
{
	return 0; 
}
