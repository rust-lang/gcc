#ifndef HAVE_DEFINED_GROUP_OVERLAP_H
#define HAVE_DEFINED_GROUP_OVERLAP_H

#include <stdint.h>
#include <stdbool.h>
#include <riscv_vector.h>

#define LOOP_UNARY_BODY_X4(NT, WT, LD_F, OUT_F, ST_F, OUT, START, VL)  \
    NT vs0 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs1 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs2 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs3 = LD_F ((void *)START, VL); START += VL;                    \
                                                                       \
    asm volatile("nop" ::: "memory");                                  \
                                                                       \
    WT vd0 = OUT_F (vs0, VL);                                          \
    WT vd1 = OUT_F (vs1, VL);                                          \
    WT vd2 = OUT_F (vs2, VL);                                          \
    WT vd3 = OUT_F (vs3, VL);                                          \
                                                                       \
    asm volatile("nop" ::: "memory");                                  \
                                                                       \
    ST_F ((void *)out, vd0, VL); OUT += VL;                            \
    ST_F ((void *)out, vd1, VL); OUT += VL;                            \
    ST_F ((void *)out, vd2, VL); OUT += VL;                            \
    ST_F ((void *)out, vd3, VL); OUT += VL;                            \

#define LOOP_UNARY_BODY_X8(NT, WT, LD_F, OUT_F, ST_F, OUT, START, VL)  \
    NT vs0 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs1 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs2 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs3 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs4 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs5 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs6 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs7 = LD_F ((void *)START, VL); START += VL;                    \
                                                                       \
    asm volatile("nop" ::: "memory");                                  \
                                                                       \
    WT vd0 = OUT_F (vs0, VL);                                          \
    WT vd1 = OUT_F (vs1, VL);                                          \
    WT vd2 = OUT_F (vs2, VL);                                          \
    WT vd3 = OUT_F (vs3, VL);                                          \
    WT vd4 = OUT_F (vs4, VL);                                          \
    WT vd5 = OUT_F (vs5, VL);                                          \
    WT vd6 = OUT_F (vs6, VL);                                          \
    WT vd7 = OUT_F (vs7, VL);                                          \
                                                                       \
    asm volatile("nop" ::: "memory");                                  \
                                                                       \
    ST_F ((void *)out, vd0, VL); OUT += VL;                            \
    ST_F ((void *)out, vd1, VL); OUT += VL;                            \
    ST_F ((void *)out, vd2, VL); OUT += VL;                            \
    ST_F ((void *)out, vd3, VL); OUT += VL;                            \
    ST_F ((void *)out, vd4, VL); OUT += VL;                            \
    ST_F ((void *)out, vd5, VL); OUT += VL;                            \
    ST_F ((void *)out, vd6, VL); OUT += VL;                            \
    ST_F ((void *)out, vd7, VL); OUT += VL;                            \

#define LOOP_UNARY_BODY_X16(NT, WT, LD_F, OUT_F, ST_F, OUT, START, VL) \
    NT vs0 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs1 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs2 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs3 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs4 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs5 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs6 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs7 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs8 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs9 = LD_F ((void *)START, VL); START += VL;                    \
    NT vs10 = LD_F ((void *)START, VL); START += VL;                   \
    NT vs11 = LD_F ((void *)START, VL); START += VL;                   \
    NT vs12 = LD_F ((void *)START, VL); START += VL;                   \
    NT vs13 = LD_F ((void *)START, VL); START += VL;                   \
    NT vs14 = LD_F ((void *)START, VL); START += VL;                   \
    NT vs15 = LD_F ((void *)START, VL); START += VL;                   \
                                                                       \
    asm volatile("nop" ::: "memory");                                  \
                                                                       \
    WT vd0 = OUT_F (vs0, VL);                                          \
    WT vd1 = OUT_F (vs1, VL);                                          \
    WT vd2 = OUT_F (vs2, VL);                                          \
    WT vd3 = OUT_F (vs3, VL);                                          \
    WT vd4 = OUT_F (vs4, VL);                                          \
    WT vd5 = OUT_F (vs5, VL);                                          \
    WT vd6 = OUT_F (vs6, VL);                                          \
    WT vd7 = OUT_F (vs7, VL);                                          \
    WT vd8 = OUT_F (vs8, VL);                                          \
    WT vd9 = OUT_F (vs9, VL);                                          \
    WT vd10 = OUT_F (vs10, VL);                                        \
    WT vd11 = OUT_F (vs11, VL);                                        \
    WT vd12 = OUT_F (vs12, VL);                                        \
    WT vd13 = OUT_F (vs13, VL);                                        \
    WT vd14 = OUT_F (vs14, VL);                                        \
    WT vd15 = OUT_F (vs15, VL);                                        \
                                                                       \
    asm volatile("nop" ::: "memory");                                  \
                                                                       \
    ST_F ((void *)out, vd0, VL); OUT += VL;                            \
    ST_F ((void *)out, vd1, VL); OUT += VL;                            \
    ST_F ((void *)out, vd2, VL); OUT += VL;                            \
    ST_F ((void *)out, vd3, VL); OUT += VL;                            \
    ST_F ((void *)out, vd4, VL); OUT += VL;                            \
    ST_F ((void *)out, vd5, VL); OUT += VL;                            \
    ST_F ((void *)out, vd6, VL); OUT += VL;                            \
    ST_F ((void *)out, vd7, VL); OUT += VL;                            \
    ST_F ((void *)out, vd8, VL); OUT += VL;                            \
    ST_F ((void *)out, vd9, VL); OUT += VL;                            \
    ST_F ((void *)out, vd10, VL); OUT += VL;                           \
    ST_F ((void *)out, vd11, VL); OUT += VL;                           \
    ST_F ((void *)out, vd12, VL); OUT += VL;                           \
    ST_F ((void *)out, vd13, VL); OUT += VL;                           \
    ST_F ((void *)out, vd14, VL); OUT += VL;                           \
    ST_F ((void *)out, vd15, VL); OUT += VL;                           \

#define DEF_GROUP_OVERLAP_UNARY_0(VL_F, NT, WT, LD_F, OUT_F, ST_F, NAME, \
				  LOOP_BODY)                             \
  void test_group_overlap_##NAME##_##NT##_unary_0(uint8_t *data,         \
						 uint8_t *out,           \
						 size_t limit)           \
  {                                                                      \
    uint8_t *start = data;                                               \
    uint8_t *end = data + limit;                                         \
    size_t vl = VL_F ();                                                 \
                                                                         \
    while (start < end) {                                                \
      LOOP_BODY (NT, WT, LD_F, OUT_F, ST_F, out, start, vl);             \
    }                                                                    \
  }

#endif
