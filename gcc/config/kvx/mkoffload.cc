/* Offload image generation tool for KVX.

   Copyright (C) 2014-2020 Free Software Foundation, Inc.

   Contributed by Kalray Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

/* Munges KVX assembly into a C source file defining the KVX code as a
   string.

   This is not a complete assembler.  We presume the source is well
   formed from the compiler and can die horribly if it is not.  */

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "obstack.h"
#include "diagnostic.h"
/* diagnostic.h:
    - extern const char *progname;
    - void diagnostic_initialize (diagnostic_context *, int);
*/
#include "intl.h"
#include <libgen.h>
#include "collect-utils.h"
#include "gomp-constants.h"

#include <assert.h>

/* {{{ Debug & Diagnostic  */

/* Flag to decide whether print to stderr information about what is going on.
   Set in init_debug depending on environment variables.  */

static bool kvx_mkoffload_debug;

/* Print a message to stderr if KVX_DEBUG value is set to true.  */

#define DEBUG_PRINT(...) \
  do \
  { \
    if (kvx_mkoffload_debug) \
      { \
	fprintf (stderr, __VA_ARGS__); \
      } \
  } \
  while (false);

/* Flush stderr if KVX_DEBUG value is set to true.  */

#define DEBUG_FLUSH()				\
  do {						\
    if (kvx_mkoffload_debug)			\
      fflush (stderr);				\
  } while (false)

/* Print a logging message with PREFIX to stderr if KVX_DEBUG value
   is set to true.  */

#define DEBUG_LOG(prefix, ...)			\
  do						\
    {						\
      DEBUG_PRINT (prefix);			\
      DEBUG_PRINT (__VA_ARGS__);		\
      DEBUG_FLUSH ();				\
    } while (false)

/* Print a debugging message to stderr.  */

#define KVX_DEBUG(...) DEBUG_LOG ("KVX debug: ", __VA_ARGS__)

/* Print a warning message to stderr.  */

#define KVX_WARNING(...) DEBUG_LOG ("KVX warning: ", __VA_ARGS__)

/* }}}  */

const char tool_name[] = "kvx-mkoffload";

/* Files to unlink.  */
static const char *kvx_s1_name;
static const char *kvx_s2_name;
static const char *kvx_o_name;
static const char *kvx_cfile_name;

enum offload_abi offload_abi = OFFLOAD_ABI_LP64;

/* Delete tempfiles.  */

void
tool_cleanup (bool from_signal ATTRIBUTE_UNUSED)
{
  if (kvx_cfile_name)
    maybe_unlink (kvx_cfile_name);
  if (kvx_s1_name)
    maybe_unlink (kvx_s1_name);
  if (kvx_s2_name)
    maybe_unlink (kvx_s2_name);
  if (kvx_o_name)
    maybe_unlink (kvx_o_name);
}

static void
mkoffload_cleanup (void)
{
  tool_cleanup (false);
}

/* Unlink FILE unless requested otherwise.  */

void
maybe_unlink (const char *file)
{
  if (!save_temps)
    {
      if (unlink_if_ordinary (file) && errno != ENOENT)
	fatal_error (input_location, "deleting file %s: %m", file);
    }
  else if (verbose)
    fprintf (stderr, "[Leaving %s]\n", file);
}

/* Add or change the value of an environment variable, outputting the
   change to standard error if in verbose mode.  */
static void
xputenv (const char *string)
{
  if (verbose)
    fprintf (stderr, "%s\n", string);
  putenv (CONST_CAST (char *, string));
}

/* Read the whole input file.  It will be NUL terminated (but
   remember, there could be a NUL in the file itself.  */
