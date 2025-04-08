/* jit.c -- Dummy "frontend" for use during JIT-compilation.
   Copyright (C) 2013-2025 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "c-family/c-common.h"
#include "target.h"
#include "jit-playback.h"
#include "stor-layout.h"
#include "debug.h"
#include "langhooks.h"
#include "langhooks-def.h"
#include "diagnostic.h"
#include "options.h"
#include "stringpool.h"
#include "attribs.h"
#include "cgraph.h"
#include "target.h"
#include "diagnostic-format-text.h"
#include "make-unique.h"
#include "print-tree.h"

#include <mpfr.h>
#include <unordered_map>

using namespace gcc::jit;

/* Attribute handling.  */

/* These variables act as a cache for the target builtins. This is needed in
   order to be able to type-check the calls since we can only get those types
   in the playback phase while we need them in the recording phase.  */
hash_map<nofree_string_hash, tree> target_builtins{};
std::unordered_map<std::string, recording::function_type*> target_function_types
{};
recording::context target_builtins_ctxt{NULL};

static const scoped_attribute_specs *const jit_attribute_table[] =
{
  &c_common_gnu_attribute_table,
  &c_common_format_attribute_table
};

char* jit_personality_func_name = NULL;
static tree personality_decl;

/* FIXME: This is a hack to preserve trees that we create from the
   garbage collector.  */

static GTY (()) tree jit_gc_root;

void
jit_preserve_from_gc (tree t)
{
  jit_gc_root = tree_cons (NULL_TREE, t, jit_gc_root);
}

/* Language-dependent contents of a type.  */

struct GTY(()) lang_type
{
  char dummy;
};

/* Language-dependent contents of a decl.  */

struct GTY((variable_size)) lang_decl
{
  char dummy;
};

/* Language-dependent contents of an identifier.  This must include a
   tree_identifier.  */

struct GTY(()) lang_identifier
{
  struct tree_identifier common;
};

/* The resulting tree type.  */

/* See lang_tree_node in gcc/c/c-decl.cc. */
union GTY((desc ("TREE_CODE (&%h.generic) == IDENTIFIER_NODE"),
	chain_next ("(union lang_tree_node *) jit_tree_chain_next (&%h.generic)"))) lang_tree_node
{
  union tree_node GTY((tag ("0"),
		       desc ("tree_node_structure (&%h)")))
	generic;
  struct lang_identifier GTY((tag ("1"))) identifier;
};

/* We don't use language_function.  */

struct GTY(()) language_function
{
  int dummy;
};

/* GC-marking callback for use from jit_root_tab.

   If there's an active playback context, call its marking method
   so that it can mark any pointers it references.  */

static void my_ggc_walker (void *)
{
  if (gcc::jit::active_playback_ctxt)
    gcc::jit::active_playback_ctxt->gt_ggc_mx ();
}

const char *dummy;

struct ggc_root_tab jit_root_tab[] =
  {
    {
      &dummy, 1, 0, my_ggc_walker, NULL
    },
    LAST_GGC_ROOT_TAB
  };

/* Subclass of diagnostic_output_format for libgccjit: like text
   output, but capture the message and call add_diagnostic with it
   on the active playback context.  */

class jit_diagnostic_listener : public diagnostic_text_output_format
{
public:
  jit_diagnostic_listener (diagnostic_context &dc,
			   gcc::jit::playback::context &playback_ctxt)
  : diagnostic_text_output_format (dc),
    m_playback_ctxt (playback_ctxt)
  {
  }

  void dump (FILE *out, int indent) const final override
  {
    fprintf (out, "%*sjit_diagnostic_listener\n", indent, "");
    fprintf (out, "%*sm_playback_context: %p\n",
	     indent + 2, "",
	     (void *)&m_playback_ctxt);
  }

