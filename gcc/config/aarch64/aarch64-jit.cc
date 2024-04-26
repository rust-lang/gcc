/* Subroutines for the jit front end on the AArch64 architecture.
   Copyright (C) 2024 Free Software Foundation, Inc.

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

extern const char *host_detect_local_cpu (int argc, const char **argv);

void
aarch64_jit_register_target_info (void)
{
#define getCPUFeature(id, ftr) __asm__("mrs %0, " #id : "=r"(ftr))
#define extractBits(val, start, number) \
  (val & ((1ULL << number) - 1ULL) << start) >> start

  const char *params[] = {"arch"};
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

  if (TARGET_AES)
    jit_add_target_info ("target_feature", "aes");
  if (TARGET_BF16_FP)
    jit_add_target_info ("target_feature", "bf16");
  if (TARGET_BTI)
    jit_add_target_info ("target_feature", "bti");
  if (TARGET_COMPLEX)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "fcma");
  if (TARGET_CRC32)
    jit_add_target_info ("target_feature", "crc");
  if (TARGET_DOTPROD)
    jit_add_target_info ("target_feature", "dotprod");
  if (TARGET_SVE_F32MM)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "f32mm");
  if (TARGET_SVE_F64MM)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "f64mm");
  if (TARGET_F16FML)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "fhm");
  if (TARGET_FP_F16INST)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "fp16");
  if (TARGET_FRINT)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "frintts");
  if (TARGET_I8MM)
    jit_add_target_info ("target_feature", "i8mm");
  if (TARGET_JSCVT)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "jsconv");
  if (TARGET_LSE)
    jit_add_target_info ("target_feature", "lse");
  if (TARGET_MEMTAG)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "mte");
  if (TARGET_PAUTH)
  {
    jit_add_target_info ("target_feature", "paca");
    jit_add_target_info ("target_feature", "pacg");
  }
  if (TARGET_RNG)
    jit_add_target_info ("target_feature", "rand");
  if (TARGET_RCPC)
    jit_add_target_info ("target_feature", "rcpc");
  if (TARGET_RCPC2)
    jit_add_target_info ("target_feature", "rcpc2");
  if (TARGET_SIMD_RDMA)
    // TODO: check if this is the correct match.
    jit_add_target_info ("target_feature", "rdm");
  if (TARGET_SB)
    jit_add_target_info ("target_feature", "sb");
  if (TARGET_SHA2)
    jit_add_target_info ("target_feature", "sha2");
  if (TARGET_SHA3)
    jit_add_target_info ("target_feature", "sha3");
  if (TARGET_SIMD)
    jit_add_target_info ("target_feature", "neon");
  if (TARGET_SM4)
    jit_add_target_info ("target_feature", "sm4");
  if (TARGET_SVE)
    jit_add_target_info ("target_feature", "sve");
  if (TARGET_SVE2)
    jit_add_target_info ("target_feature", "sve2");
  if (TARGET_SVE2_AES)
    jit_add_target_info ("target_feature", "sve2-aes");
  if (TARGET_SVE2_BITPERM)
    jit_add_target_info ("target_feature", "sve2-bitperm");
  if (TARGET_SVE2_SHA3)
    jit_add_target_info ("target_feature", "sve2-sha3");
  if (TARGET_SVE2_SM4)
    jit_add_target_info ("target_feature", "sve2-sm4");
  if (TARGET_TME)
    jit_add_target_info ("target_feature", "tme");
  // TODO: features dit, dpb, dpb2, flagm, lor, pan, pmuv3, ras, spe, ssbs, vh

  if (AARCH64_HAVE_ISA (V8_1A))
    jit_add_target_info ("target_feature", "v8.1a");
  if (AARCH64_HAVE_ISA (V8_2A))
    jit_add_target_info ("target_feature", "v8.2a");
  if (AARCH64_HAVE_ISA (V8_3A))
    jit_add_target_info ("target_feature", "v8.3a");
  if (AARCH64_HAVE_ISA (V8_4A))
    jit_add_target_info ("target_feature", "v8.4a");
  if (AARCH64_HAVE_ISA (V8_5A))
    jit_add_target_info ("target_feature", "v8.5a");
  if (AARCH64_HAVE_ISA (V8_6A))
    jit_add_target_info ("target_feature", "v8.6a");
  if (AARCH64_HAVE_ISA (V8_7A))
    jit_add_target_info ("target_feature", "v8.7a");
}
