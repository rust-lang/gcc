/* The C-family surface of one target built into the compiler.
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

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target.h"
#include "c-family/c-common.h"
#include "memmodel.h"
#include "tm_p.h"
#include "c-family/c-pragma.h"
#include "c-family/c-backend.h"

/* A single-target build never compiles this file.  A multi-target
   build compiles it once as the primary target's surface, in the
   host's own header context, and once per secondary target, inside
   the target's header context, where tm.h carries the target's
   macros.  */

#ifndef MT_C_BACKEND_SYMBOL
# define MT_C_BACKEND_SYMBOL default_c_backend
#endif

/* The registry finds the primary target's surface by identity rather
   than by name; only the secondary compiles pass a triplet.  */
#ifndef MT_C_BACKEND_TRIPLE
# define MT_C_BACKEND_TRIPLE NULL
#endif

/* TARGET_CPU_CPP_BUILTINS expects the conveniences its call site in
   c_cpp_builtins defines around it.  */

static void
backend_cpu_cpp_builtins (cpp_reader *pfile ATTRIBUTE_UNUSED)
{
# define preprocessing_asm_p() (cpp_get_options (pfile)->lang == CLK_ASM)
# define preprocessing_trad_p() (cpp_get_options (pfile)->traditional)
# define builtin_define(TXT) cpp_define (pfile, TXT)
# define builtin_assert(TXT) cpp_assert (pfile, TXT)
  TARGET_CPU_CPP_BUILTINS ();
}

#ifdef REGISTER_TARGET_PRAGMAS
static void
backend_register_pragmas (void)
{
  REGISTER_TARGET_PRAGMAS ();
}
#endif

/* The surface is const yet must have external linkage for the
   registry's declaration to reach it.  */
extern const struct mt_c_backend MT_C_BACKEND_SYMBOL;

const struct mt_c_backend MT_C_BACKEND_SYMBOL =
{
  MT_C_BACKEND_TRIPLE,
  backend_cpu_cpp_builtins,
#ifdef REGISTER_TARGET_PRAGMAS
  backend_register_pragmas,
#else
  NULL,
#endif
};
