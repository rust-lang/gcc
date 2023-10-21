/* { dg-do compile { target x86_64-*-* } } */

#include <stdlib.h>
#include <stdio.h>

#include "libgccjit.h"

#define TEST_PROVIDES_MAIN
#include "harness.h"

/* If the generated code compiles, it means that the `target` attribute was correctly set,
   otherwise it would emit an error saying that it doesn't know the `__builtin_ia32_loadupd`
   builtin. */
void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:
__attribute__((target("sse2")))
void foo () {
  __builtin_ia32_loadupd(NULL);
}
  */
  gcc_jit_type *double_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_DOUBLE);
  gcc_jit_type *pdouble_type =  gcc_jit_type_get_pointer (double_type);
  gcc_jit_type *void_type = gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_VOID);

  /* Creating the `foo` function. */
  gcc_jit_function *foo_func =
    gcc_jit_context_new_function (ctxt, NULL,
				  GCC_JIT_FUNCTION_EXPORTED,
				  void_type,
				  "foo",
				  0, NULL,
				  0);

  /* __attribute__((target("sse3"))) */
  gcc_jit_function_add_string_attribute (
    foo_func,
    GCC_JIT_FN_ATTRIBUTE_TARGET,
    "sse2");

  gcc_jit_block *foo_block = gcc_jit_function_new_block (foo_func, NULL);

  gcc_jit_function *builtin = gcc_jit_context_get_target_builtin_function (
    ctxt, "__builtin_ia32_loadupd");
  CHECK_NON_NULL (builtin);

  gcc_jit_rvalue *arg = gcc_jit_context_null (ctxt, pdouble_type);
  CHECK_NON_NULL (arg);

  gcc_jit_block_add_eval (foo_block, NULL,
    gcc_jit_context_new_call (ctxt, NULL, builtin, 1, &arg));

  gcc_jit_block_end_with_void_return (foo_block, NULL);
}

void
verify_code (gcc_jit_context *ctxt, gcc_jit_result *result)
{
  CHECK_NON_NULL (result);
}

int
main (int argc, char **argv)
{
  /*  This is the same as the main provided by harness.h, but it first create a dummy context and compile
      in order to add the target builtins to libgccjit's internal state.  */
  gcc_jit_context *ctxt;
  ctxt = gcc_jit_context_acquire ();
  if (!ctxt)
    {
      fail ("gcc_jit_context_acquire failed");
      return -1;
    }
  gcc_jit_result *result;
  result = gcc_jit_context_compile (ctxt);
  gcc_jit_result_release (result);
  gcc_jit_context_release (ctxt);

  int i;

  for (i = 1; i <= 5; i++)
    {
      snprintf (test, sizeof (test),
		"%s iteration %d of %d",
                extract_progname (argv[0]),
                i, 5);

      //printf ("ITERATION %d\n", i);
      test_jit (argv[0], NULL);
      //printf ("\n");
    }

  totals ();

  return 0;
}
