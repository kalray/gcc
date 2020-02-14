#include <stdarg.h>

int
check_args_are_correct (int argument0, int lastnamed, ...)
{

  // CHECK_VAR argument0 0

  if (argument0 != 0)
    return 0;
  // CHECK_VAR lastnamed 1

  if (lastnamed != 1)
    return 0;
  // CHECK_VARARG int (2)

  va_list ap;
  va_start (ap, lastnamed);

  int va_arg_var_0 = va_arg (ap, int);
  if (va_arg_var_0 != (2))
    return 0;
  // CHECK_VARARG int (3)

  int va_arg_var_1 = va_arg (ap, int);
  if (va_arg_var_1 != (3))
    return 0;
  // CHECK_VARARG int (4)

  int va_arg_var_2 = va_arg (ap, int);
  if (va_arg_var_2 != (4))
    return 0;
  // CHECK_VARARG int (5)

  int va_arg_var_3 = va_arg (ap, int);
  if (va_arg_var_3 != (5))
    return 0;
  // CHECK_VARARG int (6)

  int va_arg_var_4 = va_arg (ap, int);
  if (va_arg_var_4 != (6))
    return 0;
  // CHECK_VARARG int (7)

  int va_arg_var_5 = va_arg (ap, int);
  if (va_arg_var_5 != (7))
    return 0;
  // CHECK_VARARG int (8)

  int va_arg_var_6 = va_arg (ap, int);
  if (va_arg_var_6 != (8))
    return 0;
  // CHECK_VARARG int (9)

  int va_arg_var_7 = va_arg (ap, int);
  if (va_arg_var_7 != (9))
    return 0;
  // CHECK_VARARG int (10)

  int va_arg_var_8 = va_arg (ap, int);
  if (va_arg_var_8 != (10))
    return 0;
  // CHECK_VARARG int (11)

  int va_arg_var_9 = va_arg (ap, int);
  if (va_arg_var_9 != (11))
    return 0;
  // CHECK_VARARG int (12)

  int va_arg_var_10 = va_arg (ap, int);
  if (va_arg_var_10 != (12))
    return 0;
  // CHECK_VARARG int (13)

  int va_arg_var_11 = va_arg (ap, int);
  if (va_arg_var_11 != (13))
    return 0;
  // CHECK_VARARG int (14)

  int va_arg_var_12 = va_arg (ap, int);
  if (va_arg_var_12 != (14))
    return 0;
  // CHECK_VARARG int (15)

  int va_arg_var_13 = va_arg (ap, int);
  if (va_arg_var_13 != (15))
    return 0;
  return 1;
}

// only here to please the script, other function already took care of
// unwrapping args.

int
check_args_are_correct_user (int argument0, int lastnamed, ...)
{

  return 1;
}

int
main (int argc, char **argv)
{

  return !check_args_are_correct (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,

				  12, 13, 14, 15);
}
