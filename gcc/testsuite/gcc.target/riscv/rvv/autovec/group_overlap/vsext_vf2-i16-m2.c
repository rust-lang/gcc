/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m2,
  vint16m2_t,
  vint32m4_t,
  __riscv_vle16_v_i16m2,
  __riscv_vsext_vf2_i32m4,
  __riscv_vse32_v_i32m4,
  vsext_vf,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vsext\.vf2\s+v0,v2} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf2\s+v4,v6} 1 } } */
