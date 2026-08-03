/* Registry of the targets built into the compiler.
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
#include "target-registry.h"

/* recog.h routes core references to insn_data through the descriptor;
   the registry itself must capture the underlying table.  */
#undef insn_data

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

/* The descriptor of the configured target.  Its hook vector is the
   global one; multi-target builds will register the descriptors of
   the secondary targets alongside it.  */

const struct target_backend default_target_backend =
{
  TARGET_BACKEND_PRIMARY_TRIPLE,
  &targetm,
  insn_data,
  recog,
  insn_extract,
  split_insns,
  peephole2_insns,
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
    cl_target_option_save,
    cl_target_option_restore,
    cl_target_option_print,
    cl_target_option_print_diff,
    cl_target_option_eq,
    cl_target_option_hash,
    cl_target_option_stream_out,
    cl_target_option_stream_in
  },

  /* Per-target mode tables exist only in multi-target builds;
     the build glue fills this in.  */
  NULL
};

#if ENABLE_MULTI_TARGET
/* The backend the compiler is currently addressing.  */
const struct target_backend *this_target_backend
  = &default_target_backend;
#endif

const struct target_backend *const target_backend_registry[] =
{
  &default_target_backend
};

const unsigned int target_backend_count
  = ARRAY_SIZE (target_backend_registry);

/* Look BY_TRIPLE up in the registry; return null when no built-in
   target matches.  */

const struct target_backend *
find_target_backend (const char *by_triple)
{
  for (unsigned int index = 0; index < target_backend_count; index++)
    if (strcmp (target_backend_registry[index]->triple, by_triple) == 0)
      return target_backend_registry[index];
  return NULL;
}
