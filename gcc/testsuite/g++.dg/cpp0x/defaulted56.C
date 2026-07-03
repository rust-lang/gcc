// Core Issue #1331 (const mismatch with defaulted copy constructor)
// { dg-do compile { target c++11 } }

// If T2 (what would be the implicit declaration) has a parameter of
// type const C&, the corresponding parameter of T1 may be of type C&.

struct S
{
  constexpr S(S &) = default;
};

struct T
{
  constexpr T(volatile T &) = default; // { dg-error "defaulted" "" { target { c++17_down || c++29 } } }
				       // { dg-warning "implicitly deleted" "" { target { c++20 && c++26_down } } .-1 }
};

struct U
{
  constexpr U(const volatile U &) = default; // { dg-error "defaulted" "" { target { c++17_down || c++29 } } }
					     // { dg-warning "implicitly deleted" "" { target { c++20 && c++26_down } } .-1 }
};

struct V
{
  constexpr V(const V &) = default;
};
