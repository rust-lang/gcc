// P2953R5 - Adding restrictions to defaulted assignment operator functions
// { dg-do compile { target c++11 } }

// Define std::move.
namespace std {
  template <typename T>
  struct remove_reference
  { typedef T type; };

  template <typename T>
  struct remove_reference <T &>
  { typedef T type; };

  template <typename T>
  struct remove_reference <T &&>
  { typedef T type; };

  template <typename T>
  constexpr typename std::remove_reference <T>::type &&
  move (T &&t) noexcept
  { return static_cast <typename std::remove_reference <T>::type &&> (t); }
}

struct A {
  A (A &) = default;
  A &operator= (A &) = default;
};

void
foo (A a)
{
  a = a;
  A b = a;
}

struct B {
  B (const B &&) = default;		// { dg-error "explicitly defaulted move constructor is implicitly deleted because its declared type does not match the type of an implicit move constructor" "" { target c++17_down } }
					// { dg-warning "explicitly defaulted move constructor is implicitly deleted because its declared type does not match the type of an implicit move constructor" "" { target { c++20 && c++26_down } } .-1 }
					// { dg-error "defaulted declaration 'B::B\\\(const B\\\&\\\&\\\)' does not match the expected signature" "" { target c++29 } .-2 }
					// { dg-message "note: expected signature: 'constexpr B::B\\\(B\\\&\\\&\\\)'" "" { target *-*-* } .-3 }
  B &operator= (const B &&) = default;	// { dg-error "explicitly defaulted move assignment operator is implicitly deleted because its declared type does not match the type of an implicit move assignment operator" "" { target c++17_down } }
					// { dg-warning "explicitly defaulted move assignment operator is implicitly deleted because its declared type does not match the type of an implicit move assignment operator" "" { target { c++20 && c++26_down } } .-1 }
};					// { dg-error "defaulted declaration 'B\\\& B::operator=\\\(const B\\\&\\\&\\\)' does not match the expected signature" "" { target c++29 } .-2 }
					// { dg-message "note: expected signature: 'constexpr B\\\& B::operator=\\\(B\\\&\\\&\\\)'" "" { target c++14 } .-3 }
					// { dg-message "note: expected signature: 'B\\\& B::operator=\\\(B\\\&\\\&\\\)'" "" { target c++11_only } .-4 }

void
foo (B a)
{
  a = std::move (a);	// { dg-error "use of deleted function 'constexpr B\\\& B::operator=\\\(const B\\\&\\\&\\\)'" "" { target { c++14 && c++26_down } } }
			// { dg-error "use of deleted function 'B\\\& B::operator=\\\(const B\\\&\\\&\\\)'" "" { target c++11_only } .-1 }
  B b = std::move (a);	// { dg-error "use of deleted function 'constexpr B::B\\\(const B\\\&\\\&\\\)'" "" { target c++26_down } }
}

struct C {
  C &operator= (const C &) const = default; // { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
};					// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target { c++20 && c++26_down } } .-1 }
					// { dg-error "defaulted declaration 'C\\\& C::operator=\\\(const C\\\&\\\) const' does not match the expected signature" "" { target c++29 } .-2 }
					// { dg-message "note: expected signature: 'constexpr C\\\& C::operator=\\\(const C\\\&\\\)'" "" { target c++14 } .-3 }
					// { dg-message "note: expected signature: 'C\\\& C::operator=\\\(const C\\\&\\\)'" "" { target c++11_only } .-4 }

void
foo (C &)
{
  C c;
  c = c;	// { dg-error "use of deleted function 'constexpr C\\\& C::operator=\\\(const C\\\&\\\) const'" "" { target { c++14 && c++26_down } } }
}		// { dg-error "use of deleted function 'C\\\& C::operator=\\\(const C\\\&\\\) const'" "" { target c++11_only } .-1 }

struct D {
  D &operator= (const D &) && = default; // { dg-error "defaulted declaration 'D\\\& D::operator=\\\(const D\\\&\\\) \\\&\\\&' does not match the expected signature" "" { target c++29 } }
};		// { dg-message "note: expected signature: 'constexpr D\\\& D::operator=\\\(const D\\\&\\\)'" "" { target c++29 } .-1 }

void
foo (D a)
{
  std::move (a) = a;
}

struct E {
  E &&operator= (const E &) && = default; // { dg-error "defaulted declaration 'E\\\&\\\& E::operator=\\\(const E\\\&\\\) \\\&\\\&' does not match the expected signature" }
};					// { dg-message "note: expected signature: 'constexpr E\\\& E::operator=\\\(const E\\\&\\\)'" "" { target c++14 } .-1 }
					// { dg-message "note: expected signature: 'E\\\& E::operator=\\\(const E\\\&\\\)'" "" { target c++11_only } .-2 }

void
foo (E a)
{
  std::move (a) = a;
}

struct F {        
  F &operator= (const F &) volatile;	// { dg-message "note: initializing argument 1 of 'F\\\& F::operator=\\\(const F\\\&\\\) volatile'" }
};
struct G {
  volatile F f;
  G &operator= (const G &) = default;	// { dg-error "binding reference of type 'const F\\\&' to 'const volatile F' discards qualifiers" }
}; // { dg-message "note: 'G\\\& G::operator=\\\(const G\\\&\\\)' is implicitly deleted because the default definition would be ill-formed:" "" { target *-*-* } .-1 }

void
foo (G &)
{
  G a;
  decltype(a = a) b = a;	// { dg-error "cannot convert 'G' to 'int' in initialization" }
} // { dg-error "use of deleted function 'G\\\& G::operator=\\\(const G\\\&\\\)'" "" { target *-*-* } .-1 }

struct H {
  H &operator= (H &);		// { dg-message "note: initializing argument 1 of 'H\\\& H::operator=\\\(H\\\&\\\)'" }
};
struct I {
  H h;
  I &operator= (const I &);
};
I &I::operator= (const I &) = default;	// { dg-error "binding reference of type 'H\\\&' to 'const H' discards qualifiers" }
// { dg-message "note: 'I\\\& I::operator=\\\(const I\\\&\\\)' is implicitly deleted because the default definition would be ill-formed:" "" { target *-*-* } .-1 }
