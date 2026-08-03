/* Print the option-state sizes of one target.
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
   against the target's own options.h, and merges the outputs into
   per-target mt-options-pad.h headers.  Every target's gcc_options,
   cl_optimization and cl_target_option then pad themselves out to
   the largest layout, so that the option objects common code
   allocates can carry any enabled target's option state.  The
   compile defines MT_OPT_SIZE_PROBE, which keeps the pad members
   themselves out of the measurement, and GENERATOR_FILE is undefined
   around options.h because generator programs otherwise see extern
   declarations in place of the gcc_options structure.  */

#include "bconfig.h"
#include "system.h"
#include "coretypes.h"
#undef GENERATOR_FILE
#include "options.h"

int
main (void)
{
  printf ("gcc_options_size %d\n", (int) sizeof (struct gcc_options));
  printf ("gcc_options_align %d\n", (int) alignof (struct gcc_options));
  printf ("cl_optimization_size %d\n",
	  (int) sizeof (struct cl_optimization));
  printf ("cl_optimization_align %d\n",
	  (int) alignof (struct cl_optimization));
  printf ("cl_target_option_size %d\n",
	  (int) sizeof (struct cl_target_option));
  printf ("cl_target_option_align %d\n",
	  (int) alignof (struct cl_target_option));

  if (ferror (stdout) != 0 || fflush (stdout) != 0)
    return FATAL_EXIT_CODE;
  return SUCCESS_EXIT_CODE;
}