  void on_report_diagnostic (const diagnostic_info &info,
			     diagnostic_t orig_diag_kind) final override
  {
    JIT_LOG_SCOPE (gcc::jit::active_playback_ctxt->get_logger ());

    /* Let the text output format do most of the work.  */
    diagnostic_text_output_format::on_report_diagnostic (info, orig_diag_kind);

    const char *text = pp_formatted_text (get_printer ());

    /* Delegate to the playback context (and thence to the
       recording context).  */
    gcc::jit::active_playback_ctxt->add_diagnostic (text, info);

    pp_clear_output_area (get_printer ());
  }

private:
  gcc::jit::playback::context &m_playback_ctxt;
};

/* JIT-specific implementation of diagnostic callbacks.  */

/* Implementation of "begin_diagnostic".  */

static void
jit_begin_diagnostic (diagnostic_text_output_format &,
		      const diagnostic_info */*diagnostic*/)
{
  gcc_assert (gcc::jit::active_playback_ctxt);
  JIT_LOG_SCOPE (gcc::jit::active_playback_ctxt->get_logger ());

  /* No-op (apart from logging); the real error-handling is done by the
     jit_diagnostic_listener.  */
}

/* Implementation of "end_diagnostic".  */

static void
jit_end_diagnostic (diagnostic_text_output_format &,
		    const diagnostic_info *,
		    diagnostic_t)
{
  gcc_assert (gcc::jit::active_playback_ctxt);
  JIT_LOG_SCOPE (gcc::jit::active_playback_ctxt->get_logger ());

  /* No-op (apart from logging); the real error-handling is done by the
     jit_diagnostic_listener.  */
}

/* Language hooks.  */

static bool
jit_langhook_init (void)
{
  jit_gc_root = NULL_TREE;
  personality_decl = NULL_TREE;
  gcc_assert (gcc::jit::active_playback_ctxt);
  JIT_LOG_SCOPE (gcc::jit::active_playback_ctxt->get_logger ());

  static bool registered_root_tab = false;
  if (!registered_root_tab)
    {
      ggc_register_root_tab (jit_root_tab);
      registered_root_tab = true;
    }

  gcc_assert (global_dc);
  diagnostic_text_starter (global_dc) = jit_begin_diagnostic;
  diagnostic_text_finalizer (global_dc) = jit_end_diagnostic;
  auto sink
    = ::make_unique<jit_diagnostic_listener> (*global_dc,
					      *gcc::jit::active_playback_ctxt);
  global_dc->set_output_format (std::move (sink));

  build_common_tree_nodes (flag_signed_char);

  /* I don't know why this has to be done explicitly.  */
  void_list_node = build_tree_list (NULL_TREE, void_type_node);

  target_builtins.empty ();
  build_common_builtin_nodes ();

  /* Initialize EH, if we've been told to do so.  */
  if (flag_exceptions)
    using_eh_for_cleanups ();

  /* The default precision for floating point numbers.  This is used
     for floating point constants with abstract type.  This may
     eventually be controllable by a command line option.  */
  mpfr_set_default_prec (256);

  // FIXME: This code doesn't work as it erases the `target_builtins` map
  // without checking if it's already filled before. A better check would be
  // `if target_builtins.len() == 0` (or whatever this `hash_map` type method
  // name is).
   //static bool builtins_initialized = false;
   //if (!builtins_initialized)
   //{
  targetm.init_builtins ();
     //builtins_initialized = true;
   //}

  return true;
}

static void
jit_langhook_parse_file (void)
{
  /* Replay the activity by the client, recorded on the context.  */
  gcc_assert (gcc::jit::active_playback_ctxt);
  gcc::jit::active_playback_ctxt->replay ();
}

