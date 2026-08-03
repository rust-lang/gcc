/* The long double format is the sharpest shared-state hazard of
   target switching: x86-64 and m68k both keep an 80-bit extended
   float in XFmode, laid out differently, and the layout installs at
   activation time.  Compile a long double constant for one target,
   for another, then for the first again: the outputs of the first
   and third compilations must be byte-identical, constant words
   included.  A leaked format produces silently wrong bits, not a
   crash.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libgccjit.h"

#define TEST_PROVIDES_MAIN
#define TEST_ESCHEWS_TEST_JIT
#define TEST_ESCHEWS_SET_OPTIONS
#include "harness.h"

static void
build_long_double_constant (gcc_jit_context *ctxt)
{
  gcc_jit_type *long_double_type
    = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_LONG_DOUBLE);
  gcc_jit_function *fn
    = gcc_jit_context_new_function (ctxt, NULL,
				    GCC_JIT_FUNCTION_EXPORTED,
				    long_double_type,
				    "get_two_and_a_half", 0, NULL,
				    0);
  gcc_jit_block *block = gcc_jit_function_new_block (fn, NULL);
  gcc_jit_rvalue *value
    = gcc_jit_context_new_rvalue_from_double (ctxt,
					      long_double_type,
					      2.5);
  gcc_jit_block_end_with_return (block, NULL, value);
}

static int
compile_constant_for (const char *triple, const char *path)
{
  gcc_jit_context *ctxt = gcc_jit_context_acquire ();
  const char *first_error;
  if (triple != NULL)
    gcc_jit_context_set_target (ctxt, triple);
  build_long_double_constant (ctxt);
  gcc_jit_context_compile_to_file (ctxt,
				   GCC_JIT_OUTPUT_KIND_ASSEMBLER,
				   path);
  first_error = gcc_jit_context_get_first_error (ctxt);
  if (first_error != NULL)
    {
      fail ("compiling for %s: %s",
	    triple != NULL ? triple : "(default)", first_error);
      gcc_jit_context_release (ctxt);
      return 0;
    }
  pass ("compiled for %s", triple != NULL ? triple : "(default)");
  gcc_jit_context_release (ctxt);
  return 1;
}

static char *
read_whole_file (const char *path, long *size_out)
{
  FILE *f = fopen (path, "rb");
  char *buffer;
  long size;
  if (f == NULL)
    return NULL;
  fseek (f, 0, SEEK_END);
  size = ftell (f);
  fseek (f, 0, SEEK_SET);
  buffer = (char *) malloc (size);
  if (buffer == NULL || fread (buffer, 1, size, f) != (size_t) size)
    {
      fclose (f);
      free (buffer);
      return NULL;
    }
  fclose (f);
  *size_out = size;
  return buffer;
}

int
main (int argc, char **argv)
{
  const char *secondaries = getenv ("JIT_MULTI_TARGET_SECONDARIES");
  char *list, *first, *second;
  char *first_output, *third_output;
  long first_size, third_size;

  if (secondaries == NULL || secondaries[0] == '\0')
    {
      untested ("not a multi-target compiler");
      totals ();
      return 0;
    }
  list = strdup (secondaries);
  first = strtok (list, " ");
  second = strtok (NULL, " ");

  if (!compile_constant_for (first, "tmp-longdouble-1.s")
      /* With a single secondary the middle compilation uses the
	 default target; its long double format is what must not
	 leak into the third.  */
      || !compile_constant_for (second, "tmp-longdouble-2.s")
      || !compile_constant_for (first, "tmp-longdouble-3.s"))
    {
      totals ();
      return 0;
    }

  first_output = read_whole_file ("tmp-longdouble-1.s", &first_size);
  third_output = read_whole_file ("tmp-longdouble-3.s", &third_size);
  if (first_output == NULL || third_output == NULL)
    fail ("reading the outputs back");
  else if (first_size != third_size
	   || memcmp (first_output, third_output, first_size) != 0)
    fail ("long double emission changed after switching targets");
  else
    pass ("long double emission is byte-identical after switching");

  totals ();
  return 0;
}
