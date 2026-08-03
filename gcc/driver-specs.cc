/* The driver-facing specs of one target built into the compiler.
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
   inside the target's own header context, capturing the specs the
   driver would carry had that target been the configured one.  The
   build provides MT_DRIVER_SPECS_SYMBOL and MT_BACKEND_TRIPLE.  The
   capture is the native surface; the macro must precede every
   include, as in target-backend-def.cc.  */
#define MT_NATIVE_TARGET_SURFACE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "driver-spec-macros.h"
#include "mt-driver-specs.h"

/* The fallbacks gcc.cc applies for targets that leave the driver
   macros undefined.  */

#ifndef ASM_SPEC
#define ASM_SPEC ""
#endif

#ifndef DRIVER_SELF_SPECS
#define DRIVER_SELF_SPECS ""
#endif

#ifndef OPTION_DEFAULT_SPECS
#define OPTION_DEFAULT_SPECS { "", "" }
#endif

#ifdef EXTRA_SPECS
static const struct mt_driver_spec_entry mt_captured_extra_specs[]
  = { EXTRA_SPECS };
#endif

static const char *const mt_captured_self_specs[]
  = { DRIVER_SELF_SPECS };

static const struct mt_driver_spec_entry mt_captured_option_defaults[]
  = { OPTION_DEFAULT_SPECS };

/* C++ gives a const object internal linkage unless it is declared
   extern first; the driver must see this symbol.  */
extern const struct mt_driver_specs MT_DRIVER_SPECS_SYMBOL;

const struct mt_driver_specs MT_DRIVER_SPECS_SYMBOL =
{
  MT_BACKEND_TRIPLE,
  ASM_SPEC,
#ifdef EXTRA_SPECS
  mt_captured_extra_specs,
  ARRAY_SIZE (mt_captured_extra_specs),
#else
  NULL,
  0,
#endif
  mt_captured_self_specs,
  ARRAY_SIZE (mt_captured_self_specs),
  mt_captured_option_defaults,
  ARRAY_SIZE (mt_captured_option_defaults),
};