static tree
jit_langhook_type_for_mode (machine_mode mode, int unsignedp)
{
  /* Build any vector types here (see PR 46805).  */
  if (VECTOR_MODE_P (mode))
    {
      tree inner;

      inner = jit_langhook_type_for_mode (GET_MODE_INNER (mode), unsignedp);
      if (inner != NULL_TREE)
	return build_vector_type_for_mode (inner, mode);
      return NULL_TREE;
    }

  if (mode == TYPE_MODE (float_type_node))
    return float_type_node;

  if (mode == TYPE_MODE (double_type_node))
    return double_type_node;

  if (mode == TYPE_MODE (intQI_type_node))
    return unsignedp ? unsigned_intQI_type_node : intQI_type_node;
  if (mode == TYPE_MODE (intHI_type_node))
    return unsignedp ? unsigned_intHI_type_node : intHI_type_node;
  if (mode == TYPE_MODE (intSI_type_node))
    return unsignedp ? unsigned_intSI_type_node : intSI_type_node;
  if (mode == TYPE_MODE (intDI_type_node))
    return unsignedp ? unsigned_intDI_type_node : intDI_type_node;
  if (mode == TYPE_MODE (intTI_type_node))
    return unsignedp ? unsigned_intTI_type_node : intTI_type_node;

  if (mode == TYPE_MODE (integer_type_node))
    return unsignedp ? unsigned_type_node : integer_type_node;

  if (mode == TYPE_MODE (long_integer_type_node))
    return unsignedp ? long_unsigned_type_node : long_integer_type_node;

  if (mode == TYPE_MODE (long_long_integer_type_node))
    return unsignedp ? long_long_unsigned_type_node : long_long_integer_type_node;

  if (COMPLEX_MODE_P (mode))
    {
      if (mode == TYPE_MODE (complex_float_type_node))
	return complex_float_type_node;
      if (mode == TYPE_MODE (complex_double_type_node))
	return complex_double_type_node;
      if (mode == TYPE_MODE (complex_long_double_type_node))
	return complex_long_double_type_node;
      if (mode == TYPE_MODE (complex_integer_type_node) && !unsignedp)
	return complex_integer_type_node;
    }

  for (int i = 0; i < NUM_FLOATN_NX_TYPES; i++)
    if (FLOATN_NX_TYPE_NODE (i) != NULL_TREE
	&& mode == TYPE_MODE (FLOATN_NX_TYPE_NODE (i)))
      return FLOATN_NX_TYPE_NODE (i);
  /* gcc_unreachable */
  return NULL;
}

