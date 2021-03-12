/* { dg-do  compile } */
/* { dg-options "-O3"  } */

/* This test previously ICE because we were trying to split a 128 load in odd
   argument register.
 */

typedef struct
{
  struct
  {
    long a;
  };
  int : 8;
} b;
int c, e;
b *d;
void g (int, b);
void
f (void)
{
  g (c, d[e]);
}
