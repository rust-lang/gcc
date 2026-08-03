/* The backend descriptor of one target built into the compiler.
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

/* The descriptor captures the native target surface; the routing
   headers must not send it back through the descriptor.  The macro
   must precede every include: defaults.h reads it, and tm.h pulls
   defaults.h in through backend.h.  */
#define MT_NATIVE_TARGET_SURFACE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "rtl.h"
#include "insn-config.h"
#include "recog.h"
#include "insn-attr.h"
#include "memmodel.h"
#include "optabs.h"
#include "target.h"
#include "target-backend.h"
/* The descriptor binds one target's own tables by their real
   names; see opts.h.  */
#define MT_OWN_OPTION_TABLES 1
#include "opts.h"
#include "common/common-target.h"
#include "ggc.h"
#include "register-tables.h"
/* For the declaration of the target's OVERRIDE_ABI_FORMAT function;
   tm.h itself arrives through backend.h.  */
#include "tm_p.h"
#include "tm-preds-ops.h"

/* A single-target build compiles this file once, as the descriptor of
   the configured target.  A multi-target build compiles it once per
   enabled target, inside the target's own header context: there the
   generated headers rename every entry point they declare into the
   target's symbol namespace, and MT_BACKEND_PREFIX — the same prefix
   the generators were given — renames the fixed surface whose macros
   live inside the generated translation units themselves.  */

#ifdef MT_BACKEND_PREFIX
# define MT_PASTE_1(a, b) a##b
# define MT_PASTE(a, b) MT_PASTE_1 (a, b)
# define MT_RENAMED(name) MT_PASTE (MT_BACKEND_PREFIX, name)
#else
# define MT_RENAMED(name) name
#endif

#ifndef MT_BACKEND_SYMBOL
# define MT_BACKEND_SYMBOL default_target_backend
#endif

#ifndef MT_BACKEND_TRIPLE
# define MT_BACKEND_TRIPLE TARGET_BACKEND_PRIMARY_TRIPLE
#endif

/* recog.h routes core references to insn_data through the descriptor;
   the descriptor itself must capture the underlying table.  */
#undef insn_data

/* The renamed generated symbols this file references without a
   declaration from the included headers.  */
#ifdef MT_BACKEND_PREFIX
extern const struct insn_data_d MT_RENAMED (insn_data)[];
extern int MT_RENAMED (recog) (rtx, rtx_insn *, int *);
extern void MT_RENAMED (insn_extract) (rtx_insn *);
extern rtx_insn *MT_RENAMED (split_insns) (rtx, rtx_insn *);
extern rtx_insn *MT_RENAMED (peephole2_insns) (rtx, rtx_insn *, int *);
#endif


/* The renamed options.cc and options-save.cc symbols; their
   declarations in opts.h and options.h carry the shared names, and
   the rename prologues live inside the generated units.  */
#ifdef MT_BACKEND_PREFIX
extern const struct cl_option MT_RENAMED (cl_options)[];
extern const unsigned int MT_RENAMED (cl_options_count);
extern const struct cl_enum MT_RENAMED (cl_enums)[];
extern const unsigned int MT_RENAMED (cl_enums_count);
extern const unsigned short MT_RENAMED (cl_option_name_order)[];
extern struct gcc_options MT_RENAMED (global_options);
extern struct gcc_options MT_RENAMED (global_options_set);
extern const struct gcc_options MT_RENAMED (global_options_init);
extern bool MT_RENAMED (common_handle_option_auto)
  (struct gcc_options *, struct gcc_options *,
   const struct cl_decoded_option *, unsigned int, int, location_t,
   const struct cl_option_handlers *, diagnostics::context *);
extern void MT_RENAMED (cpp_handle_option_auto)
  (const struct gcc_options *, size_t, struct cpp_options *);
extern void MT_RENAMED (init_global_opts_from_cpp)
  (struct gcc_options *, const struct cpp_options *);
/* A secondary target's tm.h renames these entry points for the
   port's own references; the descriptor spells the rename itself.  */
#undef cl_target_option_save
#undef cl_target_option_restore
#undef cl_target_option_print
#undef cl_target_option_print_diff
#undef cl_target_option_eq
#undef cl_target_option_hash
#undef cl_target_option_stream_out
#undef cl_target_option_stream_in

extern void MT_RENAMED (cl_target_option_save)
  (struct cl_target_option *, struct gcc_options *,
   struct gcc_options *);
extern void MT_RENAMED (cl_target_option_restore)
  (struct gcc_options *, struct gcc_options *,
   struct cl_target_option *);
extern void MT_RENAMED (cl_target_option_print)
  (FILE *, int, struct cl_target_option *);
