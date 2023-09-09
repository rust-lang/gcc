/* Subroutines for the JIT front end on the x86 architecture.
   Copyright (C) 2023 Free Software Foundation, Inc.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target.h"
#include "tm.h"
#include "tm_jit.h"
#include "jit/jit-target.h"
#include "jit/jit-target-def.h"

/* Implement TARGET_JIT_CPU_VERSIONS for x86 targets.  */

void
ix86_jit_target_versions (void)
{
  if (TARGET_64BIT)
    {
      jit_add_target_info ("target_arch", "x86_64");

      // TODO
      /*if (TARGET_X32)
	jit_add_target_info ("target_arch", "D_X32");*/
    }
  else
    jit_add_target_info ("target_arch", "x86");
}

/* Implement TARGET_JIT_REGISTER_CPU_TARGET_INFO.  */

extern const char *host_detect_local_cpu (int argc, const char **argv);

#if TARGET_64BIT_DEFAULT
const char* x86_bits = "64";
#else
const char* x86_bits = "32";
#endif

void
ix86_jit_register_target_info (void)
{
  const char *params[] = {"arch", x86_bits};
  const char *arch = host_detect_local_cpu (2, params);

  fprintf (stderr, "***************************** Arch: %s\n**********************************", arch);

  const char* arg = "-march=";
  const char* arg_pos = strstr(arch, arg);
  fprintf (stderr, "***************************** arg_pos: %s\n**********************************", arg_pos);
  const char* arg_value = arg_pos + strlen(arg);
  fprintf (stderr, "***************************** arg_value: %s\n**********************************", arg_value);
  const char* arg_value_end = strchr(arg_value, ' ');

  size_t len = arg_value_end - arg_value;
  char *cpu = new char[len];
  strncpy(cpu, arg_value, len);
  cpu[len] = '\0';
  fprintf (stderr, "***************************** cpu: %s\n**********************************", cpu);
  jit_target_set_arch (cpu);

  jit_target_set_128bit_int_support (targetm.scalar_mode_supported_p (TImode));

  free (const_cast <char *> (arch));

  if (TARGET_MMX)
    jit_add_target_info ("target_feature", "mmx");
  if (TARGET_SSE)
    jit_add_target_info("target_feature", "sse");
  if (TARGET_SSE2)
    jit_add_target_info("target_feature", "sse2");
  if (TARGET_SSE3)
    jit_add_target_info("target_feature", "sse3");
  if (TARGET_SSSE3)
    jit_add_target_info("target_feature", "ssse3");
  if (TARGET_SSE4_1)
    jit_add_target_info("target_feature", "sse4.1");
  if (TARGET_SSE4_2)
    jit_add_target_info("target_feature", "sse4.2");
  if (TARGET_AES)
    jit_add_target_info("target_feature", "aes");
  if (TARGET_SHA)
    jit_add_target_info("target_feature", "sha");
  if (TARGET_AVX)
    jit_add_target_info("target_feature", "avx");
  if (TARGET_AVX2)
    jit_add_target_info("target_feature", "avx2");
  if (TARGET_AVX512F)
    jit_add_target_info("target_feature", "avx512f");
  if (TARGET_AVX512ER)
    jit_add_target_info("target_feature", "avx512er");
  if (TARGET_AVX512CD)
    jit_add_target_info("target_feature", "avx512cd");
  if (TARGET_AVX512PF)
    jit_add_target_info("target_feature", "avx512pf");
  if (TARGET_AVX512DQ)
    jit_add_target_info("target_feature", "avx512dq");
  if (TARGET_AVX512BW)
    jit_add_target_info("target_feature", "avx512bw");
  if (TARGET_AVX512VL)
    jit_add_target_info("target_feature", "avx512vl");
  if (TARGET_AVX512VBMI)
    jit_add_target_info("target_feature", "avx512vbmi");
  if (TARGET_AVX512IFMA)
    jit_add_target_info("target_feature", "avx512ifma");
  if (TARGET_AVX512VPOPCNTDQ)
    jit_add_target_info("target_feature", "avx512vpopcntdq");
  if (TARGET_FMA)
    jit_add_target_info("target_feature", "fma");
  if (TARGET_RTM)
    jit_add_target_info("target_feature", "rtm");
  if (TARGET_SSE4A)
    jit_add_target_info("target_feature", "sse4a");
  if (TARGET_BMI) {
    jit_add_target_info("target_feature", "bmi1");
    jit_add_target_info("target_feature", "bmi");
  }
  if (TARGET_BMI2)
    jit_add_target_info("target_feature", "bmi2");
  if (TARGET_LZCNT)
    jit_add_target_info("target_feature", "lzcnt");
  if (TARGET_TBM)
    jit_add_target_info("target_feature", "tbm");
  if (TARGET_POPCNT)
    jit_add_target_info("target_feature", "popcnt");
  if (TARGET_RDRND) {
    jit_add_target_info("target_feature", "rdrand");
    jit_add_target_info("target_feature", "rdrnd");
  }
  if (TARGET_F16C)
    jit_add_target_info("target_feature", "f16c");
  if (TARGET_RDSEED)
    jit_add_target_info("target_feature", "rdseed");
  if (TARGET_ADX)
    jit_add_target_info("target_feature", "adx");
  if (TARGET_FXSR)
    jit_add_target_info("target_feature", "fxsr");
  if (TARGET_XSAVE)
    jit_add_target_info("target_feature", "xsave");
  if (TARGET_XSAVEOPT)
    jit_add_target_info("target_feature", "xsaveopt");
  if (TARGET_XSAVEC)
    jit_add_target_info("target_feature", "xsavec");
  if (TARGET_XSAVES)
    jit_add_target_info("target_feature", "xsaves");
  if (TARGET_VPCLMULQDQ) {
    jit_add_target_info("target_feature", "pclmulqdq");
    jit_add_target_info("target_feature", "vpclmulqdq");
  }
  if (TARGET_CMPXCHG16B)
    jit_add_target_info("target_feature", "cmpxchg16b");
  if (TARGET_MOVBE)
    jit_add_target_info("target_feature", "movbe");
  if (TARGET_AVX512VBMI2)
    jit_add_target_info("target_feature", "avx512vbmi2");
  if (TARGET_PKU)
    jit_add_target_info("target_feature", "pku");
  if (TARGET_AVX512VNNI)
    jit_add_target_info("target_feature", "avx512vnni");
  if (TARGET_AVX512BF16)
    jit_add_target_info("target_feature", "avx512bf16");
  if (TARGET_AVX512BITALG)
    jit_add_target_info("target_feature", "avx512bitalg");
  if (TARGET_AVX512VP2INTERSECT)
    jit_add_target_info("target_feature", "avx512vp2intersect");
  if (TARGET_PCLMUL)
    jit_add_target_info("target_feature", "pclmul");
  if (TARGET_GFNI)
    jit_add_target_info("target_feature", "gfni");
  if (TARGET_FMA4)
    jit_add_target_info("target_feature", "fma4");
  if (TARGET_XOP)
    jit_add_target_info("target_feature", "xop");

  // this is only enabled by choice in llvm, never by default - TODO determine if gcc enables it
  // jit_add_target_info("target_feature", "sse-unaligned-mem");

  if (TARGET_VAES)
    jit_add_target_info("target_feature", "vaes");
  if (TARGET_LWP)
    jit_add_target_info("target_feature", "lwp");
  if (TARGET_FSGSBASE)
    jit_add_target_info("target_feature", "fsgsbase");
  if (TARGET_SHSTK)
    jit_add_target_info("target_feature", "shstk");
  if (TARGET_PRFCHW)
    jit_add_target_info("target_feature", "prfchw");
  if (TARGET_SAHF) // would this be better as TARGET_USE_SAHF?
    jit_add_target_info("target_feature", "sahf");
  if (TARGET_MWAITX)
    jit_add_target_info("target_feature", "mwaitx");
  if (TARGET_CLZERO)
    jit_add_target_info("target_feature", "clzero");
  if (TARGET_CLDEMOTE)
    jit_add_target_info("target_feature", "cldemote");
  if (TARGET_PTWRITE)
    jit_add_target_info("target_feature", "ptwrite");
  bool hasERMSB = ix86_arch == PROCESSOR_HASWELL || ix86_arch == PROCESSOR_SKYLAKE
    || ix86_arch == PROCESSOR_SKYLAKE_AVX512 || ix86_arch == PROCESSOR_CANNONLAKE
    || ix86_arch == PROCESSOR_ICELAKE_CLIENT || ix86_arch == PROCESSOR_ICELAKE_SERVER
    || ix86_arch == PROCESSOR_CASCADELAKE || ix86_arch == PROCESSOR_TIGERLAKE
    || ix86_arch == PROCESSOR_COOPERLAKE;
  if (hasERMSB)
    jit_add_target_info("target_feature", "ermsbd");
}
