/* THIS IS THE PREVIOUS VERSION, USED ON BOSTAN AND ANDEY */
static unsigned long long
udivmoddi4 (unsigned long long num, unsigned long long den, int modwanted)
{
  unsigned long long r = num, q = 0;

  if (den <= r)
    {
      unsigned k = __builtin_clzll (den) - __builtin_clzll (r);
      den = den << k;
      if (r >= den)
	{
	  r = r - den;
	  q = 1LL << k;
	}
      if (k != 0)
	{
	  unsigned i = k;
	  den = den >> 1;
	  do
	    {
	      r = __builtin_k1_stsud (den, r);
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

unsigned long long
__udivdi3 (unsigned long long a, unsigned long long b)
{
  return udivmoddi4 (a, b, 0);
}

unsigned long long
__umoddi3 (unsigned long long a, unsigned long long b)
{
  return udivmoddi4 (a, b, 1);
}

long long
__divdi3 (long long a, long long b)
{
  int neg = 0;
  long long res;

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

  res = udivmoddi4 (a, b, 0);

  if (neg)
    res = -res;

  return res;
}

long long
__moddi3 (long long a, long long b)
{
  int neg = 0;
  long long res;

  if (a < 0)
    {
      a = -a;
      neg = 1;
    }

  if (b < 0)
    b = -b;

  res = udivmoddi4 (a, b, 1);

  if (neg)
    res = -res;

  return res;
}
