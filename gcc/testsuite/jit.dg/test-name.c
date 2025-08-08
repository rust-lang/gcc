/* { dg-do compile { target x86_64-*-* } } */

#include <stdlib.h>
#include <stdio.h>

#include "libgccjit.h"

// We want the debug info to check the function variable's name.
#define TEST_ESCHEWS_SET_OPTIONS
static void set_options (gcc_jit_context *ctxt, const char *argv0)
{
    gcc_jit_context_set_bool_option(ctxt, GCC_JIT_BOOL_OPTION_DEBUGINFO, 1);
}

#define TEST_COMPILING_TO_FILE
#define OUTPUT_KIND      GCC_JIT_OUTPUT_KIND_ASSEMBLER
#define OUTPUT_FILENAME  "output-of-test-name.c.s"
#include "harness.h"

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:

int original_foo = 10;
  */
  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_lvalue *foo =
    gcc_jit_context_new_global (ctxt, NULL, GCC_JIT_GLOBAL_EXPORTED,
                                int_type, "original_foo");
  gcc_jit_rvalue *ten = gcc_jit_context_new_rvalue_from_int (ctxt, int_type, 10);
  gcc_jit_global_set_initializer_rvalue (foo, ten);

  CHECK_STRING_VALUE (gcc_jit_lvalue_get_name (foo), "original_foo");
  gcc_jit_lvalue_set_name (foo, "new_one");
  CHECK_STRING_VALUE (gcc_jit_lvalue_get_name (foo), "new_one");

  /* Let's try to inject te equivalent of:
int blabla() {
  int the_var = 12;

  return the_var;
}
  */
  gcc_jit_function *blabla_func =
    gcc_jit_context_new_function (ctxt, NULL,
          GCC_JIT_FUNCTION_EXPORTED,
          int_type,
          "blabla",
          0, NULL,
          0);

  gcc_jit_block *blabla_block = gcc_jit_function_new_block (blabla_func, NULL);

  /* Build locals:  */
  gcc_jit_lvalue *the_var =
    gcc_jit_function_new_local (blabla_func, NULL, int_type, "the_var");

  /* int the_var = 0; */
  gcc_jit_block_add_assignment (
    blabla_block, NULL,
    the_var,
    gcc_jit_context_new_rvalue_from_int (ctxt, int_type, 0));

  gcc_jit_block_end_with_return (
    blabla_block, NULL,
    gcc_jit_lvalue_as_rvalue (the_var));

  CHECK_STRING_VALUE (gcc_jit_lvalue_get_name (the_var), "the_var");
  gcc_jit_lvalue_set_name (the_var, "confiture");
  CHECK_STRING_VALUE (gcc_jit_lvalue_get_name (the_var), "confiture");
}

/* { dg-final { jit-verify-output-file-was-created "" } } */
/* Check that the global was correctly renamed */
/* { dg-final { jit-verify-assembler-output-not "original_foo:" } } */
/* { dg-final { jit-verify-assembler-output "new_one:" } } */

/* { dg-final { jit-verify-assembler-output-not ".string\\s+\"the_var\"" } } */
/* { dg-final { jit-verify-assembler-output ".string\\s+\"confiture\"" } } */
