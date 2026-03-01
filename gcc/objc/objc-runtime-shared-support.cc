/* Support routines shared by all runtimes.
   Copyright (C) 2011-2026 Free Software Foundation, Inc.
   Contributed by Iain Sandoe (partially split from objc-act.cc)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "stringpool.h"

#ifdef OBJCPLUS
#include "cp/cp-tree.h"
#else
#include "c/c-tree.h"
#include "c/c-lang.h"
#endif
#include "c-family/c-objc.h"
#include "objc-act.h"
#include "cgraph.h"
#include "stor-layout.h"

/* When building Objective-C++, we are not linking against the C front-end
   and so need to replicate the C tree-construction functions in some way.  */
#ifdef OBJCPLUS
#define OBJCP_REMAP_FUNCTIONS
#include "objcp-decl.h"
#endif  /* OBJCPLUS */

/* Hooks for string decls etc.  */
#include "objc-runtime-hooks.h"

#include "objc-runtime-shared-support.h"
#include "objc-next-metadata-tags.h"
#include "objc-encoding.h"

/* Rather than repeatedly looking up the identifiers, we save them here.  */
extern GTY(()) tree objc_rt_trees[OCTI_RT_META_MAX];
tree objc_rt_trees[OCTI_RT_META_MAX];

/* For building an objc struct.  These might not be used when this file
   is compiled as part of obj-c++.  */

static bool objc_building_struct;
static struct c_struct_parse_info *objc_struct_info ATTRIBUTE_UNUSED;

/* Start building a struct for objc.  */

tree
objc_start_struct (tree name)
{
  gcc_assert (!objc_building_struct);
  objc_building_struct = true;
  return start_struct (input_location, RECORD_TYPE, name, &objc_struct_info);
}

/* Finish building a struct for objc.  */

tree
objc_finish_struct (tree type, tree fieldlist)
{
  gcc_assert (objc_building_struct);
  objc_building_struct = false;
  return finish_struct (input_location, type, fieldlist, NULL_TREE,
			objc_struct_info);
}

tree
build_sized_array_type (tree base_type, int size)
{
  tree index_type = build_index_type (size_int (size - 1));
  return build_array_type (base_type, index_type);
}

/* Create a declaration for field NAME of a given TYPE.  */

static tree
create_field_decl (tree type, const char *name)
{
  return objc_build_decl (input_location,
		     FIELD_DECL, get_identifier (name), type);
}

tree
add_field_decl (tree type, const char *name, tree **chain)
{
  tree field = create_field_decl (type, name);

  if (*chain != NULL)
    **chain = field;
  *chain = &DECL_CHAIN (field);

  return field;
}

/* Objective-C Metadata decls (not-really variables, but we cannot use
   CONST_DECL yet - as that has limitations with address fields).
   Create a global, static declaration for variable NAME of a given TYPE.  The
   finish_meta_decl() routine will need to be called on it afterwards.  */

tree
start_meta_decl (tree type, const char *name)
{
#ifdef OBJCPLUS
  if (flag_next_runtime)
    /* Meta-data for the NeXT runtime is expected to be 'extern "C"'.  */
    push_lang_context (lang_name_c);
#endif
  tree name_id = get_identifier (name);
  tree var = objc_build_decl (input_location, VAR_DECL, name_id, type);
  TREE_STATIC (var) = true;
  DECL_IGNORED_P (var) = true;
  DECL_ARTIFICIAL (var) = true;
  DECL_INITIAL (var) = error_mark_node; /* A real initializer is coming... */
  DECL_CONTEXT (var) = NULL_TREE;
#ifdef OBJCPLUS
  /* Make sure that we don't stick in a mangled name for NeXT cases.  */
  if (flag_next_runtime)
    {
      SET_DECL_ASSEMBLER_NAME (var, DECL_NAME (var));
      pop_lang_context ();
    }
#endif
  return var;
}

/* Finish off the declaration created by start_meta_decl().  */

