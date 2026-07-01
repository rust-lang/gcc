/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m1,
  vint16m1_t,
  vint32m2_t,
  __riscv_vle16_v_i16m1,
  __riscv_vsext_vf2_i32m2,
  __riscv_vse32_v_i32m2,
  vsext_vf,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-times {vsext\.vf2\s+v0,v1} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf2\s+v2,v3} 1 } } */