extern void MT_RENAMED (cl_target_option_print_diff)
  (FILE *, int, struct cl_target_option *, struct cl_target_option *);
extern bool MT_RENAMED (cl_target_option_eq)
  (const struct cl_target_option *, const struct cl_target_option *);
extern hashval_t MT_RENAMED (cl_target_option_hash)
  (const struct cl_target_option *);
extern void MT_RENAMED (cl_target_option_stream_out)
  (struct output_block *, struct bitpack_d *,
   struct cl_target_option *);
extern void MT_RENAMED (cl_target_option_stream_in)
  (struct data_in *, struct bitpack_d *, struct cl_target_option *);
extern void MT_RENAMED (init_adjust_machine_modes) (void);
#endif


/* Which vector objects the descriptor binds.  A single-target build
   binds the configured target's vectors by their shared names.  In a
   multi-target build, a secondary target's tm.h renamed the tokens
   to the target's own vectors, so the shared names still bind them
   directly; the primary's copies name the vectors as target-def.h
   and its common counterpart define them, because the routing macros
   target.h installs for host code must not bind a descriptor through
   the switchable pointer.  */
#if ENABLE_MULTI_TARGET
# ifdef MT_TARGETM_RENAMED
#  define MT_TARGETM_REF (&targetm)
#  define MT_TARGETM_COMMON_REF (&targetm_common)
# else
extern struct gcc_target mt_targetm;
extern struct gcc_targetm_common mt_targetm_common;
#  define MT_TARGETM_REF (&mt_targetm)
#  define MT_TARGETM_COMMON_REF (&mt_targetm_common)
# endif
#else
# define MT_TARGETM_REF (&targetm)
# define MT_TARGETM_COMMON_REF (&targetm_common)
#endif

/* The per-target mode tables of a multi-target build; a single-target
   build has no use for them.  */
#ifdef MT_BACKEND_MODE_TABLES
extern const struct mode_tables MT_BACKEND_MODE_TABLES;
# define MT_BACKEND_MODE_TABLES_REF (&MT_BACKEND_MODE_TABLES)
#else
# define MT_BACKEND_MODE_TABLES_REF NULL
#endif

/* The GTY root tables of a secondary target's blob, aggregated by
   the generated mt-gt-roots.h; activation registers them.  The
   primary's roots live in the host tables, exactly as in a
   single-target build.  */
#if defined (MT_BACKEND_PREFIX) && defined (MT_TARGETM_RENAMED)
# define MT_GT_ROOT_TAB(name) extern const struct ggc_root_tab name[];
# include "mt-gt-roots.h"
# undef MT_GT_ROOT_TAB
static const struct ggc_root_tab *const mt_backend_gt_roots[] =
{
# define MT_GT_ROOT_TAB(name) name,
# include "mt-gt-roots.h"
# undef MT_GT_ROOT_TAB
  NULL
};
# define MT_BACKEND_GT_ROOTS_REF mt_backend_gt_roots
#else
# define MT_BACKEND_GT_ROOTS_REF NULL
#endif

/* The target's register information (target-register-tables.cc).  */
extern const struct mt_register_tables
  MT_RENAMED (target_register_tables);

/* The port's machine_function markers.  A secondary's aggregation
   header names its renamed marker; the primary's are the host's
   own, whose presence the generated wrapper macro signals.  In a
   multi-target build the primary must define machine_function
   whenever any enabled target does, or the field walk of struct
   function omits it.  */
#include "gtype-desc.h"
#if defined (MT_BACKEND_PREFIX) && defined (MT_TARGETM_RENAMED)
# ifdef MT_GT_MACHINE_FUNCTION_GGC_MX
extern void MT_GT_MACHINE_FUNCTION_GGC_MX (void *);
#  define MT_BACKEND_MACHINE_GGC_MX MT_GT_MACHINE_FUNCTION_GGC_MX
# else
#  define MT_BACKEND_MACHINE_GGC_MX NULL
# endif
# define MT_BACKEND_MACHINE_PCH_NX NULL
#elif defined (gt_ggc_m_16machine_function)
# define MT_BACKEND_MACHINE_GGC_MX gt_ggc_mx_machine_function
# define MT_BACKEND_MACHINE_PCH_NX gt_pch_nx_machine_function
#else
# define MT_BACKEND_MACHINE_GGC_MX NULL
# define MT_BACKEND_MACHINE_PCH_NX NULL
#endif

/* The enabled and preferred_for_* attributes return an int on some
   targets and a target-specific enum on others; these wrappers give
   the descriptor a uniform signature.  */