void
finish_meta_decl (tree var, tree initializer)
{
  gcc_checking_assert (!initializer || really_constant_p (initializer));
#if OBJCPLUS
  DECL_INITIAL (var) = initializer;
  if (initializer)
    {
      DECL_INITIALIZED_P (var) = true;
      DECL_NONTRIVIALLY_INITIALIZED_P (var) = true;
      DECL_INITIALIZED_BY_CONSTANT_EXPRESSION_P (var) = true;
    }
  layout_decl (var, 0);
  varpool_node::finalize_decl (var);
#else
  finish_decl (var, input_location, initializer, NULL_TREE, NULL_TREE);
#endif
}

/* Just a handy wrapper for add_objc_string.  */

tree
build_selector (tree ident)
{
  return convert (objc_selector_type, add_objc_string (ident, meth_var_names));
}

/* --- templates --- */

/* Set 'objc_super_template' to the data type node for 'struct _objc_super'.
   This needs to be done just once per compilation.  */

/* struct _objc_super {
     struct _objc_object *self;
     struct _objc_class *super_class;
		[or Class cls; for the abi v2]
   };  */

void
build_super_template (void)
{
  tree decls, *chain = NULL;

  objc_super_template = objc_start_struct (get_identifier (UTAG_SUPER));

  /* struct _objc_object *self; */
  decls = add_field_decl (objc_object_type, "self", &chain);

  /* struct _objc_class *super_class; */
  add_field_decl (build_pointer_type (objc_class_template),
		  "super_class", &chain);

  objc_finish_struct (objc_super_template, decls);
}

/* To accomplish method prototyping without generating all kinds of
   inane warnings, the definition of the dispatch table entries were
   changed from:

	struct objc_method { SEL _cmd; ...; id (*_imp)(); };
   to:
	struct objc_method { SEL _cmd; ...; void *_imp; };  */

tree
build_method_template (void)
{
  tree _SLT_record;
  tree decls, *chain = NULL;

  _SLT_record = objc_start_struct (get_identifier (UTAG_METHOD));

  /* SEL _cmd; */
  decls = add_field_decl (objc_selector_type, "_cmd", &chain);

  /* char *method_types; */
  add_field_decl (string_type_node, "method_types", &chain);

  /* void *_imp; */
  add_field_decl (build_pointer_type (void_type_node), "_imp", &chain);

  objc_finish_struct (_SLT_record, decls);

  return _SLT_record;
}

tree
build_method_prototype_template (void)
{
  tree proto_record;
  tree decls, *chain = NULL;

  proto_record = objc_start_struct (get_identifier (UTAG_METHOD_PROTOTYPE));

  /* SEL _cmd; */
  decls = add_field_decl (objc_selector_type, "_cmd", &chain);

  /* char *method_types; */
  add_field_decl (string_type_node, "method_types", &chain);

  objc_finish_struct (proto_record, decls);

  return proto_record;
}

/* struct {
     struct _objc__method_prototype_list *method_next;
     int method_count;
     struct objc_method method_list[method_count];
   };  */

tree
build_method_list_template (tree list_type, int size)
{
  tree objc_ivar_list_record;
  tree array_type, decls, *chain = NULL;

  objc_ivar_list_record = objc_start_struct (NULL);

  /* struct _objc__method_prototype_list *method_next; */
  decls = add_field_decl (objc_method_proto_list_ptr, "method_next", &chain);

  /* int method_count; */
  add_field_decl (integer_type_node, "method_count", &chain);

  /* struct objc_method method_list[]; */
  array_type = build_sized_array_type (list_type, size);
  add_field_decl (array_type, "method_list", &chain);

  objc_finish_struct (objc_ivar_list_record, decls);

  return objc_ivar_list_record;
}

/* struct objc_method_prototype_list {
     int count;
     struct objc_method_prototype {
	SEL name;
	char *types;
     } list[1];
   };  */

