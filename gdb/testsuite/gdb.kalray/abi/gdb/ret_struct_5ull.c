#include <stdio.h>

#define NB_LONGS 5
const int nb_longs = NB_LONGS;

struct struct_ret
{
  unsigned long long a[NB_LONGS];
};

struct struct_ret
fc_return_struct (int x)
{
  int i;
  struct struct_ret ret;

  for (i = 0; i < nb_longs; i++)
    ret.a[i] = i + x;

  return ret;
}

int
check_return_is_correct (struct struct_ret ret, int x)
{
  int i;

  for (i = 0; i < nb_longs; i++)
    if (ret.a[i] != i + x)
      return 0;

  return 1;
}

int
main (int argc, char **argv)
{
  return !check_return_is_correct (fc_return_struct (nb_longs), nb_longs);
}
