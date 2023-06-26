/* Linux support needed only by jit front-end.
   Copyright (C) 2023 Free Software Foundation, Inc.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "tm_jit.h"
#include "jit/jit-target.h"
#include "jit/jit-target-def.h"

// TODO: remove this hook?
/* Implement TARGET_JIT_OS_VERSIONS for Linux targets.  */

static void
linux_jit_os_builtins (void)
{
}

// TODO: remove this hook?
/* Implement TARGET_JIT_REGISTER_OS_TARGET_INFO for Linux targets.  */

static void
linux_jit_register_target_info (void)
{
}

#undef TARGET_JIT_OS_VERSIONS
#define TARGET_JIT_OS_VERSIONS linux_jit_os_builtins

#undef TARGET_JIT_REGISTER_OS_TARGET_INFO
#define TARGET_JIT_REGISTER_OS_TARGET_INFO linux_jit_register_target_info

struct gcc_targetjitm targetjitm = TARGETJITM_INITIALIZER;
