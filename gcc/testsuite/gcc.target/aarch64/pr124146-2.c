/* PR target/124146 */
/* { dg-do compile } */
/* { dg-options "-O2" } */

/* A type that combines an alignment attribute with an attribute that affects
   type identity (may_alias) is its own TYPE_MAIN_VARIANT and keeps
   TYPE_USER_ALIGN set.  aarch64_function_arg_alignment used to assert that
   the main variant had no user alignment, which ICEd for such types.  Verify
   that a wide range of them can be passed (alone and after another argument,
   over-aligned and under-aligned) and returned without an ICE.  */

typedef int v4si __attribute__((__vector_size__ (16)));

#define TEST(BASE, SUF)							\
  typedef __attribute__((__aligned__, __may_alias__)) BASE big_##SUF;	\
  typedef __attribute__((__aligned__ (8), __may_alias__)) BASE al8_##SUF; \
  void gbig_##SUF (big_##SUF);						\
  void hbig_##SUF (int, big_##SUF);					\
  void gal8_##SUF (al8_##SUF);						\
  void hal8_##SUF (int, al8_##SUF);					\
  void call_##SUF (big_##SUF a, al8_##SUF b)				\
  {									\
    gbig_##SUF (a);							\
    hbig_##SUF (1, a);							\
    gal8_##SUF (b);							\
    hal8_##SUF (1, b);							\
  }									\
  big_##SUF ret_##SUF (big_##SUF a) { return a; }

TEST (unsigned char, uc)
TEST (unsigned short, us)
TEST (unsigned int, ui)
TEST (unsigned long, ul)
TEST (long long, ll)
TEST (__int128, i128)
TEST (float, f)
TEST (double, d)
TEST (v4si, v)
