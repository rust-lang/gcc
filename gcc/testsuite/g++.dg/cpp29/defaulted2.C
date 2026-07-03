// P2953R5 - Adding restrictions to defaulted assignment operator functions
// { dg-do compile { target c++17 } }

#include <type_traits>

struct A {
  A &operator= (A &);
};
struct B {
  A a;
  B &operator= (const B &) = default;	// { dg-error "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++17_only } }
};					// { dg-warning "explicitly defaulted copy assignment operator is implicitly deleted because its declared type does not match the type of an implicit copy assignment operator" "" { target c++20 } .-1 }
					// { dg-message "note: expected signature: 'B\\\& B::operator=\\\(B\\\&\\\)'" "" { target *-*-* } .-2 }
static_assert (!std::is_assignable_v <B &, const B &>, "");
static_assert (!std::is_assignable_v <B &, B &>, "");
