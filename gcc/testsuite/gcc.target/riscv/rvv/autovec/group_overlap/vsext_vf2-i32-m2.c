/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e32m2,
  vint32m2_t,
  vint64m4_t,
  __riscv_vle32_v_i32m2,
  __riscv_vsext_vf2_i64m4,
  __riscv_vse64_v_i64m4,
  vsext_vf,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vsext\.vf2\s+v0,v2} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf2\s+v4,v6} 1 } } */