tree
build_method_prototype_list_template (tree list_type, int size)
{
  tree objc_ivar_list_record;
  tree array_type, decls, *chain = NULL;

  /* Generate an unnamed struct definition.  */

  objc_ivar_list_record = objc_start_struct (NULL_TREE);

  /* int method_count; */
  decls = add_field_decl (integer_type_node, "method_count", &chain);

  /* struct objc_method method_list[]; */
  array_type = build_sized_array_type (list_type, size);
  add_field_decl (array_type, "method_list", &chain);

  objc_finish_struct (objc_ivar_list_record, decls);

  return objc_ivar_list_record;
}

/* --- names, decls entry --- */

/* For each string section we have a chain which maps identifier nodes
   to decls for the strings.  */

static GTY(()) int meth_var_names_idx;
static GTY(()) int meth_var_types_idx;
static GTY(()) int property_name_attr_idx;

tree
add_objc_string (tree ident, string_section section)
{
  tree *chain, decl, type;
  char buf[BUFSIZE];

  switch (section)
    {
    case class_names:
      chain = &class_names_chain;
      snprintf (buf, BUFSIZE, "_OBJC_ClassName_%s", IDENTIFIER_POINTER (ident));
      break;
    case meth_var_names:
      chain = &meth_var_names_chain;
      snprintf (buf, BUFSIZE, "_OBJC_METH_VAR_NAME_%d", meth_var_names_idx++);
      break;
    case meth_var_types:
      chain = &meth_var_types_chain;
      snprintf (buf, BUFSIZE, "_OBJC_METH_VAR_TYPE_%d", meth_var_types_idx++);
      break;
    case prop_names_attr:
      chain = &prop_names_attr_chain;
      snprintf (buf, BUFSIZE, "_OBJC_PropertyAttributeOrName_%d", property_name_attr_idx++);
      break;
    default:
      gcc_unreachable ();
    }

  while (*chain)
    {
      if (TREE_VALUE (*chain) == ident)
	return convert (string_type_node,
			build_unary_op (input_location,
					ADDR_EXPR, TREE_PURPOSE (*chain), 1));

      chain = &TREE_CHAIN (*chain);
    }

  type = build_sized_array_type (char_type_node, IDENTIFIER_LENGTH (ident) + 1);
  /* Get a runtime-specific string decl which will be finish_var()'ed in
     generate_strings ().  */
  decl = (*runtime.string_decl) (type, buf, section);
  TREE_CONSTANT (decl) = 1;
  *chain = tree_cons (decl, ident, NULL_TREE);

  return convert (string_type_node,
		  build_unary_op (input_location, ADDR_EXPR, decl, 1));
}

/* --- shared metadata routines --- */

tree
build_descriptor_table_initializer (tree type, tree entries)
{
  vec<constructor_elt, va_gc> *inits = NULL;

  unsigned n = 0;
  do
    {
      vec<constructor_elt, va_gc> *elts = NULL;

      tree fields = first_type_field (type);
      CONSTRUCTOR_APPEND_ELT (elts, fields,
			      build_selector (METHOD_SEL_NAME (entries)));
      fields = next_type_field (fields);
      CONSTRUCTOR_APPEND_ELT (elts, fields,
			      add_objc_string (METHOD_ENCODING (entries),
					       meth_var_types));

      tree idx = size_int (n++);
      CONSTRUCTOR_APPEND_ELT (inits, idx,
			      objc_build_constructor (type, elts));

      entries = DECL_CHAIN (entries);
    }
  while (entries);

  tree idx_type = build_index_type (size_int (n));
  return objc_build_constructor (build_array_type (type, idx_type), inits);
}

