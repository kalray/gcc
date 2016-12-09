
static unsigned int
udivmodsi4 (unsigned int num, unsigned int den, int modwanted)
{
  unsigned r = num, q = 0;

  if (den <= r)
    {
      unsigned k = __builtin_k1_clz (den) - __builtin_k1_clz (r);
      den = den << k;
      if (r >= den)
	{
	  r = r - den;
	  q = 1 << k;
	}
      if (k != 0)
	{
	  unsigned i = k;
	  den = den >> 1;
	  do
	    {
	      r = __builtin_k1_stsu (den, r);
	      i--;
	    }
	  while (i != 0);
	  q = q + r;
	  r = r >> k;
	  q = q - (r << k);
	}
    }

  return modwanted ? r : q;
}

unsigned int
__udivsi3 (unsigned int a, unsigned int b)
{
  return udivmodsi4 (a, b, 0);
}

unsigned int
__umodsi3 (unsigned int a, unsigned int b)
{
  return udivmodsi4 (a, b, 1);
}

int
__divsi3 (int a, int b)
{
  int neg = 0;
  int res;

  if (a < 0)
    {
      a = -a;
      neg = !neg;
    }

  if (b < 0)
    {
      b = -b;
      neg = !neg;
    }

  res = udivmodsi4 (a, b, 0);

  if (neg)
    res = -res;

  return res;
}

int
__modsi3 (int a, int b)
{
  int neg = 0;
  int res;

  if (a < 0)
    {
      a = -a;
      neg = 1;
    }

  if (b < 0)
    b = -b;

  res = udivmodsi4 (a, b, 1);

  if (neg)
    res = -res;

  return res;
}
