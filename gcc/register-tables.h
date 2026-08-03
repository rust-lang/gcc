/* Per-target register information for multi-target builds.
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

#ifndef GCC_REGISTER_TABLES_H
#define GCC_REGISTER_TABLES_H

/* One target's register information, captured from its macros by
   target-register-tables.cc, which a multi-target build compiles
   once per enabled target inside the target's own header context.
   Everything is sized by the target's native counts; the consumers
   copy into the MAX_HARD_REGISTERS / MAX_REG_CLASSES superset
   structures and leave the rest zero.  */

/* Runtime-valued target macros host code must read from the ACTIVE
   target: on many ports these are expressions over the decoded
   option state, at the port's own field offsets, so each entry is a
   callable evaluated inside the target's context.  The mode-valued
   entries return the union mode number.  */

struct mt_target_scalars
{
  int (*x_char_type_size) (void);
  int (*x_short_type_size) (void);
  int (*x_int_type_size) (void);
  int (*x_long_type_size) (void);
  int (*x_long_long_type_size) (void);
  int (*x_pointer_size) (void);
  int (*x_units_per_word) (void);
  int (*x_bits_per_word) (void);
  int (*x_pmode) (void);
  int (*x_function_mode) (void);
  int (*x_stack_boundary) (void);
  int (*x_biggest_alignment) (void);
  int (*x_preferred_stack_boundary) (void);
  unsigned int (*x_max_stack_alignment) (void);
  int (*x_stack_pointer_regnum) (void);
  int (*x_frame_pointer_regnum) (void);
  int (*x_hard_frame_pointer_regnum) (void);
  int (*x_arg_pointer_regnum) (void);
  int (*x_move_max) (void);
  int (*x_move_max_pieces) (void);
  int (*x_store_max_pieces) (void);
  int (*x_compare_max_pieces) (void);
  int (*x_move_ratio) (int);
  int (*x_clear_ratio) (int);
  int (*x_set_ratio) (int);
  const char *(*x_type_operand_fmt) (void);
  const char *(*x_global_asm_op) (void);
  int (*x_function_boundary) (void);
  void (*x_asm_output_align) (FILE *, int);
  int (*x_all_regs) (void);
  int (*x_general_regs) (void);
};

/* One register elimination pair, as ELIMINABLE_REGS lists them.  */
struct mt_eliminable_pair
{
  int from;
  int to;
};

/* The DWARF and debugger register map entry points, captured in the
   descriptor's context, where cfun and the rtx builders are
   visible.  */

struct mt_dwarf_ops
{
  int (*x_dwarf_frame_registers) (void);
  unsigned int (*x_dwarf_frame_regnum) (int);
  unsigned int (*x_debugger_regno) (int);
  int (*x_dwarf_frame_return_column) (void);
  int (*x_incoming_frame_sp_offset) (void);
  unsigned int (*x_eh_return_data_regno) (int);
  rtx (*x_incoming_return_addr_rtx) (void);
};

/* The frame offset entry points, captured in the descriptor's
   context: a port's ACCUMULATE_OUTGOING_ARGS may read the function
   being compiled through cfun, and STACK_DYNAMIC_OFFSET builds on
   it.  */

struct mt_frame_offset_ops
{
  int (*x_accumulate_outgoing_args) (void);
  HOST_WIDE_INT (*x_first_parm_offset) (const_tree);
  HOST_WIDE_INT (*x_stack_pointer_offset) (void);

  /* Null unless the port defines the STACK_DYNAMIC_OFFSET macro;
     the host computes its own default from the members above
     otherwise.  */
  void (*x_stack_dynamic_offset) (const_tree, poly_int64 *);
};

/* The mode-switching entity table, captured in the descriptor's
   context.  */

struct mt_mode_switching_ops
{
  int (*x_n_entities) (void);
  int (*x_num_modes) (int);
  int (*x_optimize_p) (int);
};

struct mt_register_tables
{
  /* The target's own FIRST_PSEUDO_REGISTER and N_REG_CLASSES.  */
  unsigned int x_first_pseudo_register;
  int x_n_reg_classes;

  /* The number of 32-bit words per class in reg_class_contents.  */
  unsigned int x_n_reg_ints;

  /* FIXED_REGISTERS and CALL_USED_REGISTERS (or its really-used
     variant), each first_pseudo_register entries.  */
  const char *x_fixed_regs;
  const char *x_call_used_regs;

  /* REG_CLASS_CONTENTS: n_reg_classes rows of n_reg_ints words.  */
  const unsigned int *x_reg_class_contents;

  /* REG_ALLOC_ORDER, first_pseudo_register entries; null when the
     target allocates in register number order.  */
  const int *x_reg_alloc_order;

  /* REGISTER_NAMES and REG_CLASS_NAMES.  */
  const char *const *x_reg_names;
  const char *const *x_reg_class_names;

  /* REGNO_REG_CLASS, as a callable; the value is the target's own
     register class enumerator.  */
  int (*x_regno_reg_class) (unsigned int);

  /* The runtime-valued macros above.  */
  const struct mt_target_scalars *x_scalars;

  /* ELIMINABLE_REGS: the target's register elimination pairs.  */
  const struct mt_eliminable_pair *x_eliminable_regs;
  int x_eliminable_regs_count;
};

#endif /* GCC_REGISTER_TABLES_H */
