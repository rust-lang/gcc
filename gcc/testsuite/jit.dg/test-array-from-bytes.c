#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "libgccjit.h"

#include "harness.h"

/* Bytes with an interior NUL, a non-ASCII byte and no NUL terminator.  */
const unsigned char array_from_bytes_bytes[5] = { 'a', 0, 'b', 0xff, 'c' };

/* Different bytes for the const-qualified array: leading NUL, non-ASCII.  */
const unsigned char array_from_bytes_const_bytes[4] = { 0, 'x', 0x80, 'y' };

static void
create_getter (gcc_jit_context *ctxt, const char *name,
	       gcc_jit_type *local_type, gcc_jit_rvalue *init,
	       gcc_jit_field *field)
{
/* Create a function:

int NAME (int idx) {
  ARRAY_TYPE local = INIT;
  return (int)local[idx];
}

// or, if FIELD is non-NULL:

int NAME (int idx) {
  ARRAY_TYPE local = INIT;
  return (int)local.FIELD[idx];
}

*/
  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_param *idx =
    gcc_jit_context_new_param (ctxt, NULL, int_type, "idx");
  gcc_jit_function *func =
    gcc_jit_context_new_function (ctxt, NULL, GCC_JIT_FUNCTION_EXPORTED,
				  int_type, name, 1, &idx, 0);
  gcc_jit_block *block = gcc_jit_function_new_block (func, NULL);

  gcc_jit_lvalue *local =
    gcc_jit_function_new_local (func, NULL, local_type, "local");
  gcc_jit_block_add_assignment (block, NULL, local, init);

  gcc_jit_rvalue *array = gcc_jit_lvalue_as_rvalue (local);
  if (field)
    array = gcc_jit_lvalue_as_rvalue (
      gcc_jit_lvalue_access_field (local, NULL, field));

  gcc_jit_lvalue *element =
    gcc_jit_context_new_array_access (ctxt, NULL, array,
				      gcc_jit_param_as_rvalue (idx));
  gcc_jit_block_end_with_return (
    block, NULL,
    gcc_jit_context_new_cast (ctxt, NULL,
			      gcc_jit_lvalue_as_rvalue (element),
			      int_type));
}

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:

unsigned char array_from_bytes_array[5] = "a\0b\377c";

uint8_t array_from_bytes_u8_array[5] = "a\0b\377c";

const unsigned char array_from_bytes_const_array[4] = "\0x\200y";

struct S {
  int i;
  char arr[5];
}

struct S array_from_bytes_struct = { 42, "a\0b\377c" };

int array_from_bytes_get_uchar (int idx)
{
  unsigned char local[5] = "a\0b\377c";
  return (int)local[idx];
}

int array_from_bytes_get_u8 (int idx)
{
  uint8_t local[5] = "a\0b\377c";
  return (int)local[idx];
}

