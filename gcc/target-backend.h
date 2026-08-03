/* Descriptors for the targets built into the compiler.
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

#ifndef GCC_TARGET_BACKEND_H
#define GCC_TARGET_BACKEND_H

/* Generator programs compile against the target's own headers and
   never address a backend descriptor; for them this header is
   empty.  */
#ifndef GENERATOR_FILE

#include "insn-codes.h"
#include "hashtab.h"

struct gcc_target;
struct insn_data_d;
struct target_optabs;
struct cl_target_option;
struct gcc_options;
struct output_block;
struct bitpack_d;
class data_in;
struct mode_tables;
struct gcc_targetm_common;
struct cl_option;
struct cl_enum;
struct cl_decoded_option;
struct cl_option_handlers;
struct cpp_options;
namespace diagnostics { class context; }
struct ggc_root_tab;
struct mt_register_tables;

/* The generated insn attribute and DFA scheduler entry points of one
   target (insn-attrtab.cc, insn-automata.cc).  Core consumers reach
   them through the routing macros of insn-attr-ops.h.  The fields
   carry an x_ prefix, as in target-globals: insn-attr.h defines
   several of these names as object-like stub macros on targets
   that lack the underlying attribute, and the fields must survive
   being compiled alongside it.  */

struct insn_attr_ops
{
  /* genattr's constant flags for the special attributes, which core
     tests at run time.  */
  bool x_have_attr_length;
  bool x_have_attr_enabled;
  bool x_have_attr_preferred_for_size;
  bool x_have_attr_preferred_for_speed;

  /* Variable-length insn support (stub hooks when the target has no
     length attribute).  */
  int (*x_insn_default_length) (rtx_insn *);
  int (*x_insn_min_length) (rtx_insn *);
  int (*x_insn_variable_length_p) (rtx_insn *);
  int (*x_insn_current_length) (rtx_insn *);

  /* The special boolean attributes consulted by the alternative
     filtering in recog.cc (stub hooks when not defined).  */
  int (*x_get_attr_enabled) (rtx_insn *);
  int (*x_get_attr_preferred_for_size) (rtx_insn *);
  int (*x_get_attr_preferred_for_speed) (rtx_insn *);

  /* Delay slot descriptions consumed by reorg.cc.  */
  int (*x_num_delay_slots) (rtx_insn *);
  int (*x_eligible_for_delay) (rtx_insn *, int, rtx_insn *, int);
  int (*x_const_num_delay_slots) (rtx_insn *);
  int (*x_eligible_for_annul_true) (rtx_insn *, int, rtx_insn *, int);
  int (*x_eligible_for_annul_false) (rtx_insn *, int, rtx_insn *, int);

  /* The DFA pipeline hazard recognizer (insn-automata.cc); null when
     the target has no insn reservations.  x_insn_default_latency points
     at the target's generated insn_default_latency function pointer
     variable, set up by init_sched_attrs.  The void * state arguments are the
     state_t of insn-attr.h.  */
  void (*x_init_sched_attrs) (void);
  int (**x_insn_default_latency) (rtx_insn *);
  int (*x_bypass_p) (rtx_insn *);
  int (*x_insn_latency) (rtx_insn *, rtx_insn *);
  int (*x_maximal_insn_latency) (rtx_insn *);
  const int *x_max_insn_queue_index;
  int (*x_state_size) (void);
  void (*x_state_reset) (void *);
  int (*x_state_transition) (void *, rtx);
  int (*x_state_dead_lock_p) (void *);
  int (*x_min_insn_conflict_delay) (void *, rtx_insn *, rtx_insn *);
  void (*x_print_reservation) (FILE *, rtx_insn *);
  void (*x_dfa_start) (void);
  void (*x_dfa_finish) (void);
  void (*x_dfa_clear_single_insn_cache) (rtx_insn *);
};

/* The generated cl_target_option entry points of one target
   (options-save.cc).  They operate on the target block of the option
   state; core consumers reach them through the target_backend_*
   accessors below.  */

struct cl_target_option_ops
{
  void (*x_cl_target_option_save) (struct cl_target_option *,
				 struct gcc_options *, struct gcc_options *);
  void (*x_cl_target_option_restore) (struct gcc_options *,
				    struct gcc_options *,
				    struct cl_target_option *);
  void (*x_cl_target_option_print) (FILE *, int, struct cl_target_option *);
  void (*x_cl_target_option_print_diff) (FILE *, int,
				       struct cl_target_option *,
				       struct cl_target_option *);
  bool (*x_cl_target_option_eq) (const struct cl_target_option *,
			       const struct cl_target_option *);
  hashval_t (*x_cl_target_option_hash) (const struct cl_target_option *);
  void (*x_cl_target_option_stream_out) (struct output_block *,
				       struct bitpack_d *,
				       struct cl_target_option *);
  void (*x_cl_target_option_stream_in) (class data_in *,
				      struct bitpack_d *,
				      struct cl_target_option *);
};