recording::type* tree_type_to_jit_type (tree type)
{
  if (TREE_CODE (type) == VECTOR_TYPE)
  {
    tree inner_type = TREE_TYPE (type);
    recording::type* element_type = tree_type_to_jit_type (inner_type);
    poly_uint64 size = TYPE_VECTOR_SUBPARTS (type);
    long constant_size = size.to_constant ();
    if (element_type != NULL)
      return element_type->get_vector (constant_size);
    return NULL;
  }
  if (TREE_CODE (type) == REFERENCE_TYPE)
    // For __builtin_ms_va_start.
    // FIXME: wrong type.
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_VOID);
  if (TREE_CODE (type) == RECORD_TYPE)
    // For __builtin_sysv_va_copy.
    // FIXME: wrong type.
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_VOID);
  if (type == void_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_VOID);
  else if (type == ptr_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_VOID_PTR);
  else if (type == const_ptr_type_node)
  {
    // Void const ptr.
    recording::type* result =
      new recording::memento_of_get_type (&target_builtins_ctxt,
					  GCC_JIT_TYPE_VOID_PTR);
    return new recording::memento_of_get_const (result);
  }
  else if (type == unsigned_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_UNSIGNED_INT);
  else if (type == long_unsigned_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_UNSIGNED_LONG);
  else if (type == integer_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_INT);
  else if (type == long_integer_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_LONG);
  else if (type == long_long_integer_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_LONG_LONG);
  else if (type == signed_char_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_SIGNED_CHAR);
  else if (type == char_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_CHAR);
  else if (type == unsigned_intQI_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_UINT8_T);
  else if (type == short_integer_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_SHORT);
  else if (type == short_unsigned_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_UNSIGNED_SHORT);
  else if (type == complex_float_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_COMPLEX_FLOAT);
  else if (type == complex_double_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_COMPLEX_DOUBLE);
  else if (type == complex_long_double_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					    GCC_JIT_TYPE_COMPLEX_LONG_DOUBLE);
  else if (type == float_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_FLOAT);
  else if (type == double_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_DOUBLE);
  else if (type == long_double_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_LONG_DOUBLE);
  else if (type == bfloat16_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_BFLOAT16);
  else if (type == float16_type_node)
  {
    return new recording::memento_of_get_type(&target_builtins_ctxt, GCC_JIT_TYPE_FLOAT16);
  }
  else if (type == float32_type_node)
  {
    return new recording::memento_of_get_type(&target_builtins_ctxt, GCC_JIT_TYPE_FLOAT32);
  }
  else if (type == float64_type_node)
  {
    return new recording::memento_of_get_type(&target_builtins_ctxt, GCC_JIT_TYPE_FLOAT64);
  }
  else if (type == float128_type_node)
  {
    return new recording::memento_of_get_type(&target_builtins_ctxt, GCC_JIT_TYPE_FLOAT128);
  }
  else if (type == dfloat128_type_node)
    // FIXME: wrong type.
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_VOID);
  else if (type == long_long_unsigned_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_UNSIGNED_LONG_LONG);
  else if (type == boolean_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_BOOL);
  else if (type == size_type_node)
    return new recording::memento_of_get_type (&target_builtins_ctxt,
					       GCC_JIT_TYPE_SIZE_T);
  else if (TREE_CODE (type) == POINTER_TYPE)
  {
    tree inner_type = TREE_TYPE (type);
    recording::type* element_type = tree_type_to_jit_type (inner_type);
    if (!element_type)
      return nullptr;
    return element_type->get_pointer ();
  }
  else if (type == unsigned_intTI_type_node)
  {
    // TODO: check if this is the correct type.
    return new recording::memento_of_get_type (&target_builtins_ctxt, GCC_JIT_TYPE_UINT128_T);
  }
  else if (INTEGRAL_TYPE_P (type))
  {
    // TODO: check if this is the correct type.
    unsigned int size = tree_to_uhwi (TYPE_SIZE_UNIT (type));
    return target_builtins_ctxt.get_int_type (size, TYPE_UNSIGNED (type));
  }
  else if (SCALAR_FLOAT_TYPE_P (type))
  {
    // TODO: check if this is the correct type.
    unsigned int size = tree_to_uhwi (TYPE_SIZE_UNIT (type));
    enum gcc_jit_types type;
    switch (size) {
        case 2:
            type = GCC_JIT_TYPE_FLOAT16;
            break;
        case 4:
            type = GCC_JIT_TYPE_FLOAT32;
            break;
        case 8:
            type = GCC_JIT_TYPE_FLOAT64;
            break;
        default:
            fprintf (stderr, "Unexpected float size: %d\n", size);
            abort ();
            break;
    }
    return new recording::memento_of_get_type (&target_builtins_ctxt, type);
  }
  else
  {
    // Attempt to find an unqualified type when the current type has qualifiers.
    tree tp = TYPE_MAIN_VARIANT (type);
    for ( ; tp != NULL ; tp = TYPE_NEXT_VARIANT (tp))
    {
      if (TYPE_QUALS (tp) == 0 && type != tp)
      {
	recording::type* result = tree_type_to_jit_type (tp);
	if (result != NULL)
	{
	  if (TYPE_READONLY (tp))
	    result = new recording::memento_of_get_const (result);
	  if (TYPE_VOLATILE (tp))
	    result = new recording::memento_of_get_volatile (result);
	  return result;
	}
      }
    }
  }

  return NULL;
}

