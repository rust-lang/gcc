/* Routing of the generated insn attribute and DFA entry points.
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

#ifndef GCC_INSN_ATTR_OPS_H
#define GCC_INSN_ATTR_OPS_H

#include "insn-attr.h"
#include "target-backend.h"

#if ENABLE_MULTI_TARGET && !defined IN_TARGET_CODE

/* Route the generated entry points declared in insn-attr.h through the
   active backend descriptor.  Core files that consume them include this
   header in place of insn-attr.h, leaving the spelling of every use
   unchanged; ports and the generated definitions keep addressing the
   underlying functions.  insn-attr.h defines some of the names as stub
   macros when the target lacks the corresponding attribute; the
   descriptor of such a target captures the same stubs, so those
   definitions are simply dropped here.  The constant
   HAVE_ATTR_* flags of the special attributes are likewise
   captured by the descriptor.  */

#undef HAVE_ATTR_length
#undef HAVE_ATTR_enabled
#undef HAVE_ATTR_preferred_for_size
#undef HAVE_ATTR_preferred_for_speed
#undef insn_default_length
#undef insn_min_length
#undef insn_variable_length_p
#undef insn_current_length
#undef get_attr_enabled
#undef get_attr_preferred_for_size
#undef get_attr_preferred_for_speed

#define HAVE_ATTR_length (this_target_backend->attr_ops.x_have_attr_length)
#define HAVE_ATTR_enabled (this_target_backend->attr_ops.x_have_attr_enabled)
#define HAVE_ATTR_preferred_for_size \
  (this_target_backend->attr_ops.x_have_attr_preferred_for_size)
#define HAVE_ATTR_preferred_for_speed \
  (this_target_backend->attr_ops.x_have_attr_preferred_for_speed)
#define insn_default_length \
  (this_target_backend->attr_ops.x_insn_default_length)
#define insn_min_length (this_target_backend->attr_ops.x_insn_min_length)
#define insn_variable_length_p \
  (this_target_backend->attr_ops.x_insn_variable_length_p)
#define insn_current_length \
  (this_target_backend->attr_ops.x_insn_current_length)
#define get_attr_enabled (this_target_backend->attr_ops.x_get_attr_enabled)
#define get_attr_preferred_for_size \
  (this_target_backend->attr_ops.x_get_attr_preferred_for_size)
#define get_attr_preferred_for_speed \
  (this_target_backend->attr_ops.x_get_attr_preferred_for_speed)
#define num_delay_slots (this_target_backend->attr_ops.x_num_delay_slots)
#define eligible_for_delay (this_target_backend->attr_ops.x_eligible_for_delay)
#define const_num_delay_slots \
  (this_target_backend->attr_ops.x_const_num_delay_slots)
#define eligible_for_annul_true \
  (this_target_backend->attr_ops.x_eligible_for_annul_true)
#define eligible_for_annul_false \
  (this_target_backend->attr_ops.x_eligible_for_annul_false)
/* A target without a tune attribute has no init_sched_attrs: its
   generated insn-attr.h makes the name an empty statement and its
   descriptor carries null, so the routed call checks first.  */
inline void
target_backend_init_sched_attrs (void)
{
  if (this_target_backend->attr_ops.x_init_sched_attrs != NULL)
    this_target_backend->attr_ops.x_init_sched_attrs ();
}
#define init_sched_attrs target_backend_init_sched_attrs
#define insn_default_latency \
  (*this_target_backend->attr_ops.x_insn_default_latency)
#define bypass_p (this_target_backend->attr_ops.x_bypass_p)
#define insn_latency (this_target_backend->attr_ops.x_insn_latency)
#define maximal_insn_latency \
  (this_target_backend->attr_ops.x_maximal_insn_latency)
#define max_insn_queue_index \
  (*this_target_backend->attr_ops.x_max_insn_queue_index)
#define state_size (this_target_backend->attr_ops.x_state_size)
#define state_reset (this_target_backend->attr_ops.x_state_reset)
#define state_transition (this_target_backend->attr_ops.x_state_transition)
#define state_dead_lock_p (this_target_backend->attr_ops.x_state_dead_lock_p)
#define min_insn_conflict_delay \
  (this_target_backend->attr_ops.x_min_insn_conflict_delay)
#define print_reservation (this_target_backend->attr_ops.x_print_reservation)
#define dfa_start (this_target_backend->attr_ops.x_dfa_start)
#define dfa_finish (this_target_backend->attr_ops.x_dfa_finish)
#define dfa_clear_single_insn_cache \
  (this_target_backend->attr_ops.x_dfa_clear_single_insn_cache)

#endif

#endif