static const char *
read_file (FILE * stream, size_t * plen)
{
  size_t alloc = 16384;
  size_t base = 0;
  char *buffer;

  if (!fseek (stream, 0, SEEK_END))
    {
      /* Get the file size.  */
      long s = ftell (stream);
      if (s >= 0)
	alloc = s + 100;
      fseek (stream, 0, SEEK_SET);
    }
  buffer = XNEWVEC (char, alloc);

  for (;;)
    {
      size_t n = fread (buffer + base, 1, alloc - base - 1, stream);

      if (!n)
	break;
      base += n;
      if (base + 1 == alloc)
	{
	  alloc *= 2;
	  buffer = XRESIZEVEC (char, buffer, alloc);
	}
    }
  buffer[base] = 0;
  *plen = base;
  return buffer;
}

/* Parse STR, saving found tokens into PVALUES and return their number.
   Tokens are assumed to be delimited by ':'.  */
static unsigned
parse_env_var (const char *str, char ***pvalues)
{
  const char *curval, *nextval;
  char **values;
  unsigned num = 1, i;

  curval = strchr (str, ':');
  while (curval)
    {
      num++;
      curval = strchr (curval + 1, ':');
    }

  values = (char **) xmalloc (num * sizeof (char *));
  curval = str;
  nextval = strchr (curval, ':');
  if (nextval == NULL)
    nextval = strchr (curval, '\0');

  for (i = 0; i < num; i++)
    {
      int l = nextval - curval;
      values[i] = (char *) xmalloc (l + 1);
      memcpy (values[i], curval, l);
      values[i][l] = 0;
      curval = nextval + 1;
      nextval = strchr (curval, ':');
      if (nextval == NULL)
	nextval = strchr (curval, '\0');
    }
  *pvalues = values;
  return num;
}

/* Auxiliary function that frees elements of PTR and PTR itself.
   N is number of elements to be freed.  If PTR is NULL, nothing is freed.
   If an element is NULL, subsequent elements are not freed.  */
static void
free_array_of_ptrs (void **ptr, unsigned n)
{
  unsigned i;
  if (!ptr)
    return;
  for (i = 0; i < n; i++)
    {
      if (!ptr[i])
	break;
      free (ptr[i]);
    }
  free (ptr);
  return;
}

/* Check whether NAME can be accessed in MODE.  This is like access,
   except that it never considers directories to be executable.  */
static int
access_check (const char *name, int mode)
{
  if (mode == X_OK)
    {
      struct stat st;

      if (stat (name, &st) < 0 || S_ISDIR (st.st_mode))
	return -1;
    }

  return access (name, mode);
}

