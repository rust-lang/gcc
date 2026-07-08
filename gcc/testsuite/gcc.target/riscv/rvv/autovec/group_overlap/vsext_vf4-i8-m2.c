/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m2,
  vint8m2_t,
  vint32m8_t,
  __riscv_vle8_v_i8m2,
  __riscv_vsext_vf4_i32m8,
  __riscv_vse32_v_i32m8,
  vsext_vf,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vsext\.vf4\s+v0,v6} 1 } } */
/* { dg-final { scan-assembler-times {vsext\.vf4\s+v8,v14} 1 } } */
