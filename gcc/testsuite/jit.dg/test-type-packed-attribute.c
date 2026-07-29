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
#define OUTPUT_FILENAME  "output-of-test-type-packed-attribute.c.s"
#include "harness.h"

gcc_jit_struct *
create_struct (gcc_jit_context *ctxt,
	       gcc_jit_type *uint32_type,
	       gcc_jit_type *char_type,
	       const char *name)
{
  gcc_jit_field *a =
    gcc_jit_context_new_field (ctxt,
                               NULL,
                               char_type,
                               "a");
  gcc_jit_field *b =
    gcc_jit_context_new_field (ctxt,
                               NULL,
                               uint32_type,
                               "b");
  gcc_jit_field *fields[] = {a, b};
  return gcc_jit_context_new_struct_type (ctxt, NULL, name, 2, fields);
}

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:
struct Unpacked {
  char a;
  uint32_t b;
};

struct __attribute__((packed)) Packed {
  char a;
  uint32_t b;
};

int size_unpacked() {
  return sizeof(struct Unpacked);
}

int size_packed() {
  return sizeof(struct Packed);
}
  */
  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_type *uint32_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_UINT32_T);
  gcc_jit_type *char_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_CHAR);

  /* Creating the `Unpacked` struct. */
  gcc_jit_struct *unpacked_struct = create_struct (ctxt,
						   uint32_type,
						   char_type,
						   "Unpacked");
  /* Creating the `Packed` struct. */
  gcc_jit_struct *packed_struct = create_struct (ctxt,
						 uint32_type,
						 char_type,
						 "Packed");
  /* __attribute__ ((packed)) */
  gcc_jit_type_add_attribute (gcc_jit_struct_as_type (packed_struct),
  			      GCC_JIT_TYPE_ATTRIBUTE_PACKED);

  /* Creating the `size_unpacked` function. */
  gcc_jit_function *size_unpacked =
    gcc_jit_context_new_function (ctxt, NULL,
				  GCC_JIT_FUNCTION_INTERNAL,
				  int_type,
				  "size_unpacked",
				  0, NULL,
				  0);

  gcc_jit_block *size_unpacked_block =
    gcc_jit_function_new_block (size_unpacked, NULL);

  gcc_jit_block_end_with_return (size_unpacked_block, NULL,
    gcc_jit_context_new_sizeof (ctxt,
				gcc_jit_struct_as_type (unpacked_struct)));

  /* Creating the `size_packed` function. */
  gcc_jit_function *size_packed =
    gcc_jit_context_new_function (ctxt, NULL,
	  GCC_JIT_FUNCTION_INTERNAL,
	  int_type,
	  "size_packed",
	  0, NULL,
	  0);

  gcc_jit_block *size_packed_block =
    gcc_jit_function_new_block (size_packed, NULL);

  gcc_jit_block_end_with_return (size_packed_block, NULL,
    gcc_jit_context_new_sizeof (ctxt, gcc_jit_struct_as_type (packed_struct)));
}

/* { dg-final { jit-verify-output-file-was-created "" } } */

/* We check that size of both structs are different through the `sizeof` calls. */
/* { dg-final { jit-verify-assembler-output "movl\\s+\\\$5, %eax" } } */
/* { dg-final { jit-verify-assembler-output "movl\\s+\\\$8, %eax" } } */
