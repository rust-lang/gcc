/* Print the compile-time layout values of one target.
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

/* A multi-target build compiles this program once per enabled target,
   inside the target's header context, and merges the outputs into
   mt-maxima.h — the superset values that size the structures shared
   by every backend.  Several of these macros are expressions over
   target enumerations, so the values can only be read by compiling
   against the target's own headers, exactly as the target's code
   does.  */

#include "bconfig.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "insn-codes.h"
#include "insn-config.h"

int
main (void)
{
  printf ("num_insn_codes %u\n", NUM_INSN_CODES);
  printf ("max_recog_operands %d\n", MAX_RECOG_OPERANDS);
#ifdef MAX_DUP_OPERANDS
  printf ("max_dup_operands %d\n", MAX_DUP_OPERANDS);
#else
  printf ("max_dup_operands 1\n");
#endif
#ifdef MAX_INSNS_PER_PEEP2
  printf ("max_insns_per_peep2 %d\n", MAX_INSNS_PER_PEEP2);
#else
  printf ("max_insns_per_peep2 0\n");
#endif
  printf ("first_pseudo_register %d\n", FIRST_PSEUDO_REGISTER);
  printf ("n_reg_classes %d\n", (int) N_REG_CLASSES);

  if (ferror (stdout) != 0 || fflush (stdout) != 0)
    return FATAL_EXIT_CODE;
  return SUCCESS_EXIT_CODE;
}
