/* Per-target machine mode value tables for multi-target builds.
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

#ifndef GCC_MODE_TABLES_H
#define GCC_MODE_TABLES_H

/* Include this header after coretypes.h; it relies on the machine_mode
   enum and poly_uint16.

   In a multi-target build the mode tables machmode.h declares hold the
   ACTIVE target's values: activation copies them from the descriptor's
   instance of this structure, which genmodes-merge emits per target at
   the union mode numbering, and then runs the target's
   init_adjust_machine_modes.  A mode the target does not define keeps
   its intrinsic properties in the value tables — widest-mode idioms
   such as MAX_MODE_INT index them directly — while the link entries
   hold E_VOIDmode so iteration stays on the target's own modes;
   mode_present records which modes the target defines.  The mode
   names and classes are properties of the union itself and have no
   per-target table.  */

struct mode_tables
{
  poly_uint16 mode_precision[NUM_MACHINE_MODES];
  poly_uint16 mode_size[NUM_MACHINE_MODES];
  poly_uint16 mode_nunits[NUM_MACHINE_MODES];
  unsigned short mode_next[NUM_MACHINE_MODES];
  unsigned short mode_wider[NUM_MACHINE_MODES];
  unsigned short mode_2xwider[NUM_MACHINE_MODES];
  unsigned short mode_inner[NUM_MACHINE_MODES];
  unsigned char mode_unit_size[NUM_MACHINE_MODES];
  unsigned short mode_unit_precision[NUM_MACHINE_MODES];
  unsigned short mode_complex[NUM_MACHINE_MODES];
  unsigned HOST_WIDE_INT mode_mask_array[NUM_MACHINE_MODES];
  unsigned char mode_ibit[NUM_MACHINE_MODES];
  unsigned char mode_fbit[NUM_MACHINE_MODES];
  unsigned short mode_base_align[NUM_MACHINE_MODES];
  unsigned char mode_present[NUM_MACHINE_MODES];

  /* The target's narrowest mode of each class, not the union's: the
     union MIN_MODE_* bounds may name modes this target lacks.  */
  unsigned short class_narrowest_mode[MAX_MODE_CLASS];
};

#endif
