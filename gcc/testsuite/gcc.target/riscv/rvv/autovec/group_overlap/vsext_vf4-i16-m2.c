/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m2,
  vint16m2_t,
  vint64m8_t,
  __riscv_vle16_v_i16m2,
  __riscv_vsext_vf4_i64m8,
  __riscv_vse64_v_i64m8,
  vsext_vf,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vsext\.vf4\s+v0,v6} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf4\s+v8,v14} 1 } } */
