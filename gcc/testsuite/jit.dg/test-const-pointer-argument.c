#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "libgccjit.h"

#include "harness.h"

void
create_code (gcc_jit_context *ctxt, void *user_data)
{
  /* Let's try to inject the equivalent of:

     void test_ptr(const void* value)
     {
      return;
     }

     void main (void)
     {
       const void *ptr;
       test_ptr (ptr);
       return;
     }
  */
  gcc_jit_type *void_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_VOID);
  gcc_jit_type *real_void_ptr_type =
    gcc_jit_context_get_type (ctxt, GCC_JIT_TYPE_VOID_PTR);
  gcc_jit_type *const_real_void_ptr_type =
    gcc_jit_type_get_const (real_void_ptr_type);

  /* Build the test_ptr.  */
  gcc_jit_param *param =
    gcc_jit_context_new_param (ctxt, NULL, const_real_void_ptr_type, "value");
  gcc_jit_function *test_ptr =
    gcc_jit_context_new_function (ctxt, NULL,
				  GCC_JIT_FUNCTION_EXPORTED,
				  void_type,
				  "test_ptr",
				  1, &param,
				  0);
  gcc_jit_block *block = gcc_jit_function_new_block (test_ptr, NULL);
  gcc_jit_block_end_with_void_return (block, NULL);

  /* Build main.  */
  gcc_jit_function *main =
    gcc_jit_context_new_function (ctxt, NULL,
				  GCC_JIT_FUNCTION_EXPORTED,
				  void_type,
				  "main",
				  0, NULL,
				  0);
  gcc_jit_block *main_block = gcc_jit_function_new_block (main, NULL);

  gcc_jit_type *void_ptr_type =
    gcc_jit_type_get_pointer (void_type);
  gcc_jit_type *const_void_ptr_type =
    gcc_jit_type_get_const (void_ptr_type);

  gcc_jit_lvalue *pointer =
    gcc_jit_function_new_local (main, NULL, const_void_ptr_type, "ptr");
  gcc_jit_rvalue *ptr_rvalue = gcc_jit_lvalue_as_rvalue (pointer);

  gcc_jit_block_add_eval (main_block, NULL,
    gcc_jit_context_new_call (ctxt, NULL, test_ptr, 1, &ptr_rvalue));
  gcc_jit_block_end_with_void_return (main_block, NULL);
}

void
verify_code (gcc_jit_context *ctxt, gcc_jit_result *result)
{
  CHECK_NON_NULL (result);
}
