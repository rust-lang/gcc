/* An unknown target triple is an immediate error on the context,
   under any build: a single-target compiler accepts exactly its
   own triple.  */

#include <stdlib.h>
#include <string.h>

#include "libgccjit.h"

#define TEST_PROVIDES_MAIN
#define TEST_ESCHEWS_TEST_JIT
#define TEST_ESCHEWS_SET_OPTIONS
#include "harness.h"

int
main (int argc, char **argv)
{
  gcc_jit_context *ctxt = gcc_jit_context_acquire ();
  const char *first_error;

  gcc_jit_context_set_target (ctxt, "not-a-target-triple");
  first_error = gcc_jit_context_get_first_error (ctxt);
  if (first_error == NULL
      || strstr (first_error, "unknown target") == NULL)
    fail ("unknown triple not rejected (error: %s)",
	  first_error != NULL ? first_error : "(none)");
  else
    pass ("unknown triple rejected");

  gcc_jit_context_release (ctxt);
  totals ();
  return 0;
}