tree
build_dispatch_table_initializer (tree type, tree entries)
{
  vec<constructor_elt, va_gc> *inits = NULL;

  unsigned n = 0;
  do
    {
      vec<constructor_elt, va_gc> *elems = NULL;

      tree fields = TYPE_FIELDS (type);
      CONSTRUCTOR_APPEND_ELT (elems, fields,
			      build_selector (METHOD_SEL_NAME (entries)));

      /* Generate the method encoding if we don't have one already.  */
      if (! METHOD_ENCODING (entries))
	METHOD_ENCODING (entries) = encode_method_prototype (entries);

      fields = next_type_field (fields);
      CONSTRUCTOR_APPEND_ELT (elems, fields,
			      add_objc_string (METHOD_ENCODING (entries),
					       meth_var_types));

      tree expr = convert (ptr_type_node,
			   build_unary_op (input_location, ADDR_EXPR,
					   METHOD_DEFINITION (entries), 1));
      fields = next_type_field (fields);
      CONSTRUCTOR_APPEND_ELT (elems, fields, expr);

      tree idx = size_int (n++);
      CONSTRUCTOR_APPEND_ELT (inits, idx,
			      objc_build_constructor (type, elems));

      entries = DECL_CHAIN (entries);
    }
  while (entries);

  tree idx_type = build_index_type (size_int (n));
  return objc_build_constructor (build_array_type (type, idx_type), inits);
}

/* Used only by build_*_selector_translation_table (). */
void
diagnose_missing_method (tree meth, location_t here)
{
  tree method_chain;
  bool found = false;
  for (method_chain = meth_var_names_chain;
       method_chain;
       method_chain = TREE_CHAIN (method_chain))
    {
      if (TREE_VALUE (method_chain) == meth)
	{
	  found = true;
	  break;
	}
     }

  if (!found)
    warning_at (here, 0, "creating selector for nonexistent method %qE",
			meth);
}


static tree
init_module_descriptor (tree type, long vers)
{
  vec<constructor_elt, va_gc> *v = NULL;

  /* version = { 1, ... } */

  tree expr = build_int_cst (long_integer_type_node, vers);
  tree fields = TYPE_FIELDS (type);
  CONSTRUCTOR_APPEND_ELT (v, fields, expr);

  /* size = { ..., sizeof (struct _objc_module), ... } */

  expr = convert (long_integer_type_node,
		  size_in_bytes (objc_module_template));
  fields = next_type_field (fields);
  CONSTRUCTOR_APPEND_ELT (v, fields, expr);

  /* Don't provide any file name for security reasons. */
  /* name = { ..., "", ... } */

  expr = add_objc_string (get_identifier (""), class_names);
  fields = next_type_field (fields);
  CONSTRUCTOR_APPEND_ELT (v, fields, expr);

  /* symtab = { ..., _OBJC_SYMBOLS, ... } */

  tree ltyp = build_pointer_type (xref_tag (RECORD_TYPE,
					    get_identifier (UTAG_SYMTAB)));

  /* No really useful place to point to.  */
  location_t loc = UNKNOWN_LOCATION;
  if (UOBJC_SYMBOLS_decl)
    expr = convert (ltyp, build_unary_op (loc,
			   ADDR_EXPR, UOBJC_SYMBOLS_decl, 0));
  else
    expr = build_zero_cst (ltyp);
  fields = next_type_field (fields);
  CONSTRUCTOR_APPEND_ELT (v, fields, expr);

  return objc_build_constructor (type, v);
}

/* Write out the data structures to describe Objective C classes defined.

   struct _objc_module { ... } _OBJC_MODULE = { ... };   */