static void
process_asm (FILE * in, FILE * out, FILE * cfile)
{
  int fn_count = 0, var_count = 0;
  struct obstack fns_os, vars_os, varsizes_os;
  obstack_init (&fns_os);
  obstack_init (&vars_os);
  obstack_init (&varsizes_os);

  char buf[1000];
  enum
  { IN_CODE, IN_VARS, IN_FUNCS } state = IN_CODE;
  while (fgets (buf, sizeof (buf), in))
    {
      switch (state)
	{
	case IN_CODE:
	  {
	    char funname[256] = { 0 };
	    if (sscanf (buf, " .type %256[^,], @function", funname) > 0)
	      {
		/*  TODO: ??? Is this really needed even though we are compiling
		   with -shared?
		   or can we have the compiler auto export them? */
		fprintf (out, "\t.global %s\n", funname);
	      }
	    break;
	  }
	case IN_VARS:
	  {
	    char varname[256] = { 0 };
	    unsigned varsize;
	    if (sscanf (buf, " .8byte %s\n", varname) > 0)
	      {
		obstack_ptr_grow (&vars_os, xstrdup (varname));
		fgets (buf, sizeof (buf), in);
		if (sscanf (buf, " .8byte %u\n", &varsize) < 0)
		  abort ();
		obstack_int_grow (&varsizes_os, varsize);
		var_count++;
		KVX_DEBUG ("variable %s (size %u)\n", varname, varsize);

		KVX_DEBUG ("\t.global %s\n", varname);
	      }
	    break;
	  }
	case IN_FUNCS:
	  {
	    char funcname[256] = { 0 };
	    if (sscanf (buf, " .8byte\t%s\n", funcname) > 0)
	      {
		KVX_DEBUG ("func. %s\n", funcname);
		obstack_ptr_grow (&fns_os, xstrdup (funcname));
		fn_count++;
		continue;
	      }
	    break;
	  }
	default:
	  assert (0 || "wrong state");
	}

      char dummy;
      if (sscanf (buf, " .section .gnu.offload_vars%c", &dummy) > 0)
	state = IN_VARS;
      else if (sscanf (buf, " .section .gnu.offload_funcs%c", &dummy) > 0)
	state = IN_FUNCS;
      else if (sscanf (buf, " .section %c", &dummy) > 0
	       || sscanf (buf, " .text%c", &dummy) > 0
	       || sscanf (buf, " .bss%c", &dummy) > 0
	       || sscanf (buf, " .data%c", &dummy) > 0
	       || sscanf (buf, " .ident %c", &dummy) > 0)
	state = IN_CODE;

      if (state == IN_CODE)
	fputs (buf, out);
    }

  char **fns = XOBFINISH (&fns_os, char **);

  fprintf (cfile, "#include <stdlib.h>\n");
  fprintf (cfile, "#include <stdbool.h>\n\n");

  char **vars = XOBFINISH (&vars_os, char **);
  unsigned *varsizes = XOBFINISH (&varsizes_os, unsigned *);
  fprintf (cfile,
	   "static const struct global_var_info {\n"
	   "  const char *name;\n"
	   "  const size_t size;\n" "  void *address;\n" "} vars[] = {\n");
  int i;
  for (i = 0; i < var_count; ++i)
    {
      const char *sep = i < var_count - 1 ? "," : "";
      fprintf (cfile, "  { \"%s\", %u, NULL }%s\n",
	       vars[i], varsizes[i], sep);
    }
  fprintf (cfile, "};\n\n");

  obstack_free (&vars_os, NULL);
  obstack_free (&varsizes_os, NULL);

  /* Dump out function idents.  */
  fprintf (cfile, "static const struct kvx_kernel_description {\n"
	   "  const char *name;\n" "} kvx_kernels[] = {\n  ");
  for (int i = 0; i < fn_count; ++i)
    {
      fprintf (cfile, " \"%s\",\n", fns[i]);
    }
  fprintf (cfile, "\n};\n\n");
  obstack_free (&fns_os, NULL);
}

static void
process_obj (FILE * in, FILE * cfile)
{
  size_t len = 0;
  const char *input = read_file (in, &len);

  /* Dump out an array containing the binary.
     FIXME: do this with objcopy.  */
  fprintf (cfile, "static unsigned char kvx_code[] = {");
  for (size_t i = 0; i < len; i += 17)
    {
      fprintf (cfile, "\n\t");
      for (size_t j = i; j < i + 17 && j < len; j++)
	fprintf (cfile, "%3u,", (unsigned char) input[j]);
    }
  fprintf (cfile, "\n};\n\n");

  fprintf (cfile,
	   "static const struct kvx_image {\n"
	   "  size_t size;\n"
	   "  void *image;\n"
	   "} kvx_image = {\n" "  %zu,\n" "  kvx_code\n" "};\n\n", len);

  fprintf (cfile,
	   "static const struct kvx_image_desc {\n"
	   "  const struct kvx_image *kvx_image;\n"
	   "  unsigned kernel_count;\n"
	   "  const struct kvx_kernel_description *kernel_infos;\n"
	   "  unsigned global_variable_count;\n"
	   "  const struct global_var_info *global_variables;\n"
	   "} target_data = {\n"
	   "  &kvx_image,\n"
	   "  sizeof (kvx_kernels) / sizeof (kvx_kernels[0]),\n"
	   "  kvx_kernels,"
	   "  sizeof (vars) / sizeof (vars[0]),\n" "  vars\n" "};\n\n");

  fprintf (cfile,
	   "#ifdef __cplusplus\n"
	   "extern \"C\" {\n"
	   "#endif\n"
	   "extern void GOMP_offload_register_ver"
	   " (unsigned, const void *, int, const void *);\n"
	   "extern void GOMP_offload_unregister_ver"
	   " (unsigned, const void *, int, const void *);\n"
	   "#ifdef __cplusplus\n" "}\n" "#endif\n\n");

  fprintf (cfile, "extern const void *const __OFFLOAD_TABLE__[];\n\n");

  fprintf (cfile, "static __attribute__((constructor)) void init (void)\n"
	   "{\n"
	   "  GOMP_offload_register_ver (%#x, __OFFLOAD_TABLE__,"
	   " %d/*KVX*/, &target_data);\n"
	   "};\n", GOMP_VERSION_PACK (GOMP_VERSION, GOMP_VERSION /*_KVX*/ ),
	   GOMP_DEVICE_KALRAY_KVX);

  fprintf (cfile, "static __attribute__((destructor)) void fini (void)\n"
	   "{\n"
	   "  GOMP_offload_unregister_ver (%#x, __OFFLOAD_TABLE__,"
	   " %d/*KVX*/, &target_data);\n"
	   "};\n", GOMP_VERSION_PACK (GOMP_VERSION, GOMP_VERSION /*_KVX*/ ),
	   GOMP_DEVICE_KALRAY_KVX);
}

