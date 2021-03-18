#include <stdarg.h>

int
check_args_are_correct (int argument0, int argument1,

			int argument2, int argument3, int argument4,
			int argument5,

			int argument6, int argument7, int argument8,
			int argument9,

			int argument10, int argument11, int stack_slot0,
			int lastnamed, ...)
{

  // CHECK _VAR argument0  0

  // CHECK _VAR argument1  1

  // CHECK _VAR argument2  2

  // CHECK _VAR argument3  3

  // CHECK _VAR argument4  4

  // CHECK _VAR argument5  5

  // CHECK _VAR argument6  6

  // CHECK _VAR argument7  7

  // CHECK _VAR argument8  8

  // CHECK _VAR argument9  9

  // CHECK _VAR argument10 10

  // CHECK _VAR overflow_arg 11

  // CHECK _VAR other 12

  // CHECK_ VAR lastnamed 13

  // CHECK_VARARG int (14)

  va_list ap;
  va_start (ap, lastnamed);

  int va_arg_var_0 = va_arg (ap, int);
  if (va_arg_var_0 != (14))
    return 0;
  // CHECK_VARARG int (15)

  int va_arg_var_1 = va_arg (ap, int);
  if (va_arg_var_1 != (15))
    return 0;
  // CHECK_VARARG int (16)

  int va_arg_var_2 = va_arg (ap, int);
  if (va_arg_var_2 != (16))
    return 0;
  // CHECK_VARARG int (17)

  int va_arg_var_3 = va_arg (ap, int);
  if (va_arg_var_3 != (17))
    return 0;
  // CHECK_VARARG int (18)

  int va_arg_var_4 = va_arg (ap, int);
  if (va_arg_var_4 != (18))
    return 0;
  // CHECK_VARARG int (19)

  int va_arg_var_5 = va_arg (ap, int);
  if (va_arg_var_5 != (19))
    return 0;
  // CHECK_VARARG int (20)

  int va_arg_var_6 = va_arg (ap, int);
  if (va_arg_var_6 != (20))
    return 0;
  // CHECK_VARARG int (21)

  int va_arg_var_7 = va_arg (ap, int);
  if (va_arg_var_7 != (21))
    return 0;
  // CHECK_VARARG int (22)

  int va_arg_var_8 = va_arg (ap, int);
  if (va_arg_var_8 != (22))
    return 0;
  // CHECK_VARARG int (23)

  int va_arg_var_9 = va_arg (ap, int);
  if (va_arg_var_9 != (23))
    return 0;
  // CHECK_VARARG int (24)

  int va_arg_var_10 = va_arg (ap, int);
  if (va_arg_var_10 != (24))
    return 0;
  // CHECK_VARARG int (25)

  int va_arg_var_11 = va_arg (ap, int);
  if (va_arg_var_11 != (25))
    return 0;
  return 1;
}

// only here to please the script, other function already took care of
// unwrapping args.

int
check_args_are_correct_user (int argument0, int argument1,

			     int argument2, int argument3, int argument4,
			     int argument5,

			     int argument6, int argument7, int argument8,
			     int argument9,

			     int argument10, int argument11, int stack_slot0,
			     int lastnamed, ...)
{

  return 1;
}

int
main (int argc, char **argv)
{

  return !check_args_are_correct (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,

				  12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
				  23, 24, 25);
}
