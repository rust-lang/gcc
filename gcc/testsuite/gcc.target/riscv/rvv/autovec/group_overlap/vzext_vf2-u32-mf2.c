/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e32m1,
  vuint32mf2_t,
  vuint64m1_t,
  __riscv_vle32_v_u32mf2,
  __riscv_vzext_vf2_u64m1,
  __riscv_vse64_v_u64m1,
  vzext_vf,
  LOOP_UNARY_BODY_X16)

/* { dg-final { scan-assembler-not {vzext\.vf2\s+v0,v1} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v2,v3} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v4,v5} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v6,v7} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v8,v9} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v10,v11} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v12,v13} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v14,v15} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v16,v17} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v18,v19} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v20,v21} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v22,v23} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v24,v25} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v26,v27} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v28,v29} } } */
/* { dg-final { scan-assembler-not {vzext\.vf2\s+v30,v31} } } */
