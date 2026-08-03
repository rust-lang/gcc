/* Per-target driver specs captured for a multi-target compiler.
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

#ifndef GCC_MT_DRIVER_SPECS_H
#define GCC_MT_DRIVER_SPECS_H

/* One named spec, in the shape the EXTRA_SPECS and
   OPTION_DEFAULT_SPECS macros list them.  */

struct mt_driver_spec_entry
{
  const char *name;
  const char *spec;
};

/* The driver-facing spec surface of one enabled target, captured by
   compiling driver-specs.cc inside the target's own header context.
   The driver itself is built with the primary's specs; when a
   secondary is selected, it installs that target's captured values
   before any spec is processed.  */

struct mt_driver_specs
{
  /* The target's canonical triplet, the selection key.  */
  const char *triple;

  /* The target's ASM_SPEC.  */
  const char *asm_spec;

  /* The target's EXTRA_SPECS entries: the named specs its other
     specs reference through %(name).  */
  const struct mt_driver_spec_entry *extra_specs;
  size_t num_extra_specs;

  /* The target's DRIVER_SELF_SPECS entries.  They normalize the
     command line — forcing default endianness or ABI options onto
     it — and run in place of the primary's own entries.  */
  const char *const *self_specs;
  size_t num_self_specs;

  /* The assembler configure named for the target with
     --with-mt-as-<tag>=, or null.  */
  const char *configured_assembler;

  /* The target's OPTION_DEFAULT_SPECS entries.  They act on
     configure-time defaults, which only the primary has, so the
     driver does not run them; the capture keeps the surface whole
     for a per-target configuration channel to build on.  */
  const struct mt_driver_spec_entry *option_default_specs;
  size_t num_option_default_specs;
};

#endif
