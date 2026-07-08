/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m1,
  vuint16mf4_t,
  vuint64m1_t,
  __riscv_vle16_v_u16mf4,
  __riscv_vzext_vf4_u64m1,
  __riscv_vse64_v_u64m1,
  vzext_vf,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-not {vzext\.vf4\s+v0,v3([^0-9]|$)} } } */
/* { dg-final { scan-assembler-not {vzext\.vf4\s+v4,v7} } } */
/* { dg-final { scan-assembler-not {vzext\.vf4\s+v8,v11} } } */
/* { dg-final { scan-assembler-not {vzext\.vf4\s+v12,v15} } } */
/* { dg-final { scan-assembler-not {vzext\.vf4\s+v16,v19} } } */
/* { dg-final { scan-assembler-not {vzext\.vf4\s+v20,v23} } } */
/* { dg-final { scan-assembler-not {vzext\.vf4\s+v24,v27} } } */
/* { dg-final { scan-assembler-not {vzext\.vf4\s+v28,v31} } } */