#if HAVE_ATTR_enabled
static int
get_attr_enabled_int (rtx_insn *insn)
{
  return get_attr_enabled (insn);
}
#endif

#if HAVE_ATTR_preferred_for_size
static int
get_attr_preferred_for_size_int (rtx_insn *insn)
{
  return get_attr_preferred_for_size (insn);
}
#endif

#if HAVE_ATTR_preferred_for_speed
static int
get_attr_preferred_for_speed_int (rtx_insn *insn)
{
  return get_attr_preferred_for_speed (insn);
}
#endif

/* OVERRIDE_ABI_FORMAT is the target's per-function ABI setup,
   expanded into allocate_struct_function; captured when the target
   defines one.  */
#ifdef OVERRIDE_ABI_FORMAT
static void
backend_override_abi_format (const_tree fndecl)
{
  OVERRIDE_ABI_FORMAT (fndecl);
}
#endif

/* INIT_EXPANDERS is the target's per-function expander state setup,
   expanded into init_emit; captured when the target defines one.  */
#ifdef INIT_EXPANDERS
static void
backend_init_expanders (void)
{
  INIT_EXPANDERS;
}
#endif

/* ADJUST_REG_ALLOC_ORDER is the target's allocation-order rewrite,
   run at allocator setup against the target's own options; captured
   when the target defines one.  */
#ifdef ADJUST_REG_ALLOC_ORDER
static void
backend_adjust_reg_alloc_order (void)
{
  ADJUST_REG_ALLOC_ORDER;
}
#endif

/* INITIAL_ELIMINATION_OFFSET computes the target's elimination
   offsets from its own frame layout.  */
static void
backend_initial_elimination_offset (int from, int to,
				    poly_int64 *offset)
{
  INITIAL_ELIMINATION_OFFSET (from, to, *offset);
}

/* The constraint entry points, captured with the enumerations
   carried as plain integers; see tm-preds-ops.h.  */

static int
backend_lookup_constraint (const char *p)
{
  return (int) lookup_constraint (p);
}

static bool
backend_constraint_satisfied_p (rtx x, int c)
{
  return constraint_satisfied_p (x, (enum constraint_num) c);
}

static bool
backend_insn_extra_memory_constraint (int c)
{
  return insn_extra_memory_constraint ((enum constraint_num) c);
}

static bool
backend_insn_extra_special_memory_constraint (int c)
{
  return insn_extra_special_memory_constraint ((enum constraint_num) c);
}

static bool
backend_insn_extra_relaxed_memory_constraint (int c)
{
  return insn_extra_relaxed_memory_constraint ((enum constraint_num) c);
}

static bool
backend_insn_extra_address_constraint (int c)
{
  return insn_extra_address_constraint ((enum constraint_num) c);
}

static void
backend_insn_extra_constraint_allows_reg_mem (int c, bool *allows_reg,
					      bool *allows_mem)
{
  insn_extra_constraint_allows_reg_mem ((enum constraint_num) c,
					allows_reg, allows_mem);
}

static size_t
backend_insn_constraint_len (char fc, const char *str)
{
  return insn_constraint_len (fc, str);
}

static int
backend_reg_class_for_constraint (int c)
{
  return (int) reg_class_for_constraint ((enum constraint_num) c);
}

static bool
backend_insn_const_int_ok_for_constraint (HOST_WIDE_INT value, int c)
{
  return insn_const_int_ok_for_constraint (value,
					   (enum constraint_num) c);
}

static int
backend_get_constraint_type (int c)
{
  return (int) get_constraint_type ((enum constraint_num) c);
}

static const void *
backend_get_register_filter (int c)
{
  return get_register_filter ((enum constraint_num) c);
}

static int
backend_get_register_filter_id (int c)
{
  return get_register_filter_id ((enum constraint_num) c);
}

static int
backend_get_dependent_filter_id (int c)
{
  return get_dependent_filter_id ((enum constraint_num) c);
}

static int
backend_get_dependent_filter_ref (int id)
{
  return get_dependent_filter_ref (id);
}

static bool
backend_eval_dependent_filter (int id, unsigned int regno,
			       machine_mode mode,
			       unsigned int ref_regno,
			       machine_mode ref_mode)
{
  return eval_dependent_filter (id, regno, mode, ref_regno, ref_mode);
}

