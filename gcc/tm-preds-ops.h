/* Routing of the generated constraint surface to the active backend.
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

#ifndef GCC_TM_PREDS_OPS_H
#define GCC_TM_PREDS_OPS_H

/* The constraint entry points genpreds generates for one target,
   captured as callables so a multi-target host can hold one set per
   built-in target.  Constraint numbers, constraint types and
   register classes cross this surface as plain integers: their
   enumerations are per-target.  The register filter crosses as an
   untyped pointer for the same reason this header names no
   HARD_REG_SET: it lands in every translation unit that includes
   tm_p.h, most of which never touch register sets.  */

struct mt_constraint_ops
{
  int (*x_lookup_constraint) (const char *);
  bool (*x_constraint_satisfied_p) (rtx, int);
  bool (*x_insn_extra_memory_constraint) (int);
  bool (*x_insn_extra_special_memory_constraint) (int);
  bool (*x_insn_extra_relaxed_memory_constraint) (int);
  bool (*x_insn_extra_address_constraint) (int);
  void (*x_insn_extra_constraint_allows_reg_mem) (int, bool *, bool *);
  size_t (*x_insn_constraint_len) (char, const char *);
  int (*x_reg_class_for_constraint) (int);
  bool (*x_insn_const_int_ok_for_constraint) (HOST_WIDE_INT, int);
  int (*x_get_constraint_type) (int);
  const void *(*x_get_register_filter) (int);
  int (*x_get_register_filter_id) (int);
  int (*x_get_dependent_filter_id) (int);
  int (*x_get_dependent_filter_ref) (int);
  bool (*x_eval_dependent_filter) (int, unsigned int, machine_mode,
				   unsigned int, machine_mode);
};

/* Route the names the primary's own tm-preds.h declared above these
   definitions.  Ports compile against their own surface, the
   descriptor captures the native entry points, and generator
   programs carry no backend at all, so all three stay unrouted.  */

#if ENABLE_MULTI_TARGET && !defined (IN_TARGET_CODE) \
    && !defined (MT_NATIVE_TARGET_SURFACE) && !defined (GENERATOR_FILE)

#define lookup_constraint(P) \
  ((enum constraint_num) \
   this_target_backend->constraint_ops->x_lookup_constraint (P))
#define constraint_satisfied_p(X, C) \
  this_target_backend->constraint_ops->x_constraint_satisfied_p \
    ((X), (int) (C))
#define insn_extra_memory_constraint(C) \
  this_target_backend->constraint_ops->x_insn_extra_memory_constraint \
    ((int) (C))
#define insn_extra_special_memory_constraint(C) \
  this_target_backend->constraint_ops \
    ->x_insn_extra_special_memory_constraint ((int) (C))
#define insn_extra_relaxed_memory_constraint(C) \
  this_target_backend->constraint_ops \
    ->x_insn_extra_relaxed_memory_constraint ((int) (C))
#define insn_extra_address_constraint(C) \
  this_target_backend->constraint_ops->x_insn_extra_address_constraint \
    ((int) (C))
#define insn_extra_constraint_allows_reg_mem(C, REG, MEM) \
  this_target_backend->constraint_ops \
    ->x_insn_extra_constraint_allows_reg_mem ((int) (C), (REG), (MEM))
#define insn_constraint_len(FC, STR) \
  this_target_backend->constraint_ops->x_insn_constraint_len \
    ((FC), (STR))
#define reg_class_for_constraint(C) \
  ((enum reg_class) \
   this_target_backend->constraint_ops->x_reg_class_for_constraint \
     ((int) (C)))
#define insn_const_int_ok_for_constraint(V, C) \
  this_target_backend->constraint_ops \
    ->x_insn_const_int_ok_for_constraint ((V), (int) (C))
#define get_constraint_type(C) \
  ((enum constraint_type) \
   this_target_backend->constraint_ops->x_get_constraint_type \
     ((int) (C)))
#define get_register_filter(C) \
  ((const HARD_REG_SET *) \
   this_target_backend->constraint_ops->x_get_register_filter \
     ((int) (C)))
#define get_register_filter_id(C) \
  this_target_backend->constraint_ops->x_get_register_filter_id \
    ((int) (C))
#define get_dependent_filter_id(C) \
  this_target_backend->constraint_ops->x_get_dependent_filter_id \
    ((int) (C))
#define get_dependent_filter_ref(ID) \
  this_target_backend->constraint_ops->x_get_dependent_filter_ref (ID)
#define eval_dependent_filter(ID, REGNO, MODE, REF_REGNO, REF_MODE) \
  this_target_backend->constraint_ops->x_eval_dependent_filter \
    ((ID), (REGNO), (MODE), (REF_REGNO), (REF_MODE))

#endif

#endif /* GCC_TM_PREDS_OPS_H */
