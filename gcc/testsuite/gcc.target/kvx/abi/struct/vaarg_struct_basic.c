#include <stdarg.h>

#define SHIFT_64                                                               \
  *(256ULL * 256ULL * 256ULL) * (256ULL * 256ULL * 256ULL * 256ULL * 256ULL)

struct big_struct
{

  int x;

  int y;

  unsigned __int128 bigint;
};

int
check_args_are_correct (int argument0, int argument1, int lastnamed, ...)
{

  // CHECK_VAR argument0 0

  if (argument0 != 0)
    return 0;
  // CHECK_VAR argument1 1

  if (argument1 != 1)
    return 0;
  // CHECK_VAR lastnamed 2

  if (lastnamed != 2)
    return 0;
  // CHECK_VARARG_STRUCT struct big_struct {x=(0),y=(1),bigint=(((unsigned
  // __int128)0xdeaddead12345678ULL) SHIFT_64 + 0xAACDFF9955555555ULL)}

  va_list ap;
  va_start (ap, lastnamed);

  struct big_struct va_arg_var_0 = va_arg (ap, struct big_struct);

  if (va_arg_var_0.x != ((0)))
    return 0;
  if (va_arg_var_0.y != ((1)))
    return 0;
  if (va_arg_var_0.bigint
      != ((((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
	   + 0xAACDFF9955555555ULL)))
    return 0;
  // CHECK_VARARG_STRUCT struct big_struct {x=(0),y=(1),bigint=(((unsigned
  // __int128)0xdeaddead12345678ULL) SHIFT_64 + 0xAACDFF9955555555ULL)}

  struct big_struct va_arg_var_1 = va_arg (ap, struct big_struct);

  if (va_arg_var_1.x != ((0)))
    return 0;
  if (va_arg_var_1.y != ((1)))
    return 0;
  if (va_arg_var_1.bigint
      != ((((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
	   + 0xAACDFF9955555555ULL)))
    return 0;
  // CHECK_VARARG_STRUCT struct big_struct {x=(0),y=(1),bigint=(((unsigned
  // __int128)0xdeaddead12345678ULL) SHIFT_64 + 0xAACDFF9955555555ULL)}

  struct big_struct va_arg_var_2 = va_arg (ap, struct big_struct);

  if (va_arg_var_2.x != ((0)))
    return 0;
  if (va_arg_var_2.y != ((1)))
    return 0;
  if (va_arg_var_2.bigint
      != ((((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
	   + 0xAACDFF9955555555ULL)))
    return 0;
  // CHECK_VARARG int (6)

  int va_arg_var_3 = va_arg (ap, int);
  if (va_arg_var_3 != (6))
    return 0;
  // CHECK_VARARG int (7)

  int va_arg_var_4 = va_arg (ap, int);
  if (va_arg_var_4 != (7))
    return 0;
  // CHECK_VARARG int (8)

  int va_arg_var_5 = va_arg (ap, int);
  if (va_arg_var_5 != (8))
    return 0;
  // CHECK_VARARG int (9)

  int va_arg_var_6 = va_arg (ap, int);
  if (va_arg_var_6 != (9))
    return 0;
  // CHECK_VARARG_STRUCT struct big_struct {x=(0),y=(1),bigint=(((unsigned
  // __int128)0xdeaddead12345678ULL) SHIFT_64 + 0xAACDFF9955555555ULL)}

  struct big_struct va_arg_var_7 = va_arg (ap, struct big_struct);

  if (va_arg_var_7.x != ((0)))
    return 0;
  if (va_arg_var_7.y != ((1)))
    return 0;
  if (va_arg_var_7.bigint
      != ((((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
	   + 0xAACDFF9955555555ULL)))
    return 0;
  // CHECK_VARARG int (11)

  int va_arg_var_8 = va_arg (ap, int);
  if (va_arg_var_8 != (11))
    return 0;
  // CHECK_VARARG int (12)

  int va_arg_var_9 = va_arg (ap, int);
  if (va_arg_var_9 != (12))
    return 0;
  // CHECK_VARARG int (13)

  int va_arg_var_10 = va_arg (ap, int);
  if (va_arg_var_10 != (13))
    return 0;
  return 1;
}

// only here to please the script, other function already took care of
// unwrapping args.

int
check_args_are_correct_user (int argument0, int argument1, int lastnamed, ...)
{

  return 1;
}

int
main (int argc, char **argv)
{

  struct big_struct foo = {0, 1,
			   ((unsigned __int128) 0xdeaddead12345678ULL) SHIFT_64
			     + 0xAACDFF9955555555ULL};

  return !check_args_are_correct (0, 1, 2, /* r0 r1 r2 */

				  foo, foo, foo, /* r3 r4 r5 */

				  6, 7, 8, 9, /* r6 r7 r8 r9 */

				  foo, /* r10 */

				  11, 12, 13); /* r11 stack stack*/
}
