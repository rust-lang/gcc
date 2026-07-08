/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d" } */

#include "group_overlap.h"

DEF_GROUP_OVERLAP_UNARY_0(
  __riscv_vsetvlmax_e8m2,
  vuint8m2_t,
  vuint32m8_t,
  __riscv_vle8_v_u8m2,
  __riscv_vzext_vf4_u32m8,
  __riscv_vse32_v_u32m8,
  vzext_vf,
  LOOP_UNARY_BODY_X4)

/* { dg-final { scan-assembler-times {vzext\.vf4\s+v0,v6} 1 } } */
/* { dg-final { scan-assembler-times {vzext\.vf4\s+v8,v14} 1 } } */
