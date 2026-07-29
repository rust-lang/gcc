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
#define OUTPUT_FILENAME  "output-of-test-type-aligned-attribute.c.s"
#include "harness.h"

gcc_jit_struct *
create_struct (gcc_jit_context *ctxt,
	       gcc_jit_type *uint8_type,
	       const char *name)
{
  gcc_jit_field *a =
    gcc_jit_context_new_field (ctxt,
			       NULL,
			       uint8_type,
			       "a");
  gcc_jit_field *b =
    gcc_jit_context_new_field (ctxt,
			       NULL,
			       uint8_type,
			       "b");
  gcc_jit_field *fields[] = {a, b};
  return gcc_jit_context_new_struct_type (ctxt, NULL, name, 2, fields);
}

void
create_function (gcc_jit_context *ctxt,
		 gcc_jit_type *uint8_type,
		 gcc_jit_type *int_type,
		 gcc_jit_struct *struct_type,
		 const char *name)
{
  gcc_jit_type *ptr_to_struct = gcc_jit_type_get_pointer (
    gcc_jit_struct_as_type (struct_type));
  gcc_jit_param *t = gcc_jit_context_new_param (ctxt,
						NULL,
						ptr_to_struct,
						"t");
  gcc_jit_param *i = gcc_jit_context_new_param (ctxt,
						NULL,
						int_type,
						"i");
  gcc_jit_param *params[] = {t, i};
  gcc_jit_function *func = gcc_jit_context_new_function (
    ctxt,
    NULL,
    GCC_JIT_FUNCTION_EXPORTED,
    uint8_type,
    name,
    2,
    params,
    0);

  gcc_jit_lvalue *array_access = gcc_jit_context_new_array_access (
    ctxt, NULL, gcc_jit_param_as_rvalue (t), gcc_jit_param_as_rvalue (i));
  gcc_jit_field *field = gcc_jit_struct_get_field (struct_type, 1);

  gcc_jit_rvalue *field_access = gcc_jit_rvalue_access_field (
    gcc_jit_lvalue_as_rvalue (array_access),
    NULL,
    field);

  gcc_jit_block *func_block = gcc_jit_function_new_block (func, NULL);
  gcc_jit_block_end_with_return (func_block, NULL, field_access);
}

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:
struct Normal {
    uint8_t a;
    uint8_t b;
};

struct __attribute__((aligned(8))) Aligned {
    uint8_t a;
    uint8_t b;
};

uint8_t get_b_normal(struct Normal *t, int i) {
    return t[i].b;
}

uint8_t get_b_aligned(struct Aligned *t, int i) {
    return t[i].b;
}
  */
  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_type *uint8_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_UINT8_T);

  /* Creating the `Normal` struct. */
  gcc_jit_struct *normal_struct = create_struct (ctxt,
						 uint8_type,
						 "Normal");
  /* Creating the `Aligned` struct. */
  gcc_jit_struct *aligned_struct = create_struct (ctxt,
						  uint8_type,
						  "Aligned");
  /* __attribute__ ((aligned(8))) */
  gcc_jit_type_add_integer_attribute (
    gcc_jit_struct_as_type (aligned_struct),
    GCC_JIT_TYPE_ATTRIBUTE_ALIGNED,
    8);

  create_function (ctxt, uint8_type, int_type, normal_struct, "get_b_normal");
  create_function (ctxt, uint8_type, int_type, aligned_struct, "get_b_aligned");
}

/* { dg-final { jit-verify-output-file-was-created "" } } */

/* When aligned(8) is not used, we should have "2" instead of "8" as last argument. */
/* { dg-final { jit-verify-assembler-output "movzbl\\s+1\\\(%rdi,%rsi,2\\\),\\s+%eax" } } */
/* { dg-final { jit-verify-assembler-output "movzbl\\s+1\\\(%rdi,%rsi,8\\\),\\s+%eax" } } */
