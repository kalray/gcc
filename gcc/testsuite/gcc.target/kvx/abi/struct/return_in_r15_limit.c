#include <string.h>

struct big_struct
{
  long long r0;
  long long r1;
  long long r2;
  long long r3;
  char oversize;
};

extern int check_return_is_correct (struct big_struct (*f) (void),
				    int size_of_struct);

struct big_struct
return_in_r15 (void)
{
  struct big_struct ret = {0, 1, 2, 3, 0x88};
  return ret;
}

int
main (int argc, char **argv)
{

  // CHECK_STRUCT_RET_RELATIVE r15:0 0x0
  // CHECK_STRUCT_RET_RELATIVE r15:8 0x1
  // CHECK_STRUCT_RET_RELATIVE r15:16 0x2
  // CHECK_STRUCT_RET_RELATIVE r15:24 0x3
  // CHECK_STRUCT_RET_RELATIVE r15:32:1 0x88

  return !check_return_is_correct (return_in_r15, sizeof (struct big_struct));
}
