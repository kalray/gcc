// { dg-do compile { target { kvx-*-* } } }

__bypass struct foo; // { dg-error "address space can only be specified for objects and functions" }

int
main ()
{
	return 0; 
}
