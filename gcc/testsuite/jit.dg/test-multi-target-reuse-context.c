/* Retarget one context in place: build a function once, then compile
   the same context for a secondary, for another target, and for the
   first secondary again.  Every set_target call must steer the next
   compilation of the shared recording, and the first and third
   outputs must be byte-identical.  */

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
recompile_for (gcc_jit_context *ctxt, const char *triple,
	       const char *path)
{
  const char *first_error;
  gcc_jit_context_set_target (ctxt, triple);
  gcc_jit_context_compile_to_file (ctxt,
				   GCC_JIT_OUTPUT_KIND_ASSEMBLER,
				   path);
  first_error = gcc_jit_context_get_first_error (ctxt);
  if (first_error != NULL)
    {
      fail ("recompiling for %s: %s", triple, first_error);
      return 0;
    }
  pass ("recompiled for %s", triple);
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
  gcc_jit_context *ctxt;
  char *first_output, *second_output, *third_output;
  long first_size, second_size, third_size;

  if (secondaries == NULL || secondaries[0] == '\0')
    {
      untested ("not a multi-target compiler");
      totals ();
      return 0;
    }
  list = strdup (secondaries);
  first = strtok (list, " ");
  second = strtok (NULL, " ");

  ctxt = gcc_jit_context_acquire ();
  build_square (ctxt);

  if (!recompile_for (ctxt, first, "tmp-reuse-context-1.s")
      /* With a single secondary the middle compilation repeats the
	 first target; the retarget checks below then degrade to the
	 recompilation-determinism check alone.  */
      || !recompile_for (ctxt, second != NULL ? second : first,
			 "tmp-reuse-context-2.s")
      || !recompile_for (ctxt, first, "tmp-reuse-context-3.s"))
    {
      gcc_jit_context_release (ctxt);
      totals ();
      return 0;
    }
  gcc_jit_context_release (ctxt);

  first_output = read_whole_file ("tmp-reuse-context-1.s",
				  &first_size);
  second_output = read_whole_file ("tmp-reuse-context-2.s",
				   &second_size);
  third_output = read_whole_file ("tmp-reuse-context-3.s",
				  &third_size);
  if (first_output == NULL || second_output == NULL
      || third_output == NULL)
    {
      fail ("reading the outputs back");
      totals ();
      return 0;
    }

  /* A middle output identical to the first would mean the second
     set_target call did not steer the reused context.  */
  if (second != NULL)
    {
      if (first_size == second_size
	  && memcmp (first_output, second_output, first_size) == 0)
	fail ("retargeting the reused context left the output "
	      "unchanged");
      else
	pass ("retargeting the reused context changed the output");
    }

  if (first_size != third_size
      || memcmp (first_output, third_output, first_size) != 0)
    fail ("first and third compilations differ");
  else
    pass ("first and third compilations are byte-identical");

  totals ();
  return 0;
}
