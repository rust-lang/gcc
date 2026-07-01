/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e16m1,
  vint16mf4_t,
  vint32mf2_t,
  __riscv_vle16_v_i16mf4,
  __riscv_vsext_vf2_i32mf2,
  __riscv_vse32_v_i32mf2,
  vsext_vf,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-not {vsext\.vf2\s+v0,v1} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v2,v3} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v4,v5} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v6,v7} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v8,v9} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v10,v11} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v12,v13} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v14,v15} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v16,v17} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v18,v19} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v20,v21} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v22,v23} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v24,v25} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v26,v27} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v28,v29} } } */
/* { dg-final { scan-assembler-not {vsext\.vf2\s+v30,v31} } } */
