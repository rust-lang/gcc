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
};

#endif /* GCC_REGISTER_TABLES_H */
