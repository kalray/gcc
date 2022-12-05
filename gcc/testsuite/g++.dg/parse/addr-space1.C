// { dg-do compile { target { i?86-*-* x86_64-*-* } } }
// { dg-options "-std=gnu++98" }

int
main ()
{
	struct foo {int a; char b[2];} structure;
	structure = ((__seg_fs struct foo) {1 + 2, 'a', 0}); // { dg-error "compound literal qualified by address-space qualifier" }
	return 0; 
}
