/* { dg-do compile { target x86_64-*-* } } */

#include <stdlib.h>
#include <stdio.h>

#include "libgccjit.h"

#define TEST_COMPILING_TO_FILE
#define OUTPUT_KIND      GCC_JIT_OUTPUT_KIND_ASSEMBLER
#define OUTPUT_FILENAME  "output-of-test-array-from-bytes-asm.c.s"
#include "harness.h"

/* Bytes with an interior NUL, a non-ASCII byte and no NUL terminator.  */
static const unsigned char array_from_bytes_bytes[5] = { 'a', 0, 'b', 0xff, 'c' };

/* Non-ASCII bytes with no NUL at all.  */
static const unsigned char array_from_bytes_u8_bytes[5] =
  { 0xde, 0xad, 0xbe, 0xef, 0x01 };

/* A NUL-terminated string.  */
static const char array_from_bytes_struct_bytes[5] = "hey!";

static void
make_global (gcc_jit_context *ctxt, const char *name,
	     enum gcc_jit_types elem_type, const void *bytes,
	     size_t num_bytes)
{
  gcc_jit_type *array_type =
    gcc_jit_context_new_array_type (ctxt, NULL,
				    gcc_jit_context_get_type (ctxt, elem_type),
				    num_bytes);
  gcc_jit_lvalue *global =
    gcc_jit_context_new_global (ctxt, NULL, GCC_JIT_GLOBAL_EXPORTED,
				array_type, name);
  gcc_jit_global_set_initializer_rvalue (
    global,
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, array_type, bytes,
					  num_bytes));
}

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:

unsigned char not_nul_terminated[5] = "tadam";
unsigned char nul_terminated[4] = "foo";
unsigned char array_from_bytes_array[5] = "a\0b\377c";
uint8_t array_from_bytes_u8_array[5] = "\336\255\276\357\001";
struct S { int i; char arr[5]; } array_from_bytes_struct = { 42, "hey!" };

     and check that they're emitted as strings rather than byte by
     byte.  */
  make_global (ctxt, "not_nul_terminated", GCC_JIT_TYPE_UNSIGNED_CHAR,
	       "tadam", 5);
  make_global (ctxt, "nul_terminated", GCC_JIT_TYPE_UNSIGNED_CHAR,
	       "foo", 4);
  make_global (ctxt, "array_from_bytes_array", GCC_JIT_TYPE_UNSIGNED_CHAR,
	       array_from_bytes_bytes, sizeof (array_from_bytes_bytes));
  make_global (ctxt, "array_from_bytes_u8_array", GCC_JIT_TYPE_UINT8_T,
	       array_from_bytes_u8_bytes, sizeof (array_from_bytes_u8_bytes));

  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_type *char_array_type =
    gcc_jit_context_new_array_type (
      ctxt, NULL, gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_CHAR), 5);
  gcc_jit_field *fields[2] = {
    gcc_jit_context_new_field (ctxt, NULL, int_type, "i"),
    gcc_jit_context_new_field (ctxt, NULL, char_array_type, "arr"),
  };
  gcc_jit_type *struct_type = gcc_jit_struct_as_type (
    gcc_jit_context_new_struct_type (ctxt, NULL, "S", 2, fields));
  gcc_jit_rvalue *values[2] = {
    gcc_jit_context_new_rvalue_from_int (ctxt, int_type, 42),
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, char_array_type,
					  array_from_bytes_struct_bytes,
					  sizeof (array_from_bytes_struct_bytes)),
  };
  gcc_jit_lvalue *test_struct =
    gcc_jit_context_new_global (ctxt, NULL, GCC_JIT_GLOBAL_EXPORTED,
				struct_type, "array_from_bytes_struct");
  gcc_jit_global_set_initializer_rvalue (
    test_struct,
    gcc_jit_context_new_struct_constructor (ctxt, NULL, struct_type, 2,
					    fields, values));
}

/* { dg-final { jit-verify-output-file-was-created "" } } */
/* { dg-final { jit-verify-assembler-output "not_nul_terminated:\\s+\\.ascii\\s+\"tadam\"" } } */
/* { dg-final { jit-verify-assembler-output "nul_terminated:\\s+\\.string\\s+\"foo\"" } } */

/* { dg-final { jit-verify-assembler-output "array_from_bytes_array:\\s+\\.string\\s+\"a\"\\s+\\.(base64\\s+\"Yv9j\"|ascii\\s+\"b.377c\")\\s+\\.(globl|text)" } } */

/* Binary data with no NUL is emitted as a single directive: ".base64" when
   the assembler supports it, ".ascii" otherwise.  */
/* { dg-final { jit-verify-assembler-output "array_from_bytes_u8_array:\\s+\\.(base64\\s+\"3q2\\+7wE=\"|ascii\\s+\".336.255.276.357.001\")\\s+\\.(globl|text)" } } */

/* { dg-final { jit-verify-assembler-output "array_from_bytes_struct:\\s+\\.long\\s+42\\s+\\.string\\s+\"hey!\"\\s+\\.zero\\s+3\\s" } } */
