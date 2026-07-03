// P2953R5 - Adding restrictions to defaulted assignment operator functions
// { dg-do compile { target c++11 } }

struct A {
  A (A &) = default;
  A &operator= (A &) = default;
};

template <class T>
struct B {
  T b;
  explicit B ();
  B (const B &) = default;		// { dg-error "explicitly defaulted copy constructor is implicitly deleted because its declared type does not match the type of an implicit copy constructor" "" { target c++17_down } }
					// { dg-message "note: expected signature: 'constexpr B<A>::B\\\(B<A>\\\&\\\)'" "" { target c++17_down } .-1 }
  B &operator= (const B &) = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_down } }
					// { dg-message "note: expected signature: 'constexpr B<A>\\\& B<A>::operator=\\\(B<A>\\\&\\\)'" "" { target { c++14 && c++17_down } } .-1 }
					// { dg-message "note: expected signature: 'B<A>\\\& B<A>::operator=\\\(B<A>\\\&\\\)'" "" { target c++11_only } .-2 }
};

B <A> c;
