/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vint8m1_t,
  vint32m4_t,
  __riscv_vle8_v_i8m1,
  __riscv_vsext_vf4_i32m4,
  __riscv_vse32_v_i32m4,
  vsext_vf,
  LOOP_UNARY_BODY_X8)

/* { dg-final { scan-assembler-times {vsext\.vf4\s+v0,v3([^0-9]|$)} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf4\s+v4,v7} 1 } } */
