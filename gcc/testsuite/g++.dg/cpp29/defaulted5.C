// P2953R5 - Adding restrictions to defaulted assignment operator functions
// { dg-do compile { target c++11 } }

struct A {
  A &operator= (const A &) & = default;
};

struct B {
  B &&operator= (const B &) & = default;	// { dg-error "defaulted declaration 'B\\\&\\\& B::operator=\\\(const B\\\&\\\) \\\&' does not match the expected signature" }
};						// { dg-message "note: expected signature: 'constexpr B\\\& B::operator=\\\(const B\\\&\\\)'" "" { target c++14 } .-1 }
						// { dg-message "note: expected signature: 'B\\\& B::operator=\\\(const B\\\&\\\)'" "" { target c++11_only } .-2 }

struct C {
  C &operator= (const C &) noexcept (true) = default;
};

struct D {
  D &operator= (const D &) noexcept (false) = default;
};

struct E {
  E &operator= (E &) = default;
};

struct F {
  F (F &);
};

struct G {
  F a;
  G &operator= (const G &) & = default;
};

struct H {
  F a;
  H &operator= (const H &) && = default;	// { dg-error "defaulted declaration 'H\\\& H::operator=\\\(const H\\\&\\\) \\\&\\\&' does not match the expected signature" "" { target c++29 } }
};						// { dg-message "note: expected signature: 'constexpr H\\\& H::operator=\\\(const H\\\&\\\)'" "" { target c++29 } .-1 }

struct I {
  F a;
  I &operator= (const I &) const = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
};						// { dg-message "note: expected signature: 'constexpr I\\\& I::operator=\\\(const I\\\&\\\)'" "" { target c++14 } .-1 }
						// { dg-message "note: expected signature: 'I\\\& I::operator=\\\(const I\\\&\\\)'" "" { target c++11_only } .-2 }
						// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target { c++20 && c++26_down } } .-3 }
						// { dg-error "defaulted declaration 'I\\\& I::operator=\\\(const I\\\&\\\) const' does not match the expected signature" "" { target c++29 } .-4 }

struct J {
  F a;
  J &operator= (const J &) volatile = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
};						// { dg-message "note: expected signature: 'constexpr J\\\& J::operator=\\\(const J\\\&\\\)'" "" { target c++14 } .-1 }
						// { dg-message "note: expected signature: 'J\\\& J::operator=\\\(const J\\\&\\\)'" "" { target c++11_only } .-2 }
						// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target { c++20 && c++26_down } } .-3 }
						// { dg-error "defaulted declaration 'J\\\& J::operator=\\\(const J\\\&\\\) volatile' does not match the expected signature" "" { target c++29 } .-4 }

struct K {
  F a;
  K &operator= (const K &);
};

K &K::operator= (const K &) = default;

struct L {
  L &&operator= (L &) = default;		// { dg-error "defaulted declaration 'L\\\&\\\& L::operator=\\\(L\\\&\\\)' does not match the expected signature" }
};						// { dg-message "note: expected signature: 'constexpr L\\\& L::operator=\\\(L\\\&\\\)'" "" { target c++14 } .-1 }
						// { dg-message "note: expected signature: 'L\\\& L::operator=\\\(L\\\&\\\)'" "" { target c++11_only } .-2 }

struct M {
  M &operator= (M &);
};

struct N {
  M a;
  N &operator= (const N &) & = default;		// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
};						// { dg-message "note: expected signature: 'N\\\& N::operator=\\\(N\\\&\\\)'" "" { target *-*-* } .-1 }
						// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++20 } .-2 }

struct O {
  M a;
  O &operator= (const O &) && = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target { c++17_down } } }
};						// { dg-message "note: expected signature: 'O\\\& O::operator=\\\(O\\\&\\\)'" "" { target *-*-* } .-1 }
						// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target { c++20 && c++26_down } } .-2 }
						// { dg-error "defaulted declaration 'O\\\& O::operator=\\\(const O\\\&\\\) \\\&\\\&' does not match the expected signature" "" { target c++29 } .-3 }

struct P {
  M a;
  P &operator= (const P &) const = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
};						// { dg-message "note: expected signature: 'P\\\& P::operator=\\\(P\\\&\\\)'" "" { target *-*-* } .-1 }
						// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++20 } .-2 }

struct Q {
  M a;
  Q &operator= (const Q &) volatile = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
};						// { dg-message "note: expected signature: 'Q\\\& Q::operator=\\\(Q\\\&\\\)'" "" { target *-*-* } .-1 }
						// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++20 } .-2 }

struct R {
  M a;
  R &operator= (const R &);
};

R &R::operator= (const R &) = default;		// { dg-error "binding reference of type 'M\\\&' to 'const M' discards qualifiers" }
						// { dg-message "note: 'R\\\& R::operator=\\\(const R\\\&\\\)' is implicitly deleted because the default definition would be ill-formed:" "" { target *-*-* } .-1 }
