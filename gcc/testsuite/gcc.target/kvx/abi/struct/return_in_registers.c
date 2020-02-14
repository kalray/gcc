#include <string.h>

struct small_enough_struct
{
  long long r0;
  long long r1;
  long long r2;
  long long r3;
};

extern int check_return_is_correct (struct small_enough_struct (*f) (void));

struct small_enough_struct
return_in_regs (void)
{
  struct small_enough_struct ret = {0, 1, 2, 3};
  return ret;
}

int
main (int argc, char **argv)
{

  // CHECK_RET_REG r0 0x0
  // CHECK_RET_REG r1 0x1
  // CHECK_RET_REG r2 0x2
  // CHECK_RET_REG r3 0x3

  return !check_return_is_correct (return_in_regs);
}
