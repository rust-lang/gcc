/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m1,
  vuint16m1_t,
  vuint64m4_t,
  __riscv_vle16_v_u16m1,
  __riscv_vzext_vf4_u64m4,
  __riscv_vse64_v_u64m4,
  vzext_vf,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vzext\.vf4\s+v0,v3([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vzext\.vf4\s+v4,v7} 1 } } */
