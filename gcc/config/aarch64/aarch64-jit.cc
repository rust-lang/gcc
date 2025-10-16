/* Subroutines for the jit front end on the AArch64 architecture.
   Copyright (C) 2025 Free Software Foundation, Inc.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1

#define INCLUDE_STRING
#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target.h"
#include "tm.h"
#include "tm_jit.h"
#include <sys/auxv.h>
#include "jit/jit-target.h"
#include "jit/jit-target-def.h"

/* Implement TARGET_JIT_REGISTER_CPU_TARGET_INFO.  */

#ifndef CROSS_DIRECTORY_STRUCTURE
extern const char *host_detect_local_cpu (int argc, const char **argv);
#endif

void
aarch64_jit_register_target_info (void)
{
  const char *params[] = {"arch"};
#ifndef CROSS_DIRECTORY_STRUCTURE
  const char* local_cpu = host_detect_local_cpu (2, params);
  if (local_cpu != NULL)
  {
    std::string arch = local_cpu;
    free (const_cast <char *> (local_cpu));

    const char* arg = "-march=";
    size_t arg_pos = arch.find (arg) + strlen (arg);
    size_t end_pos = arch.find (" ", arg_pos);

    std::string cpu = arch.substr (arg_pos, end_pos - arg_pos);
    jit_target_set_arch (cpu);
  }
#endif

#define AARCH64_OPT_EXTENSION(NAME, IDENT, REQUIRES, EXPLICIT_ON, \
  EXPLICIT_OFF, FEATURE_STRING) \
if (AARCH64_HAVE_ISA (IDENT)) jit_add_target_info_space ("target_feature", NAME, \
  FEATURE_STRING);
#include "aarch64-option-extensions.def"

  if (TARGET_BTI)
    jit_add_target_info ("target_feature", "bti");
  // TODO: features dit, dpb, dpb2, lor, pan, pmuv3, ras, spe, vh

#define AARCH64_ARCH(NAME, CORE, ARCH_IDENT, ARCH_REV, FLAGS) \
if (AARCH64_HAVE_ISA (ARCH_IDENT)) jit_add_target_info ("target_feature", NAME);
#include "aarch64-arches.def"
}
