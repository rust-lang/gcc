/* Compile for one target, for another, then for the first again,
   all in a single process: the third compilation must reproduce
   the first byte for byte.  The secondary sequence is bracketed by
   default-target compilations, which must also match each other:
   switching away and back must leave the unselected configuration
   undisturbed.  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libgccjit.h"

#define TEST_PROVIDES_MAIN
#define TEST_ESCHEWS_TEST_JIT
#define TEST_ESCHEWS_SET_OPTIONS
#include "harness.h"

static void
build_square (gcc_jit_context *ctxt)
{
  gcc_jit_type *int_type
    = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_param *param
    = gcc_jit_context_new_param (ctxt, NULL, int_type, "i");
  gcc_jit_function *fn
    = gcc_jit_context_new_function (ctxt, NULL,
				    GCC_JIT_FUNCTION_EXPORTED,
				    int_type, "square", 1, &param, 0);
  gcc_jit_block *block = gcc_jit_function_new_block (fn, NULL);
  gcc_jit_rvalue *expr
    = gcc_jit_context_new_binary_op (ctxt, NULL,
				     GCC_JIT_BINARY_OP_MULT, int_type,
				     gcc_jit_param_as_rvalue (param),
				     gcc_jit_param_as_rvalue (param));
  gcc_jit_block_end_with_return (block, NULL, expr);
}

static int
compile_square_for (const char *triple, const char *path)
{
  gcc_jit_context *ctxt = gcc_jit_context_acquire ();
  const char *first_error;
  if (triple != NULL)
    gcc_jit_context_set_target (ctxt, triple);
  build_square (ctxt);
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
  char *default_before, *default_after;
  long first_size, third_size, before_size, after_size;

  if (secondaries == NULL || secondaries[0] == '\0')
    {
      untested ("not a multi-target compiler");
      totals ();
      return 0;
    }
  list = strdup (secondaries);
  first = strtok (list, " ");
  second = strtok (NULL, " ");

  if (!compile_square_for (NULL, "tmp-two-contexts-0.s")
      || !compile_square_for (first, "tmp-two-contexts-1.s")
      /* With a single secondary the middle compilation uses the
	 default target; the switch away and back is what counts.  */
      || !compile_square_for (second, "tmp-two-contexts-2.s")
      || !compile_square_for (first, "tmp-two-contexts-3.s")
      || !compile_square_for (NULL, "tmp-two-contexts-4.s"))
    {
      totals ();
      return 0;
    }

  first_output = read_whole_file ("tmp-two-contexts-1.s",
				  &first_size);
  third_output = read_whole_file ("tmp-two-contexts-3.s",
				  &third_size);
  if (first_output == NULL || third_output == NULL)
    fail ("reading the outputs back");
  else if (first_size != third_size
	   || memcmp (first_output, third_output, first_size) != 0)
    fail ("first and third compilations differ");
  else
    pass ("first and third compilations are byte-identical");

  default_before = read_whole_file ("tmp-two-contexts-0.s",
				    &before_size);
  default_after = read_whole_file ("tmp-two-contexts-4.s",
				   &after_size);
  if (default_before == NULL || default_after == NULL)
    fail ("reading the default outputs back");
  else if (before_size != after_size
	   || memcmp (default_before, default_after,
		      before_size) != 0)
    fail ("default compilations before and after differ");
  else
    pass ("default compilations before and after are "
	  "byte-identical");

  totals ();
  return 0;
}