/* Record a builtin function.  We save their types to be able to check types
   in recording and for reflection.  */

static tree
jit_langhook_builtin_function (tree decl)
{
  if (TREE_CODE (decl) == FUNCTION_DECL)
  {
    const char* name = IDENTIFIER_POINTER (DECL_NAME (decl));
    target_builtins.put (name, decl);

    std::string string_name (name);
    if (target_function_types.count (string_name) == 0)
    {
      tree function_type = TREE_TYPE (decl);
      tree arg = TYPE_ARG_TYPES (function_type);
      bool is_variadic = false;

      auto_vec <recording::type *> param_types;

      while (arg != void_list_node)
      {
	if (arg == NULL)
	{
	  is_variadic = true;
	  break;
	}
	if (arg != void_list_node)
	{
	  recording::type* arg_type = tree_type_to_jit_type (TREE_VALUE (arg));
	  if (arg_type == NULL)
	    return decl;
	  param_types.safe_push (arg_type);
	}
	arg = TREE_CHAIN (arg);
      }

      tree result_type = TREE_TYPE (function_type);
      recording::type* return_type = tree_type_to_jit_type (result_type);

      if (return_type == NULL)
	return decl;

      recording::function_type* func_type =
	new recording::function_type (&target_builtins_ctxt, return_type,
				      param_types.length (),
				      param_types.address (), is_variadic,
				      false);

      target_function_types[string_name] = func_type;
    }
  }
  return decl;
}

static bool
jit_langhook_global_bindings_p (void)
{
  return true;
}

static tree
jit_langhook_pushdecl (tree decl ATTRIBUTE_UNUSED)
{
  return NULL_TREE;
}

static tree
jit_langhook_getdecls (void)
{
  return NULL;
}

static tree
jit_langhook_eh_personality (void)
{
  if (personality_decl == NULL_TREE)
  {
    if (jit_personality_func_name != NULL) {
      personality_decl = build_personality_function_with_name (jit_personality_func_name);
      jit_preserve_from_gc(personality_decl);
    }
    else {
      return lhd_gcc_personality();
    }
  }
  return personality_decl;
}

#undef LANG_HOOKS_EH_PERSONALITY
#define LANG_HOOKS_EH_PERSONALITY jit_langhook_eh_personality

#undef LANG_HOOKS_NAME
#define LANG_HOOKS_NAME		"libgccjit"

#undef LANG_HOOKS_INIT
#define LANG_HOOKS_INIT		jit_langhook_init

#undef LANG_HOOKS_PARSE_FILE
#define LANG_HOOKS_PARSE_FILE		jit_langhook_parse_file

#undef LANG_HOOKS_TYPE_FOR_MODE
#define LANG_HOOKS_TYPE_FOR_MODE	jit_langhook_type_for_mode

#undef LANG_HOOKS_BUILTIN_FUNCTION
#define LANG_HOOKS_BUILTIN_FUNCTION	jit_langhook_builtin_function

#undef LANG_HOOKS_GLOBAL_BINDINGS_P
#define LANG_HOOKS_GLOBAL_BINDINGS_P	jit_langhook_global_bindings_p

#undef LANG_HOOKS_PUSHDECL
#define LANG_HOOKS_PUSHDECL		jit_langhook_pushdecl

#undef LANG_HOOKS_GETDECLS
#define LANG_HOOKS_GETDECLS		jit_langhook_getdecls

/* Attribute hooks.  */
#undef LANG_HOOKS_ATTRIBUTE_TABLE
#define LANG_HOOKS_ATTRIBUTE_TABLE jit_attribute_table

#undef  LANG_HOOKS_DEEP_UNSHARING
#define LANG_HOOKS_DEEP_UNSHARING	true

struct lang_hooks lang_hooks = LANG_HOOKS_INITIALIZER;

#include "gt-jit-dummy-frontend.h"
#include "gtype-jit.h"