/* Everything the compiler needs in order to address one built-in
   target.  A single-target build has exactly one instance, describing
   the configured target; a multi-target build registers one instance
   per enabled target.  The structure grows fields as core consumers
   are converted to reach target-specific code through it.  */

struct target_backend
{
  /* The canonical target triplet this backend was built for.  */
  const char *triple;

  /* The backend's target hook vector.  */
  struct gcc_target *target_vector;

  /* The target's generated instruction table (insn-output.cc).  */
  const struct insn_data_d *insn_data;

  /* Generated recognizer entry points (insn-recog.cc, insn-extract.cc).  */
  int (*recog) (rtx, rtx_insn *, int *);
  void (*insn_extract) (rtx_insn *);
  rtx_insn *(*split_insns) (rtx, rtx_insn *);
  rtx_insn *(*peephole2_insns) (rtx, rtx_insn *, int *);

  /* Generated optab support (insn-opinit.cc); x_-prefixed like the
     insn_attr_ops fields, because insn-opinit.h renames these names
     in multi-target builds and reaches many consumers through
     optabs.h.  */
  void (*x_init_all_optabs) (struct target_optabs *);
  enum insn_code (*x_raw_optab_handler) (unsigned);

  /* Generated insn attribute and DFA entry points (insn-attrtab.cc,
     insn-automata.cc).  */
  struct insn_attr_ops attr_ops;

  /* Generated cl_target_option entry points (options-save.cc).  */
  struct cl_target_option_ops option_ops;


  /* The backend's common-target hook vector.  */
  const struct gcc_targetm_common *x_targetm_common;

  /* The target's option tables (options.cc): the decode table, the
     enumeration value tables, and — in multi-target builds — the
     name-order permutation that keeps find_opt's binary search
     working on the two-block table.  */
  const struct cl_option *x_cl_options;
  unsigned int x_cl_options_count;
  const struct cl_enum *x_cl_enums;
  unsigned int x_cl_enums_count;
  const unsigned short *x_cl_option_name_order;

  /* The target's option-state images (options.cc): the defaults the
     target was built with, and the blob's own working state, at the
     blob's padded layout.  */
  struct gcc_options *x_global_options;
  struct gcc_options *x_global_options_set;
  const struct gcc_options *x_global_options_init;

  /* The target-independent generated option handlers (options.cc).  */
  bool (*x_common_handle_option_auto) (struct gcc_options *,
				       struct gcc_options *,
				       const struct cl_decoded_option *,
				       unsigned int, int, location_t,
				       const struct cl_option_handlers *,
				       diagnostics::context *);
  void (*x_cpp_handle_option_auto) (const struct gcc_options *, size_t,
				    struct cpp_options *);
  void (*x_init_global_opts_from_cpp) (struct gcc_options *,
				       const struct cpp_options *);

  /* The target's machine mode value tables at the union
     numbering (mode-tables.h); null outside multi-target
     builds.  */
  const struct mode_tables *mode_tables;

  /* The target's runtime mode adjustments (insn-modes.cc, or the
     per-target adjust unit of a multi-target build); runs through
     the descriptor at the established point of the
     initialization sequence (do_compile).  */
  void (*init_adjust_machine_modes) (void);

  /* The GTY root tables of the target's blob (the per-target
     gengtype outputs), a null-terminated vector; null for the
     primary, whose roots live in the host tables.  */
  const struct ggc_root_tab *const *gt_ggc_roots;

  /* The target's register information (target-register-tables.cc),
     read from its macros inside its own header context.  */
  const struct mt_register_tables *register_tables;
};

/* The descriptor of the configured target.  */
extern const struct target_backend default_target_backend;

#if ENABLE_MULTI_TARGET
/* The backend the compiler is currently addressing; installed when
   a target is activated.  */
extern const struct target_backend *this_target_backend;
#else
/* A single-target build only ever addresses the configured
   target.  */
#define this_target_backend (&default_target_backend)
#endif

#ifndef GENERATOR_FILE

/* Declared in the generated options.h and (the stream pair) in
   lto-streamer.h; repeated here so the accessors below work wherever
   this header lands in a translation unit's include order.  */