int array_from_bytes_get_struct (int idx)
{
  struct S local = { 42, "a\0b\377c" };
  return (int)local.arr[idx];
}
  */
  gcc_jit_type *uchar_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_UNSIGNED_CHAR);
  gcc_jit_type *u8_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_UINT8_T);
  gcc_jit_type *char_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_CHAR);
  gcc_jit_type *int_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_INT);

  gcc_jit_type *uchar_array_type =
    gcc_jit_context_new_array_type (ctxt, NULL, uchar_type, 5);
  gcc_jit_rvalue *array_rvalue =
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, uchar_array_type,
					  array_from_bytes_bytes,
					  sizeof (array_from_bytes_bytes));
  CHECK_STRING_VALUE (gcc_jit_object_get_debug_string (
			gcc_jit_rvalue_as_object (array_rvalue)),
		      "\"a\\000b\\377c\"");

  gcc_jit_lvalue *test_array =
    gcc_jit_context_new_global (ctxt, NULL, GCC_JIT_GLOBAL_EXPORTED,
				uchar_array_type,
				"array_from_bytes_array");
  gcc_jit_global_set_initializer_rvalue (test_array, array_rvalue);

  /* Same thing with an array of uint8_t.  */
  gcc_jit_type *u8_array_type =
    gcc_jit_context_new_array_type (ctxt, NULL, u8_type, 5);
  gcc_jit_lvalue *test_u8_array =
    gcc_jit_context_new_global (ctxt, NULL, GCC_JIT_GLOBAL_EXPORTED,
				u8_array_type,
				"array_from_bytes_u8_array");
  gcc_jit_global_set_initializer_rvalue (
    test_u8_array,
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, u8_array_type,
					  array_from_bytes_bytes,
					  sizeof (array_from_bytes_bytes)));

  gcc_jit_type *const_uchar_array_type =
    gcc_jit_type_get_const (
      gcc_jit_context_new_array_type (ctxt, NULL, uchar_type, 4));
  gcc_jit_lvalue *test_const_array =
    gcc_jit_context_new_global (ctxt, NULL, GCC_JIT_GLOBAL_EXPORTED,
				const_uchar_array_type,
				"array_from_bytes_const_array");
  gcc_jit_global_set_initializer_rvalue (
    test_const_array,
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, const_uchar_array_type,
					  array_from_bytes_const_bytes,
					  sizeof (array_from_bytes_const_bytes)));

  /* Use it as a field of a struct constructor too.  */
  gcc_jit_type *char_array_type =
    gcc_jit_context_new_array_type (ctxt, NULL, char_type, 5);
  gcc_jit_field *fields[2] = {
    gcc_jit_context_new_field (ctxt, NULL, int_type, "i"),
    gcc_jit_context_new_field (ctxt, NULL, char_array_type, "arr"),
  };
  gcc_jit_struct *struct_type =
    gcc_jit_context_new_struct_type (ctxt, NULL, "S", 2, fields);
  gcc_jit_rvalue *values[2] = {
    gcc_jit_context_new_rvalue_from_int (ctxt, int_type, 42),
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, char_array_type,
					  array_from_bytes_bytes,
					  sizeof (array_from_bytes_bytes)),
  };
  gcc_jit_rvalue *struct_rvalue =
    gcc_jit_context_new_struct_constructor (
      ctxt, NULL, gcc_jit_struct_as_type (struct_type), 2, fields, values);

  gcc_jit_lvalue *test_struct =
    gcc_jit_context_new_global (ctxt, NULL, GCC_JIT_GLOBAL_EXPORTED,
				gcc_jit_struct_as_type (struct_type),
				"array_from_bytes_struct");
  gcc_jit_global_set_initializer_rvalue (test_struct, struct_rvalue);

  /* Now use all of them to initialize locals within functions.  */
  create_getter (
    ctxt, "array_from_bytes_get_uchar", uchar_array_type,
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, uchar_array_type,
					  array_from_bytes_bytes,
					  sizeof (array_from_bytes_bytes)),
    NULL);
  create_getter (
    ctxt, "array_from_bytes_get_u8", u8_array_type,
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, u8_array_type,
					  array_from_bytes_bytes,
					  sizeof (array_from_bytes_bytes)),
    NULL);

  gcc_jit_rvalue *local_values[2] = {
    gcc_jit_context_new_rvalue_from_int (ctxt, int_type, 42),
    gcc_jit_context_new_array_from_bytes (ctxt, NULL, char_array_type,
					  array_from_bytes_bytes,
					  sizeof (array_from_bytes_bytes)),
  };
  create_getter (
    ctxt, "array_from_bytes_get_struct",
    gcc_jit_struct_as_type (struct_type),
    gcc_jit_context_new_struct_constructor (
      ctxt, NULL, gcc_jit_struct_as_type (struct_type), 2, fields,
      local_values),
    fields[1]);
}


struct array_from_bytes_s
{
  int i;
  char arr[5];
};

typedef int (*array_from_bytes_getter_fn) (int);

void
verify_code (gcc_jit_context *ctxt, gcc_jit_result *result)
{
  CHECK_NON_NULL (result);

  const unsigned char *test_array =
    (const unsigned char *)gcc_jit_result_get_global (result,
						      "array_from_bytes_array");
  CHECK_NON_NULL (test_array);
  CHECK_VALUE (memcmp (test_array, array_from_bytes_bytes,
		       sizeof (array_from_bytes_bytes)), 0);

  const uint8_t *test_u8_array =
    (const uint8_t *)gcc_jit_result_get_global (result,
						"array_from_bytes_u8_array");
  CHECK_NON_NULL (test_u8_array);
  CHECK_VALUE (memcmp (test_u8_array, array_from_bytes_bytes,
		       sizeof (array_from_bytes_bytes)), 0);

  const unsigned char *test_const_array =
    (const unsigned char *)gcc_jit_result_get_global (
	result, "array_from_bytes_const_array");
  CHECK_NON_NULL (test_const_array);
  CHECK_VALUE (memcmp (test_const_array, array_from_bytes_const_bytes,
		       sizeof (array_from_bytes_const_bytes)), 0);

  const struct array_from_bytes_s *test_struct =
    (const struct array_from_bytes_s *)gcc_jit_result_get_global (
	result, "array_from_bytes_struct");
  CHECK_NON_NULL (test_struct);
  CHECK_VALUE (test_struct->i, 42);
  CHECK_VALUE (memcmp (test_struct->arr, array_from_bytes_bytes,
		       sizeof (array_from_bytes_bytes)), 0);

  array_from_bytes_getter_fn get_uchar =
    (array_from_bytes_getter_fn)gcc_jit_result_get_code (
	result, "array_from_bytes_get_uchar");
  CHECK_NON_NULL (get_uchar);
  array_from_bytes_getter_fn get_u8 =
    (array_from_bytes_getter_fn)gcc_jit_result_get_code (
	result, "array_from_bytes_get_u8");
  CHECK_NON_NULL (get_u8);
  array_from_bytes_getter_fn get_struct =
    (array_from_bytes_getter_fn)gcc_jit_result_get_code (
	result, "array_from_bytes_get_struct");
  CHECK_NON_NULL (get_struct);

  for (int i = 0; i < (int)sizeof (array_from_bytes_bytes); i++)
    {
      CHECK_VALUE (get_uchar (i), (int)array_from_bytes_bytes[i]);
      CHECK_VALUE (get_u8 (i), (int)(uint8_t)array_from_bytes_bytes[i]);
      CHECK_VALUE (get_struct (i), (int)(char)array_from_bytes_bytes[i]);
    }
}
