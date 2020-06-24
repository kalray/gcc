/*
 * Routines for KVX division and modulus on 64-bit integers.
 *
 * Based on the TMS320C6000 32-bit Integer division extended to 64-bit:
 *
 *     divmod_result_t
 *     divmodu(uint64_t a, uint64_t b)
 *     {
 *         uint64_t q = 0, r = a;
 *         if (b == 0) ERROR;
 *         if (b > a)
 *             goto end;
 *         int k = __builtin_clzll(b) - __builtin_clzll(a);
 *         b <<= k;
 *         k++;
 *         for (int i = 0; i < k; i++) {
 *             r = __builtin_kvx_stsud(b, r);
 *         }
 *         q = (r << (64-k)) >> (64-k);
 *         r >>= k;
 *     end:;
 *         divmod_result_t result = { q, r };
 *         return result;
 *     }
 *
 * This algorithm may overflow and is corrected as in libgcc2.c:
 *
 *     divmod_result_t
 *     divmodu(uint64_t a, uint64_t b)
 *     {
 *         uint64_t q = 0, r = a;
 *         if (b == 0) ERROR;
 *         if (b > a)
 *             goto end;
 *         int k = __builtin_clzll(b) - __builtin_clzll(a);
 *         b <<= k;
 *         if (r >= b) {
 *             r -= b;
 *             q = 1LL << k;
 *         }
 *         if (k > 0) {
 *             b >>= 1;
 *             uint64_t s = (1LL<<k) - 1;
 *             for (int i = 0; i < k; i++) {
 *                 r = __builtin_kvx_stsud(b, r);
 *             }
 *             q += r & s;
 *             r >>= k;
 *         }
 *     end:;
 *         divmod_result_t result = { q, r };
 *         return result;
 *     }
 *
 * -- Benoit Dupont de Dinechin (benoit.dinechin@kalray.eu)
 */

#include <stdint.h>

typedef uint64_t uint64x2_t __attribute ((vector_size (2 * sizeof (uint64_t))));

#define ERROR __asm__("errop")

static inline uint64x2_t
uint64_divmod (uint64_t a, uint64_t b)
{
  int k = __builtin_clzll (b) - __builtin_clzll (a);
  uint64_t q = 0, r = a;
  if (b == 0)
    goto div0;
  if (b > a)
    goto end;
  b <<= k;
  if (r >= b)
    {
      r -= b;
      q = 1LL << k;
    }
  if (k > 0)
    {
      b >>= 1;
      uint64_t s = (1LL << k) - 1;
      for (int i = 0; i < k; i++)
	{
	  r = __builtin_kvx_stsud (b, r);
	}
      q += r & s;
      r >>= k;
    }
end:
  return (uint64x2_t){q, r};
div0:
  ERROR;
  __builtin_unreachable ();
}

uint64_t
__udivdi3 (uint64_t a, uint64_t b)
{
  uint64x2_t divmod = uint64_divmod (a, b);
  return divmod[0];
}

uint64_t
__umoddi3 (uint64_t a, uint64_t b)
{
  uint64x2_t divmod = uint64_divmod (a, b);
  return divmod[1];
}

uint64_t
__udivmoddi4 (uint64_t a, uint64_t b, uint64_t *c)
{
  uint64x2_t divmod = uint64_divmod (a, b);
  *c = divmod[1];
  return divmod[0];
}

int64_t
__divdi3 (int64_t a, int64_t b)
{
  uint64_t absa = a < 0 ? -a : a;
  uint64_t absb = b < 0 ? -b : b;
  uint64x2_t divmod = uint64_divmod (absa, absb);
  if ((a ^ b) < 0)
    divmod[0] = -divmod[0];
  return divmod[0];
}

int64_t
__moddi3 (int64_t a, int64_t b)
{
  uint64_t absa = a < 0 ? -a : a;
  uint64_t absb = b < 0 ? -b : b;
  uint64x2_t divmod = uint64_divmod (absa, absb);
  if (a < 0)
    divmod[1] = -divmod[1];
  return divmod[1];
}