static const struct mt_constraint_ops backend_constraint_ops =
{
  backend_lookup_constraint,
  backend_constraint_satisfied_p,
  backend_insn_extra_memory_constraint,
  backend_insn_extra_special_memory_constraint,
  backend_insn_extra_relaxed_memory_constraint,
  backend_insn_extra_address_constraint,
  backend_insn_extra_constraint_allows_reg_mem,
  backend_insn_constraint_len,
  backend_reg_class_for_constraint,
  backend_insn_const_int_ok_for_constraint,
  backend_get_constraint_type,
  backend_get_register_filter,
  backend_get_register_filter_id,
  backend_get_dependent_filter_id,
  backend_get_dependent_filter_ref,
  backend_eval_dependent_filter,
};

/* C++ gives a const object internal linkage unless it is declared
   extern first; the registry must see this symbol.  */
extern const struct target_backend MT_BACKEND_SYMBOL;

const struct target_backend MT_BACKEND_SYMBOL =
{
  MT_BACKEND_TRIPLE,
  MT_TARGETM_REF,
  MT_RENAMED (insn_data),
  MT_RENAMED (recog),
  MT_RENAMED (insn_extract),
  MT_RENAMED (split_insns),
  MT_RENAMED (peephole2_insns),
  init_all_optabs,
  raw_optab_handler,
  {
    HAVE_ATTR_length,
    HAVE_ATTR_enabled,
    HAVE_ATTR_preferred_for_size,
    HAVE_ATTR_preferred_for_speed,
    insn_default_length,
    insn_min_length,
    insn_variable_length_p,
    insn_current_length,
#if HAVE_ATTR_enabled
    get_attr_enabled_int,
#else
    /* The stub takes a plain rtx; core tests HAVE_ATTR_enabled before
       calling, so the descriptor can carry null instead.  */
    NULL,
#endif
#if HAVE_ATTR_preferred_for_size
    get_attr_preferred_for_size_int,
#else
    NULL,
#endif
#if HAVE_ATTR_preferred_for_speed
    get_attr_preferred_for_speed_int,
#else
    NULL,
#endif
    num_delay_slots,
    eligible_for_delay,
    const_num_delay_slots,
    eligible_for_annul_true,
    eligible_for_annul_false,
#ifdef INSN_SCHEDULING
# ifdef HAVE_INIT_SCHED_ATTRS
    init_sched_attrs,
    &insn_default_latency,
# else
    /* Without a tune attribute, init_sched_attrs is a stub macro and
       insn_default_latency a plain function; the activation phase
       supplies the wrapper.  */
    NULL,
    NULL,
# endif
    bypass_p,
    insn_latency,
    maximal_insn_latency,
    &max_insn_queue_index,
    state_size,
    state_reset,
    state_transition,
    state_dead_lock_p,
    min_insn_conflict_delay,
    print_reservation,
    dfa_start,
    dfa_finish,
    dfa_clear_single_insn_cache,
#else
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
#endif
  },
  {
    MT_RENAMED (cl_target_option_save),
    MT_RENAMED (cl_target_option_restore),
    MT_RENAMED (cl_target_option_print),
    MT_RENAMED (cl_target_option_print_diff),
    MT_RENAMED (cl_target_option_eq),
    MT_RENAMED (cl_target_option_hash),
    MT_RENAMED (cl_target_option_stream_out),
    MT_RENAMED (cl_target_option_stream_in)
  },

  MT_TARGETM_COMMON_REF,

  MT_RENAMED (cl_options),
  MT_RENAMED (cl_options_count),
  MT_RENAMED (cl_enums),
  MT_RENAMED (cl_enums_count),
#if ENABLE_MULTI_TARGET
  MT_RENAMED (cl_option_name_order),
#else
  /* Single-target option tables are sorted as a whole; no permutation
     exists or is needed.  */
  NULL,
#endif

  &MT_RENAMED (global_options),
  &MT_RENAMED (global_options_set),
  &MT_RENAMED (global_options_init),

  MT_RENAMED (common_handle_option_auto),
  MT_RENAMED (cpp_handle_option_auto),
  MT_RENAMED (init_global_opts_from_cpp),

  MT_BACKEND_MODE_TABLES_REF,
  MT_RENAMED (init_adjust_machine_modes),
  MT_BACKEND_GT_ROOTS_REF,
  &MT_RENAMED (target_register_tables),
  MT_BACKEND_MACHINE_GGC_MX,
  MT_BACKEND_MACHINE_PCH_NX,

#ifdef OVERRIDE_ABI_FORMAT
  backend_override_abi_format,
#else
  NULL,
#endif

#ifdef INIT_EXPANDERS
  backend_init_expanders,
#else
  NULL,
#endif

#ifdef ADJUST_REG_ALLOC_ORDER
  backend_adjust_reg_alloc_order,
#else
  NULL,
#endif

  backend_initial_elimination_offset,

  &backend_constraint_ops,
};
