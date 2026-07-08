/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m1,
  vint16mf2_t,
  vint64m2_t,
  __riscv_vle16_v_i16mf2,
  __riscv_vsext_vf4_i64m2,
  __riscv_vse64_v_i64m2,
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
