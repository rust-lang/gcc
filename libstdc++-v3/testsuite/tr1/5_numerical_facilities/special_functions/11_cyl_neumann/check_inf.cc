// { dg-require-c-std "" }
// { dg-add-options ieee }

// TR1 5.2.1.11 cyl_neumann
// N_nu(+inf) == 0 for all finite nu >= 0

#include <tr1/cmath>
#include <limits>
#include <testsuite_hooks.h>

void
test01()
{
  float xinf_f = std::numeric_limits<float>::infinity();
  double xinf_d = std::numeric_limits<double>::infinity();
  long double xinf_l = std::numeric_limits<long double>::infinity();

  VERIFY(std::tr1::cyl_neumann(0.0F, xinf_f) == 0.0F);
  VERIFY(std::tr1::cyl_neumannf(0.0F, xinf_f) == 0.0F);
  VERIFY(std::tr1::cyl_neumann(0.0, xinf_d) == 0.0);
  VERIFY(std::tr1::cyl_neumann(0.0L, xinf_l) == 0.0L);
  VERIFY(std::tr1::cyl_neumannl(0.0L, xinf_l) == 0.0L);

  VERIFY(std::tr1::cyl_neumann(1.0F, xinf_f) == 0.0F);
  VERIFY(std::tr1::cyl_neumann(1.0, xinf_d) == 0.0);
  VERIFY(std::tr1::cyl_neumann(1.0L, xinf_l) == 0.0L);

  VERIFY(std::tr1::cyl_neumann(2.5, xinf_d) == 0.0);
}

int
main()
{
  test01();
  return 0;
}

