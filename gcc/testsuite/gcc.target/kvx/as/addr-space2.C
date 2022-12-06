// { dg-do compile { target { kvx-*-* } } }

__bypass __speculate int *a; // { dg-error "incompatible address space qualifiers" }

int
main ()
{
	return 0;
}
