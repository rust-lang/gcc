// P2953R5 - Adding restrictions to defaulted assignment operator functions
// { dg-do compile { target c++23 } }

struct A {
  A &operator= (this A &, const A &) = default;
};

struct D {
  D &&operator= (this D &, const D &) = default;	// { dg-error "defaulted declaration 'D\\\&\\\& D::operator=\\\(this D\\\&, const D\\\&\\\)' does not match the expected signature" }
};							// { dg-message "note: expected signature: 'constexpr D\\\& D::operator=\\\(const D\\\&\\\)'" "" { target *-*-* } .-1 }

struct E {
  E &operator= (this E, const E &) = default;		// { dg-error "'E\\\& E::operator=\\\(this E, const E\\\&\\\)' cannot be defaulted" }
};

struct F {
  F (F &);
};

struct G {
  F a;
  G &operator= (this G &, const G &) = default;
};

struct H {
  F a;
  H &operator= (this H &, H &) = default;
};

struct I {
  F a;
  I &operator= (this I, const I &) = default;		// { dg-error "'I\\\& I::operator=\\\(this I, const I\\\&\\\)' cannot be defaulted" }
};