void
build_module_descriptor (long vers, tree attr)
{
  tree decls, *chain = NULL;

#ifdef OBJCPLUS
  push_lang_context (lang_name_c); /* extern "C" */
#endif

  objc_module_template = objc_start_struct (get_identifier (UTAG_MODULE));

  /* long version; */
  decls = add_field_decl (long_integer_type_node, "version", &chain);

  /* long size; */
  add_field_decl (long_integer_type_node, "size", &chain);

  /* char *name; */
  add_field_decl (string_type_node, "name", &chain);

  /* struct _objc_symtab *symtab; */
  add_field_decl (build_pointer_type (xref_tag (RECORD_TYPE,
						get_identifier (UTAG_SYMTAB))),
		  "symtab", &chain);

  objc_finish_struct (objc_module_template, decls);

  /* Create an instance of "_objc_module".  */
  UOBJC_MODULES_decl = start_meta_decl (objc_module_template, "_OBJC_Module");

#ifdef OBJCPLUS
  pop_lang_context ();
#endif

  /* This is the root of the metadata for defined classes and categories, it
     is referenced by the runtime and, therefore, needed.  */
  DECL_PRESERVE_P (UOBJC_MODULES_decl) = 1;

  /* Squash `defined but not used' warning.  */
  TREE_USED (UOBJC_MODULES_decl) = 1;

  /* Allow the runtime to mark meta-data such that it can be assigned to target
     specific sections by the back-end.  */
  if (attr)
    DECL_ATTRIBUTES (UOBJC_MODULES_decl) = attr;

  TREE_ADDRESSABLE (UOBJC_MODULES_decl) = true;
  finish_meta_decl (UOBJC_MODULES_decl,
		   init_module_descriptor (TREE_TYPE (UOBJC_MODULES_decl),
					   vers));
}

tree
build_ivar_list_initializer (tree type, tree field_decl)
{
  vec<constructor_elt, va_gc> *inits = NULL;

  unsigned n = 0;
  for (; field_decl; field_decl = DECL_CHAIN (field_decl))
    if (TREE_CODE (field_decl) == FIELD_DECL)
      {
	vec<constructor_elt, va_gc> *ivar = NULL;

	/* Set name.  */
	tree fields = TYPE_FIELDS (type);
	if (DECL_NAME (field_decl))
	  CONSTRUCTOR_APPEND_ELT (ivar, fields,
				  add_objc_string (DECL_NAME (field_decl),
						   meth_var_names));
	else
	  /* Unnamed bit-field ivar (yuck).  */
	  CONSTRUCTOR_APPEND_ELT (ivar, fields,
				  build_int_cst (NULL_TREE, 0));

	/* Set type.  */
	fields = next_type_field (fields);
	tree id = add_objc_string (encode_field_decl (field_decl),
			      meth_var_types);
	CONSTRUCTOR_APPEND_ELT (ivar, fields, id);

	/* Set offset.  */
	fields = next_type_field (fields);
	CONSTRUCTOR_APPEND_ELT (ivar, fields, byte_position (field_decl));

	tree idx = size_int (n++);
	CONSTRUCTOR_APPEND_ELT (inits, idx,
				objc_build_constructor (type, ivar));
    }

  tree idx_type = build_index_type (size_int (n));
  return objc_build_constructor (build_array_type (type, idx_type), inits);
}

/* struct {
     int ivar_count;
     struct objc_ivar ivar_list[ivar_count];
   };  */

tree
build_ivar_list_template (tree list_type, int size)
{
  tree objc_ivar_list_record;
  tree array_type, decls, *chain = NULL;

  objc_ivar_list_record = objc_start_struct (NULL_TREE);

  /* int ivar_count; */
  decls = add_field_decl (integer_type_node, "ivar_count", &chain);

  /* struct objc_ivar ivar_list[]; */
  array_type = build_sized_array_type (list_type, size);
  add_field_decl (array_type, "ivar_list", &chain);

  objc_finish_struct (objc_ivar_list_record, decls);

  return objc_ivar_list_record;
}

/* struct _objc_ivar {
     char *ivar_name;
     char *ivar_type;
     int ivar_offset;
   };  */

tree
build_ivar_template (void)
{
  tree objc_ivar_id, objc_ivar_record;
  tree decls, *chain = NULL;

  objc_ivar_id = get_identifier (UTAG_IVAR);
  objc_ivar_record = objc_start_struct (objc_ivar_id);

  /* char *ivar_name; */
  decls = add_field_decl (string_type_node, "ivar_name", &chain);

  /* char *ivar_type; */
  add_field_decl (string_type_node, "ivar_type", &chain);

  /* int ivar_offset; */
  add_field_decl (integer_type_node, "ivar_offset", &chain);

  objc_finish_struct (objc_ivar_record, decls);

  return objc_ivar_record;
}

