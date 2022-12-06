// { dg-do compile { target { kvx-*-* } } }
// { dg-options "-std=gnu++98" }

int
main ()
{
	struct foo {int a; char b[2];} structure;
	structure = ((__bypass struct foo) {1 + 2, 'a', 0}); // { dg-error "compound literal qualified by address-space qualifier" }
	return 0; 
}