static void
compile_native (const char *infile, const char *outfile, const char *compiler)
{
  const char *collect_gcc_options = getenv ("COLLECT_GCC_OPTIONS");
  if (!collect_gcc_options)
    fatal_error (input_location,
		 "environment variable COLLECT_GCC_OPTIONS must be set");

  struct obstack argv_obstack;
  obstack_init (&argv_obstack);
  obstack_ptr_grow (&argv_obstack, compiler);
  if (save_temps)
    obstack_ptr_grow (&argv_obstack, "-save-temps");
  if (verbose)
    obstack_ptr_grow (&argv_obstack, "-v");
  switch (offload_abi)
    {
    case OFFLOAD_ABI_LP64:
      obstack_ptr_grow (&argv_obstack, "-m64");
      break;
    case OFFLOAD_ABI_ILP32:
      obstack_ptr_grow (&argv_obstack, "-m32");
      break;
    default:
      gcc_unreachable ();
    }
  obstack_ptr_grow (&argv_obstack, infile);
  obstack_ptr_grow (&argv_obstack, "-c");
  obstack_ptr_grow (&argv_obstack, "-o");
  obstack_ptr_grow (&argv_obstack, outfile);
  obstack_ptr_grow (&argv_obstack, NULL);

  const char **new_argv = XOBFINISH (&argv_obstack, const char **);
  fork_execute (new_argv[0], CONST_CAST (char **, new_argv), true,
		".gccnative_args");
  obstack_free (&argv_obstack, NULL);
}