/* Used by NeXT ABI=0..2 */
void
build_next_selector_translation_table (void)
{
  tree chain;
  for (chain = sel_ref_chain; chain; chain = TREE_CHAIN (chain))
    {
      tree expr;
      tree decl = TREE_PURPOSE (chain);
      if (warn_selector)
	{
	  location_t loc;
	  if (decl)
	    loc = DECL_SOURCE_LOCATION (decl);
	  else
	    loc = UNKNOWN_LOCATION;
	  diagnose_missing_method (TREE_VALUE (chain), loc);
	}

      expr = build_selector (TREE_VALUE (chain));

      if (decl)
	{
	  /* Entries of this form are used for references to methods.
	  The runtime re-writes these on start-up, but the compiler can't see
	  that and optimizes it away unless we force it.  */
	  DECL_PRESERVE_P (decl) = 1;
	  finish_meta_decl (decl, expr);
	}
    }
}

void
generate_protocol_references (tree plist)
{
  tree lproto;

  /* Forward declare protocols referenced.  */
  for (lproto = plist; lproto; lproto = TREE_CHAIN (lproto))
    {
      tree proto = TREE_VALUE (lproto);

      if (TREE_CODE (proto) == PROTOCOL_INTERFACE_TYPE
	  && PROTOCOL_NAME (proto))
	{
          if (! PROTOCOL_FORWARD_DECL (proto))
            PROTOCOL_FORWARD_DECL (proto) = (*runtime.protocol_decl) (proto);

          if (PROTOCOL_LIST (proto))
            generate_protocol_references (PROTOCOL_LIST (proto));
        }
    }
}

/* --- new routines --- */

/* Output all strings.  */

/* FIXME: don't use global vars for all this... */

/* This emits all the meta-data string tables (and finalizes each var
   as it goes).  */
void
generate_strings (void)
{
  tree chain, string_expr;
  tree string, decl; /* , type;*/

  for (chain = class_names_chain; chain; chain = TREE_CHAIN (chain))
    {
      string = TREE_VALUE (chain);
      decl = TREE_PURPOSE (chain);
      string_expr = my_build_string (IDENTIFIER_LENGTH (string) + 1,
				     IDENTIFIER_POINTER (string));
      DECL_USER_ALIGN (decl) = 1;
      finish_meta_decl (decl, string_expr);
    }

  for (chain = meth_var_names_chain; chain; chain = TREE_CHAIN (chain))
    {
      string = TREE_VALUE (chain);
      decl = TREE_PURPOSE (chain);
      string_expr = my_build_string (IDENTIFIER_LENGTH (string) + 1,
				     IDENTIFIER_POINTER (string));
      DECL_USER_ALIGN (decl) = 1;
      finish_meta_decl (decl, string_expr);
    }

  for (chain = meth_var_types_chain; chain; chain = TREE_CHAIN (chain))
    {
      string = TREE_VALUE (chain);
      decl = TREE_PURPOSE (chain);
      string_expr = my_build_string (IDENTIFIER_LENGTH (string) + 1,
				     IDENTIFIER_POINTER (string));
      DECL_USER_ALIGN (decl) = 1;
      finish_meta_decl (decl, string_expr);
    }

  for (chain = prop_names_attr_chain; chain; chain = TREE_CHAIN (chain))
    {
      string = TREE_VALUE (chain);
      decl = TREE_PURPOSE (chain);
      string_expr = my_build_string (IDENTIFIER_LENGTH (string) + 1,
				     IDENTIFIER_POINTER (string));
      DECL_USER_ALIGN (decl) = 1;
      finish_meta_decl (decl, string_expr);
    }
}

#include "gt-objc-objc-runtime-shared-support.h"
