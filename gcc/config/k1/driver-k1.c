/* Subroutines for the gcc driver.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include <stdlib.h>
#include <unistd.h>

const char *
k1_tooldir (int argc, const char **argv)
{
  int len, i;
  char *res, *res2;
  const char *gcc_exec_prefix = STANDARD_EXEC_PREFIX;
  const char dir_separator[] = {DIR_SEPARATOR, 0};

  if (getenv ("GCC_EXEC_PREFIX"))
    {
      gcc_exec_prefix = getenv ("GCC_EXEC_PREFIX");
    }

  res = concat (gcc_exec_prefix, dir_separator, DEFAULT_TARGET_MACHINE,
		dir_separator, DEFAULT_TARGET_VERSION, dir_separator,
		TOOLDIR_BASE_PREFIX, NULL);

  for (i = 0; i < argc; ++i)
    res = concat (res, dir_separator, argv[i], NULL);

  len = strlen (res);

  if (!access (res, R_OK | X_OK))
    {
      if (len && res[len - 1] == ' ')
	res[len - 1] = DIR_SEPARATOR;
      return res;
    }

  res2 = concat (STANDARD_EXEC_PREFIX, dir_separator, DEFAULT_TARGET_MACHINE,
		 dir_separator, DEFAULT_TARGET_VERSION, dir_separator,
		 TOOLDIR_BASE_PREFIX, NULL);

  for (i = 0; i < argc; ++i)
    res2 = concat (res2, dir_separator, argv[i], NULL);

  len = strlen (res2);

  if (!access (res2, R_OK | X_OK))
    {
      if (len && res2[len - 1] == ' ')
	res2[len - 1] = DIR_SEPARATOR;
      return res2;
    }

  len = strlen (res);
  if (res[len - 1] == ' ')
    res[len - 1] = DIR_SEPARATOR;
  return res;
}

const char *
k1_concat (int argc, const char **argv)
{
  int len, i;
  char *res = "";
  const char dir_separator[] = {DIR_SEPARATOR, 0};

  for (i = 0; i < argc; ++i)
    res = concat (res, dir_separator, argv[i], NULL);

  len = strlen (res);
  if (res[len - 1] == ' ')
    res[len - 1] = DIR_SEPARATOR;
  return res;
}

const char *
k1_post_suffix_soc (int argc, const char **argv)
{
  /* compute size */
  int length = 0;
  unsigned int i;
  const char *spec_soc = argv[0];
  size_t spec_soc_len = strlen (spec_soc);
  char *ret_string;

  for (i = 1; i < argc; i++)
    {
      length += strlen (argv[i]);
      if (!(i % 2))
	{
	  length += spec_soc_len;
	}
      length++; // space
    }

  ret_string = (char *) xcalloc (length + 1, 1);
  if (ret_string == NULL)
    {
      return "%eError when building soc specific paths\n";
    }

  for (i = 1; i < argc; i++)
    {
      strcat (ret_string, " ");
      strcat (ret_string, argv[i]);
      if (!(i % 2))
	{
	  strcat (ret_string, spec_soc);
	}
    }

  return ret_string;
}

const char *
k1_board_to_startfile_prefix (int argc, const char **argv)
{
  int len;
  char *res;
  if (0 == argc)
    return "";

  /* We just want to add a trailing / to our
     startfile_prefixes. This should really be expressed directly in
     spec, but it adds an unwanted space in the path. Thus this
     hack... */

  len = strlen (argv[0]);
  res = concat (argv[0], "/", NULL);
  if (res[len - 1] == ' ')
    res[len - 1] = '/';

  /* Seamlessly change the bsp to csp_generic if developer, emb01, ab04,
   * konic80, or tc */
  if (strcmp (res, "developer/") == 0 || strcmp (res, "emb01/") == 0
      || strcmp (res, "ab04/") == 0 || strcmp (res, "tc2/") == 0
      || strcmp (res, "tc3/") == 0 || strcmp (res, "konic80/") == 0)
    {
      res = "csp_generic/";
      // Switch board name at the same time
      k1_board_name = "csp_generic";
    }

  return res;
}

const char *
k1_cluster_to_define (int argc, const char **argv)
{
  int len;
  char *res;

  if (0 == argc)
    return "";

  /* Again, to remove the unwanted trailing whitespace. */

  res = concat ("-D__", argv[argc - 1], NULL);
  len = strlen (res);
  if (res[len - 1] == ' ')
    res[len - 1] = 0;
  res = concat (res, "__", NULL);

  return res;
}

const char *
k1_cluster_board_to_bsp (int argc, const char **argv)
{
  int len;
  char *res;

  if (3 > argc)
    return "";

  /* Again, to remove the unwanted trailing whitespace. */

  res = concat (argv[0], NULL);
  len = strlen (res);
  if (res[len - 1] == ' ')
    res[len - 1] = 0;

  /* Ugly hack to handle the case when the -mprocessing option is */
  /* passed multiple times. Second argv may then be concatenated */
  /* multiple times so we directly use here the two last arguments. */
  res = concat (res, "_", argv[argc - 2], NULL);
  len = strlen (res);
  if (res[len - 1] == ' ')
    res[len - 1] = 0;

  res = concat (res, argv[argc - 1], NULL);
  len = strlen (res);
  if (res[len - 1] == ' ')
    res[len - 1] = 0;

  /* Seamlessly change the bsp to csp_generic if developer, emb01, ab04,
   * konic80, or tc */
  if (!strcmp (res, "developer") || !strcmp (res, "emb01")
      || !strcmp (res, "ab04") || !strcmp (res, "tc2") || !strcmp (res, "tc3")
      || !strcmp (res, "konic80"))
    {
      fprintf (stderr,
	       "board_to_bsp: Changing board type from %s to csp_generic\n",
	       res);
      res = "csp_generic";
    }

  return res;
}
