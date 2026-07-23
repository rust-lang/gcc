/* Language-independent attribute handlers shared across front ends.
   Copyright (C) 2026 Free Software Foundation, Inc.

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

#ifndef GCC_ATTR_HANDLERS_H
#define GCC_ATTR_HANDLERS_H

/* This header declares the machine-independent attribute handlers and the
   attribute exclusion tables that were historically copied into each front
   end.  The definitions live in attr-handlers.cc, which is linked into the
   front ends that use them (the C family, and jit) the same way attribs.o is.
   Only the handlers whose behaviour is identical across those front ends live
   here; dialect-specific variants (noreturn's Objective-C branch, cold's C++
   branch, malloc's deallocator form, nonnull, sentinel, the format checkers,
   ...) stay in the front end that needs them.  Including this header requires
   attribute_spec (tree-core.h, via tree.h) to be visible.  */

/* Attribute exclusion tables shared by the front ends that use the handlers
   declared below.  Each is a NULL-terminated array naming the attributes that
   are mutually exclusive with the one it is attached to.  */
extern const struct attribute_spec::exclusions attr_noreturn_exclusions[];
extern const struct attribute_spec::exclusions attr_returns_twice_exclusions[];
extern const struct attribute_spec::exclusions attr_alloc_exclusions[];
extern const struct attribute_spec::exclusions attr_const_pure_exclusions[];
extern const struct attribute_spec::exclusions attr_always_inline_exclusions[];
extern const struct attribute_spec::exclusions attr_cold_hot_exclusions[];
extern const struct attribute_spec::exclusions attr_noinline_exclusions[];
extern const struct attribute_spec::exclusions attr_target_exclusions[];
extern const struct attribute_spec::exclusions attr_section_exclusions[];

/* Shared helpers used by the handlers above and by front-end-specific
   handlers that validate the same kind of arguments (e.g. the C family's
   "ifunc" handler reuses handle_alias_ifunc_attribute).  */
extern bool validate_attr_args (tree node[2], tree name, tree newargs[2]);
extern bool validate_attr_arg (tree node[2], tree name, tree newarg);
extern tree handle_alias_ifunc_attribute (bool, tree *, tree, tree, bool *);

/* The machine-independent attribute handlers, referenced from the front-end
   attribute tables.  Each has the signature required by
   attribute_spec::handler.  */
extern tree handle_alias_attribute (tree *, tree, tree, int, bool *);
extern tree handle_always_inline_attribute (tree *, tree, tree, int, bool *);
extern tree handle_const_attribute (tree *, tree, tree, int, bool *);
extern tree handle_fnspec_attribute (tree *, tree, tree, int, bool *);
extern tree handle_leaf_attribute (tree *, tree, tree, int, bool *);
extern tree handle_noinline_attribute (tree *, tree, tree, int, bool *);
extern tree handle_nothrow_attribute (tree *, tree, tree, int, bool *);
extern tree handle_novops_attribute (tree *, tree, tree, int, bool *);
extern tree handle_pure_attribute (tree *, tree, tree, int, bool *);
extern tree handle_retain_attribute (tree *, tree, tree, int, bool *);
extern tree handle_returns_twice_attribute (tree *, tree, tree, int, bool *);
extern tree handle_section_attribute (tree *, tree, tree, int, bool *);
extern tree handle_target_attribute (tree *, tree, tree, int, bool *);
extern tree handle_transaction_pure_attribute (tree *, tree, tree, int, bool *);
extern tree handle_type_generic_attribute (tree *, tree, tree, int, bool *);
extern tree handle_used_attribute (tree *, tree, tree, int, bool *);
extern tree handle_visibility_attribute (tree *, tree, tree, int, bool *);
extern tree handle_weak_attribute (tree *, tree, tree, int, bool *);
extern tree ignore_attribute (tree *, tree, tree, int, bool *);

#endif /* GCC_ATTR_HANDLERS_H */
