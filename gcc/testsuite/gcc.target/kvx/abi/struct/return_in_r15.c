#include <string.h>

struct big_struct
{
  long long r0;
  long long r1;
  int r2_0;
  int r2_1;
  char r3_stack[200];
};

extern int check_return_is_correct (struct big_struct (*f) (void),
				    int size_of_struct);

struct big_struct
return_in_r15 (void)
{
  struct big_struct ret = {0, 1, 2, 3};
  memset (&ret.r3_stack, 0xAB, 200);
  return ret;
}

int
main (int argc, char **argv)
{

  // CHECK_STRUCT_RET_RELATIVE r0:0 0x0
  // CHECK_STRUCT_RET_RELATIVE r0:8 0x1
  // CHECK_STRUCT_RET_RELATIVE r0:16 0x0000000300000002
  // CHECK_STRUCT_RET_RELATIVE r0:24 0xabababababababab
  // CHECK_STRUCT_RET_RELATIVE r0:32 0xabababababababab
  // CHECK_RET_REG_BEFORE_AFTER r15 r0
  return !check_return_is_correct (return_in_r15, sizeof (struct big_struct));
}
