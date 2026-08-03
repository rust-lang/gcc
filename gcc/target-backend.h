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

struct gcc_target;
struct insn_data_d;
struct target_optabs;

/* The generated insn attribute and DFA scheduler entry points of one
   target (insn-attrtab.cc, insn-automata.cc).  Core consumers reach
   them through the routing macros of insn-attr-ops.h.  The fields
   carry an x_ prefix, as in target-globals: insn-attr.h defines
   several of these names as object-like stub macros on targets
   that lack the underlying attribute, and the fields must survive
   being compiled alongside it.  */

struct insn_attr_ops
{
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

#endif /* GENERATOR_FILE */
#endif
