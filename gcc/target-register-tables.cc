/* One target's register information, read from its macros.
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

/* A multi-target build compiles this file once per enabled target,
   inside the target's own header context, under the target's symbol
   renames.  The macros here are the ones reginfo.cc used to read
   directly, which a multi-target compiler must read from whichever
   target is active.  */

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include <initializer_list>
#include "coretypes.h"
#include "tm.h"
#include "tm_p.h"
#include "register-tables.h"

static const char table_fixed_regs[] = FIXED_REGISTERS;

#ifdef CALL_REALLY_USED_REGISTERS
static const char table_call_used_regs[] = CALL_REALLY_USED_REGISTERS;
#else
static const char table_call_used_regs[] = CALL_USED_REGISTERS;
#endif

/* Note that this hard-codes 32, not HOST_BITS_PER_INT, exactly as
   reginfo.cc does.  */
#define TABLE_REG_INTS ((FIRST_PSEUDO_REGISTER + (32 - 1)) / 32)

static const unsigned int
  table_class_contents[N_REG_CLASSES][TABLE_REG_INTS]
  = REG_CLASS_CONTENTS;

#ifdef REG_ALLOC_ORDER
/* The initializer is empty when the target allocates in register
   number order; an initializer list accepts that where an array
   definition would not.  */
static constexpr std::initializer_list<int> table_alloc_order
  = REG_ALLOC_ORDER;
#endif

static const char *const table_reg_names[] = REGISTER_NAMES;

static const char *const table_class_names[] = REG_CLASS_NAMES;

STATIC_ASSERT (ARRAY_SIZE (table_fixed_regs) == FIRST_PSEUDO_REGISTER);
STATIC_ASSERT (ARRAY_SIZE (table_call_used_regs)
	       == FIRST_PSEUDO_REGISTER);
STATIC_ASSERT (ARRAY_SIZE (table_reg_names) == FIRST_PSEUDO_REGISTER);
STATIC_ASSERT (ARRAY_SIZE (table_class_names) == N_REG_CLASSES);

/* REGNO_REG_CLASS as a callable; also the seed of the routing
   pointer, so it keeps its own linkage name.  */

int
target_regno_reg_class (unsigned int regno)
{
  return (int) REGNO_REG_CLASS (regno);
}

/* extern: a const object would otherwise have internal linkage in
   C++.  */
extern const struct mt_register_tables target_register_tables =
{
  FIRST_PSEUDO_REGISTER,
  N_REG_CLASSES,
  TABLE_REG_INTS,
  table_fixed_regs,
  table_call_used_regs,
  &table_class_contents[0][0],
#ifdef REG_ALLOC_ORDER
  /* An empty REG_ALLOC_ORDER means register number order.  */
  (table_alloc_order.size () != 0 ? table_alloc_order.begin () : NULL),
#else
  NULL,
#endif
  table_reg_names,
  table_class_names,
  target_regno_reg_class
};
