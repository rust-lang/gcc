/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m1,
  vint8mf4_t,
  vint32m1_t,
  __riscv_vle8_v_i8mf4,
  __riscv_vsext_vf4_i32m1,
  __riscv_vse32_v_i32m1,
  vsext_vf,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-not {vsext\.vf4\s+v0,v3([^0-9]|$)} } } */
/* { dg-final { scan-assembler-not {vsext\.vf4\s+v4,v7} } } */
/* { dg-final { scan-assembler-not {vsext\.vf4\s+v8,v11} } } */
/* { dg-final { scan-assembler-not {vsext\.vf4\s+v12,v15} } } */
/* { dg-final { scan-assembler-not {vsext\.vf4\s+v16,v19} } } */
/* { dg-final { scan-assembler-not {vsext\.vf4\s+v20,v23} } } */
/* { dg-final { scan-assembler-not {vsext\.vf4\s+v24,v27} } } */
/* { dg-final { scan-assembler-not {vsext\.vf4\s+v28,v31} } } */