int
main (int argc, char **argv)
{
  FILE *in = stdin, *out = stdout, *cfile = stdout;
  const char *outname = NULL, *offloadsrc = NULL;

  progname = "mkoffload";
  diagnostic_initialize (global_dc, 0);

  kvx_s1_name = make_temp_file (".mkoffload.1.s");
  kvx_s2_name = make_temp_file (".mkoffload.2.s");
  kvx_o_name = make_temp_file (".mkoffload.kvxco");
  kvx_cfile_name = make_temp_file (".c");

  if (secure_getenv ("MKOFFLOAD_DEBUG"))
    kvx_mkoffload_debug = true;
  else
    kvx_mkoffload_debug = false;

  if (atexit (mkoffload_cleanup) != 0)
    fatal_error (input_location, "atexit failed");

  char *collect_gcc = getenv ("COLLECT_GCC");
  if (collect_gcc == NULL)
    fatal_error (input_location, "COLLECT_GCC must be set.");
  const char *gcc_path = dirname (xstrdup (collect_gcc));
  const char *gcc_exec = basename (xstrdup (collect_gcc));

  size_t len = (strlen (gcc_path) + 1 + strlen (GCC_INSTALL_NAME) + 1);
  char *driver = XALLOCAVEC (char, len);

  if (strcmp (gcc_exec, collect_gcc) == 0)
    /* collect_gcc has no path, so it was found in PATH.  Make sure we also
       find accel-gcc in PATH.  */
    gcc_path = NULL;

  int driver_used = 0;
  if (gcc_path != NULL)
    driver_used = sprintf (driver, "%s/", gcc_path);
  sprintf (driver + driver_used, "%s", GCC_INSTALL_NAME);

  bool found = false;
  if (gcc_path == NULL)
    found = true;
  else if (access_check (driver, X_OK) == 0)
    found = true;
  else
    {
      /* Don't use alloca pointer with XRESIZEVEC.  */
      driver = NULL;
      /* Look in all COMPILER_PATHs for GCC_INSTALL_NAME.  */
      char **paths = NULL;
      unsigned n_paths;
      n_paths = parse_env_var (getenv ("COMPILER_PATH"), &paths);
      for (unsigned i = 0; i < n_paths; i++)
	{
	  len = strlen (paths[i]) + 1 + strlen (GCC_INSTALL_NAME) + 1;
	  driver = XRESIZEVEC (char, driver, len);
	  sprintf (driver, "%s/%s", paths[i], GCC_INSTALL_NAME);
	  if (access_check (driver, X_OK) == 0)
	    {
	      found = true;
	      break;
	    }
	}
      free_array_of_ptrs ((void **) paths, n_paths);
    }

  if (!found)
    fatal_error (input_location,
		 "offload compiler %s not found (consider using %<-B%>)",
		 GCC_INSTALL_NAME);

  /* We may be called with all the arguments stored in some file and
     passed with @file.  Expand them into argv before processing.  */
  expandargv (&argc, &argv);

  /* Scan the argument vector.  */
  bool fopenmp = false;
  bool fopenacc = false;
  for (int i = 1; i < argc; i++)
    {
#define STR "-foffload-abi="
      if (strncmp (argv[i], STR, strlen (STR)) == 0)
	{
	  if (strcmp (argv[i] + strlen (STR), "lp64") == 0)
	    offload_abi = OFFLOAD_ABI_LP64;
	  else if (strcmp (argv[i] + strlen (STR), "ilp32") == 0)
	    offload_abi = OFFLOAD_ABI_ILP32;
	  else
	    fatal_error (input_location,
			 "unrecognizable argument of option " STR);
	}
#undef STR
      else if (strcmp (argv[i], "-fopenmp") == 0)
	fopenmp = true;
      else if (strcmp (argv[i], "-fopenacc") == 0)
	fopenacc = true;
      else if (strcmp (argv[i], "-save-temps") == 0)
	save_temps = true;
      else if (strcmp (argv[i], "-v") == 0)
	verbose = true;
    }
  if (!(fopenacc ^ fopenmp))
    fatal_error (input_location, "either %<-fopenacc%> or %<-fopenmp%> "
		 "must be set");

  /* Build argument for compiler pass */
  struct obstack cc_argv_obstack;
  obstack_init (&cc_argv_obstack);
  obstack_ptr_grow (&cc_argv_obstack, driver);
  obstack_ptr_grow (&cc_argv_obstack, "-shared");
  obstack_ptr_grow (&cc_argv_obstack, "-S");
  obstack_ptr_grow (&cc_argv_obstack, "-xlto");
//  if (fopenmp)
//    obstack_ptr_grow (&cc_argv_obstack, "-mgomp");

  for (int ix = 1; ix != argc; ix++)
    {
      if (!strcmp (argv[ix], "-o") && ix + 1 != argc)
	outname = argv[++ix];
      else
	{
	  obstack_ptr_grow (&cc_argv_obstack, argv[ix]);

	  if (argv[ix][0] != '-')
	    offloadsrc = argv[ix];
	}
    }

  obstack_ptr_grow (&cc_argv_obstack, "-fpic");
  obstack_ptr_grow (&cc_argv_obstack, "-o");
  obstack_ptr_grow (&cc_argv_obstack, kvx_s1_name);
  obstack_ptr_grow (&cc_argv_obstack, NULL);
  const char **cc_argv = XOBFINISH (&cc_argv_obstack, const char **);

  /* Build arguments for assemble/link pass */
  struct obstack ld_argv_obstack;
  obstack_init (&ld_argv_obstack);
  obstack_ptr_grow (&ld_argv_obstack, driver);
  obstack_ptr_grow (&ld_argv_obstack, "-shared");
//  obstack_ptr_grow (&ld_argv_obstack, "-c");
  obstack_ptr_grow (&ld_argv_obstack, kvx_s2_name);
//  obstack_ptr_grow (&ld_argv_obstack, "-lgomp");

  for (int i = 1; i < argc; i++)
    if (strncmp (argv[i], "-l", 2) == 0
	|| strncmp (argv[i], "-Wl", 3) == 0
	|| strncmp (argv[i], "-march", 6) == 0)
      obstack_ptr_grow (&ld_argv_obstack, argv[i]);

  obstack_ptr_grow (&ld_argv_obstack, "-o");
  obstack_ptr_grow (&ld_argv_obstack, kvx_o_name);
  obstack_ptr_grow (&ld_argv_obstack, NULL);
  if (save_temps)
    obstack_ptr_grow (&ld_argv_obstack, "-save-temps");
  if (verbose)
    obstack_ptr_grow (&ld_argv_obstack, "-v");
  const char **ld_argv = XOBFINISH (&ld_argv_obstack, const char **);

  cfile = fopen (kvx_cfile_name, "w");
  if (!cfile)
    fatal_error (input_location, "cannot open '%s'", kvx_cfile_name);

  /* PR libgomp/65099: Currently, we only support offloading in 64-bit
     configurations.  */
  if (offload_abi == OFFLOAD_ABI_LP64)
    {
      /* Clean up unhelpful environment variables.  */
      char *execpath = getenv ("GCC_EXEC_PREFIX");
      char *cpath = getenv ("COMPILER_PATH");
      char *lpath = getenv ("LIBRARY_PATH");
      unsetenv ("GCC_EXEC_PREFIX");
      unsetenv ("COMPILER_PATH");
      unsetenv ("LIBRARY_PATH");

      fork_execute (cc_argv[0], CONST_CAST (char **, cc_argv), true,
		    ".gcc_args");
      obstack_free (&cc_argv_obstack, NULL);

      in = fopen (kvx_s1_name, "r");
      if (!in)
	fatal_error (input_location, "cannot open intermediate kvx asm file");

      out = fopen (kvx_s2_name, "w");
      if (!out)
	fatal_error (input_location, "cannot open '%s'", kvx_s2_name);

      process_asm (in, out, cfile);

      fclose (in);
      fclose (out);

      /* Run the assemble/link pass.  */
      fork_execute (ld_argv[0], CONST_CAST (char **, ld_argv), true,
		    ".ld_args");
      obstack_free (&ld_argv_obstack, NULL);

      in = fopen (kvx_o_name, "r");
      if (!in)
	fatal_error (input_location, "cannot open intermediate kvx obj file");

      process_obj (in, cfile);

      fclose (in);

      xputenv (concat ("GCC_EXEC_PREFIX=", execpath, NULL));
      xputenv (concat ("COMPILER_PATH=", cpath, NULL));
      xputenv (concat ("LIBRARY_PATH=", lpath, NULL));
    }

  fclose (cfile);

  compile_native (kvx_cfile_name, outname, collect_gcc);

  return 0;
}
