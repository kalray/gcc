#include <stdio.h>
#include <string.h>
#include <k1c-regs.h>

//compare (kdiff3) the output of this program with the result of the following command:
//awk '{print $2 " " $1 " " $4}' <result of maintenance print raw-registers command in gdb>

static int
is_reg_reserved (char *name)
{
  static const char *reserved_regs[] = {"res", "vsfr"};
  static const int no_reserved_regs = sizeof (reserved_regs) / sizeof (reserved_regs[0]);
  int i;

  for (i = 0; i < no_reserved_regs; i++)
    if (name && !strncmp (name, reserved_regs[i], strlen (reserved_regs[i])))
        return 1;

  return 0;
}


int main (int argc, char **argv)
{
  int i, j;
  struct reg_desc *rd;
  int *sz, ofs;
  const char *arch, *last_reg;

  if (argc < 3)
  {
    fprintf (stderr, "Syntax: %s <k1_arch>  <last_reg>\n", argv[0]);
    return 1;
  }

  arch = argv[1];
  last_reg = argv[2];
  rd = get_register_descriptions (arch);
  sz = get_register_sizes (arch);
  if (!rd || !sz)
  {
    fprintf (stderr, "No register found for arch %s\n", arch);
    return 1;
  }

  ofs = 0;
  for (i = 0, j = 0; rd[i].name; i++)
  {
    if (is_reg_reserved (rd[i].name))
      continue;

    printf("%d %s %d\n", j, rd[i].name, ofs);
    ofs += sz[i] / 8;
    j++;

    if (!strcmp (rd[i].name, last_reg))
      break;
  }

  return 0;
}
