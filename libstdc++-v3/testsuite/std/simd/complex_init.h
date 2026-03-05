#include <complex>

/**
 * This class is a workaround for std::complex not being allowed in template arguments.
 *
 * All it does is carry the real & imag values until it can "decay" into a std::complex.
 * There's no other interface.
 */
template <typename T>
  struct C
  {
    T re, im;

    template <typename U>
      constexpr operator std::complex<U>() const
      { return {U(re), U(im)}; }
  };
