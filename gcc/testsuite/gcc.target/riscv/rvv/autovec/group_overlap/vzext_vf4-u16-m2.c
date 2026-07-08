/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m2,
  vuint16m2_t,
  vuint64m8_t,
  __riscv_vle16_v_u16m2,
  __riscv_vzext_vf4_u64m8,
  __riscv_vse64_v_u64m8,
  vzext_vf,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vzext\.vf4\s+v0,v6} 1 } } */
/* { dg-final { scan-assembler-times {vzext\.vf4\s+v8,v14} 1 } } */
