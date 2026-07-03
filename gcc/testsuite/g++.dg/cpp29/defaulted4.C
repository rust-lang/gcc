// P2953R5 - Adding restrictions to defaulted assignment operator functions
// { dg-do compile { target c++11 } }

struct A {
  A (A &);
};

struct B {
  A a;
  B (const B &) = default;	// { dg-error "explicitly defaulted copy constructor is implicitly deleted because its declared type does not match the type of an implicit copy constructor" "" { target c++17_down } }
// { dg-warning "explicitly defaulted copy constructor is implicitly deleted because its declared type does not match the type of an implicit copy constructor" "" { target c++20 } .-1 }
}; // { dg-message "note: expected signature: 'B::B\\\(B\\\&\\\)'" "" { target *-*-* } .-2 }

struct C {
  C (C &);
};

struct D {
  C a;
  D &&operator= (const D &) = default;	// { dg-error "defaulted declaration 'D\\\&\\\& D::operator=\\\(const D\\\&\\\)' does not match the expected signature" }
}; // { dg-message "note: expected signature: 'constexpr D\\\& D::operator=\\\(const D\\\&\\\)'" "" { target c++14 } .-1 }
// { dg-message "note: expected signature: 'D\\\& D::operator=\\\(const D\\\&\\\)'" "" { target c++11_only } .-2 }

struct E {
  E &operator= (const E &) && = default;	// { dg-error "defaulted declaration 'E\\\& E::operator=\\\(const E\\\&\\\) \\\&\\\&' does not match the expected signature" "" { target c++29 } }
}; // { dg-message "note: expected signature: 'constexpr E\\\& E::operator=\\\(const E\\\&\\\)'" "" { target c++29 } .-1 }

struct F {
  F &operator= (const F &) const = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target { c++20 && c++26_down } } .-1 }
// { dg-error "defaulted declaration 'F\\\& F::operator=\\\(const F\\\&\\\) const' does not match the expected signature" "" { target c++29 } .-2 }
}; // { dg-message "note: expected signature: 'constexpr F\\\& F::operator=\\\(const F\\\&\\\)'" "" { target c++14 } .-3 }
// { dg-message "note: expected signature: 'F\\\& F::operator=\\\(const F\\\&\\\)'" "" { target c++11_only } .-4 }

struct G {
  G &operator= (const G &&) = default;	// { dg-error "explicitly defaulted move assignment operator is implicitly deleted because its declared type does not match the type of an implicit move assignment operator" "" { target c++17_down } }
// { dg-warning "explicitly defaulted move assignment operator is implicitly deleted because its declared type does not match the type of an implicit move assignment operator" "" { target { c++20 && c++26_down } } .-1 }
// { dg-error "defaulted declaration 'G\\\& G::operator=\\\(const G\\\&\\\&\\\)' does not match the expected signature" "" { target c++29 } .-2 }
}; // { dg-message "note: expected signature: 'constexpr G\\\& G::operator=\\\(G\\\&\\\&\\\)'" "" { target c++14 } .-3 }
// { dg-message "note: expected signature: 'G\\\& G::operator=\\\(G\\\&\\\&\\\)'" "" { target c++11_only } .-4 }
