// { dg-do compile { target { kvx-*-* } } }

int __bypass * fs1;
int __speculate * gs1;

template<typename T, typename U>
bool f (T __bypass * a, U __speculate * b)
{
  // no warnings since __bypass and __speculate
  // are the same address space.
  return *(T *) a == *(U *) b;
}

int
main ()
{
  return f (fs1, gs1);
}