extern void cl_target_option_save (struct cl_target_option *,
				   struct gcc_options *,
				   struct gcc_options *);
extern void cl_target_option_restore (struct gcc_options *,
				      struct gcc_options *,
				      struct cl_target_option *);
extern void cl_target_option_print (FILE *, int, struct cl_target_option *);
extern void cl_target_option_print_diff (FILE *, int,
					 struct cl_target_option *,
					 struct cl_target_option *);
extern bool cl_target_option_eq (const struct cl_target_option *,
				 const struct cl_target_option *);
extern hashval_t cl_target_option_hash (const struct cl_target_option *);
extern void cl_target_option_stream_out (struct output_block *,
					 struct bitpack_d *,
					 struct cl_target_option *);
extern void cl_target_option_stream_in (class data_in *,
					struct bitpack_d *,
					struct cl_target_option *);

/* Call the active target's generated cl_target_option entry points
   through the backend descriptor.  Single-target builds call the
   generated functions directly, at zero cost.  */

inline void
target_backend_cl_target_option_save (struct cl_target_option *ptr,
				      struct gcc_options *opts,
				      struct gcc_options *opts_set)
{
#if ENABLE_MULTI_TARGET
  this_target_backend->option_ops.x_cl_target_option_save (ptr, opts,
							 opts_set);
#else
  cl_target_option_save (ptr, opts, opts_set);
#endif
}

inline void
target_backend_cl_target_option_restore (struct gcc_options *opts,
					 struct gcc_options *opts_set,
					 struct cl_target_option *ptr)
{
#if ENABLE_MULTI_TARGET
  this_target_backend->option_ops.x_cl_target_option_restore (opts, opts_set,
							    ptr);
#else
  cl_target_option_restore (opts, opts_set, ptr);
#endif
}

inline void
target_backend_cl_target_option_print (FILE *file, int indent,
				       struct cl_target_option *ptr)
{
#if ENABLE_MULTI_TARGET
  this_target_backend->option_ops.x_cl_target_option_print (file, indent, ptr);
#else
  cl_target_option_print (file, indent, ptr);
#endif
}

inline void
target_backend_cl_target_option_print_diff (FILE *file, int indent,
					    struct cl_target_option *ptr1,
					    struct cl_target_option *ptr2)
{
#if ENABLE_MULTI_TARGET
  this_target_backend->option_ops.x_cl_target_option_print_diff (file, indent,
							       ptr1, ptr2);
#else
  cl_target_option_print_diff (file, indent, ptr1, ptr2);
#endif
}

inline bool
target_backend_cl_target_option_eq (const struct cl_target_option *ptr1,
				    const struct cl_target_option *ptr2)
{
#if ENABLE_MULTI_TARGET
  return this_target_backend->option_ops.x_cl_target_option_eq (ptr1, ptr2);
#else
  return cl_target_option_eq (ptr1, ptr2);
#endif
}

inline hashval_t
target_backend_cl_target_option_hash (const struct cl_target_option *ptr)
{
#if ENABLE_MULTI_TARGET
  return this_target_backend->option_ops.x_cl_target_option_hash (ptr);
#else
  return cl_target_option_hash (ptr);
#endif
}

inline void
target_backend_cl_target_option_stream_out (struct output_block *block,
					    struct bitpack_d *bitpack,
					    struct cl_target_option *ptr)
{
#if ENABLE_MULTI_TARGET
  this_target_backend->option_ops.x_cl_target_option_stream_out (block,
							       bitpack, ptr);
#else
  cl_target_option_stream_out (block, bitpack, ptr);
#endif
}

inline void
target_backend_cl_target_option_stream_in (class data_in *data_in,
					   struct bitpack_d *bitpack,
					   struct cl_target_option *ptr)
{
#if ENABLE_MULTI_TARGET
  this_target_backend->option_ops.x_cl_target_option_stream_in (data_in,
							      bitpack, ptr);
#else
  cl_target_option_stream_in (data_in, bitpack, ptr);
#endif
}

/* Run the active target's runtime machine mode adjustments (a
   generated per-target function; declared in machmode.h and repeated
   here for include-order independence).  */

extern void init_adjust_machine_modes (void);

inline void
target_backend_init_adjust_machine_modes (void)
{
#if ENABLE_MULTI_TARGET
  this_target_backend->init_adjust_machine_modes ();
#else
  init_adjust_machine_modes ();
#endif
}

#endif


#endif /* GENERATOR_FILE */
#endif
