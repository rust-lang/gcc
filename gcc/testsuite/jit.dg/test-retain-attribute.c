/* { dg-do compile { target x86_64-*-* } } */

#include <stdlib.h>
#include <stdio.h>

#include "libgccjit.h"

#define TEST_ESCHEWS_SET_OPTIONS
static void set_options (gcc_jit_context *ctxt, const char *argv0)
{
  // Set "-O2".
  gcc_jit_context_set_int_option(ctxt, GCC_JIT_INT_OPTION_OPTIMIZATION_LEVEL, 2);
}

#define TEST_COMPILING_TO_FILE
#define OUTPUT_KIND      GCC_JIT_OUTPUT_KIND_ASSEMBLER
#define OUTPUT_FILENAME  "output-of-test-retain-attribute.c.s"
#include "harness.h"

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:
__attribute__((retain))
int not_removed() { return 1; }
const int blabla __attribute__((retain)) = 12;
  */
  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);

  /* Creating the `not_removed` function. */
    gcc_jit_function *not_removed_func =
    gcc_jit_context_new_function (ctxt, NULL,
	  GCC_JIT_FUNCTION_INTERNAL,
	  int_type,
	  "not_removed",
	  0, NULL,
	  0);

  gcc_jit_block *foo_block = gcc_jit_function_new_block (not_removed_func,
							 NULL);
  gcc_jit_block_end_with_return (foo_block, NULL,
    gcc_jit_context_new_rvalue_from_int (ctxt, int_type, 1));
  /* __attribute__ ((retain)) */
  gcc_jit_function_add_attribute(not_removed_func, GCC_JIT_FN_ATTRIBUTE_RETAIN);

  /* Creating the `blabla` variable. */
  gcc_jit_type *const_int = gcc_jit_type_get_const (int_type);
  gcc_jit_lvalue *glob = gcc_jit_context_new_global (ctxt,
						     NULL,
						     GCC_JIT_GLOBAL_INTERNAL,
						     const_int,
						     "blabla");
  gcc_jit_lvalue_add_attribute(glob, GCC_JIT_VARIABLE_ATTRIBUTE_RETAIN);
}

/* { dg-final { jit-verify-output-file-was-created "" } } */

/* We check that the retained attribute worked as expected and added the `R` in the section info. */
/* { dg-final { jit-verify-assembler-output ".section\\s+.text.not_removed,\"axR\" } } */
/* { dg-final { jit-verify-assembler-output ".section\\s+.rodata.blabla,\"aR\" } } */
