// PR c++/125901
// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// [meta.rel]

#include <meta>

struct S;
union U;
struct B {};

// If Base and Derived are non-union class types and are not (possibly
// cv-qualified versions of) the same type, Derived shall be a complete type.
constexpr bool b1 = is_base_of_type (^^B, ^^S);	// { dg-error "preconditions not satisfied" }
constexpr bool b2 = is_base_of_type (^^B, ^^const S);	// { dg-error "preconditions not satisfied" }
static_assert (is_base_of_type (^^S, ^^S));
static_assert (is_base_of_type (^^S, ^^const S));
static_assert (!is_base_of_type (^^B, ^^U));

constexpr bool b15 = is_pointer_interconvertible_base_of_type (^^B, ^^S);	// { dg-error "preconditions not satisfied" }
constexpr bool b16 = is_pointer_interconvertible_base_of_type (^^B, ^^const S);	// { dg-error "preconditions not satisfied" }
static_assert (is_pointer_interconvertible_base_of_type (^^S, ^^S));
static_assert (is_pointer_interconvertible_base_of_type (^^S, ^^const S));
static_assert (!is_pointer_interconvertible_base_of_type (^^B, ^^U));

// If Base and Derived are non-union class types, Derived shall be a complete type.
constexpr bool b3 = is_virtual_base_of_type (^^B, ^^S);	// { dg-error "preconditions not satisfied" }
constexpr bool b4 = is_virtual_base_of_type (^^B, ^^const S);	// { dg-error "preconditions not satisfied" }
constexpr bool b5 = is_virtual_base_of_type (^^S, ^^S);	// { dg-error "preconditions not satisfied" }
constexpr bool b6 = is_virtual_base_of_type (^^S, ^^const S);	// { dg-error "preconditions not satisfied" }
static_assert (!is_virtual_base_of_type (^^B, ^^U));
static_assert (!is_virtual_base_of_type (^^U, ^^U));

// From and To shall be complete types, cv void, or arrays of unknown bound.
constexpr bool b7 = is_convertible_type (^^S, ^^B);   // { dg-error "preconditions not satisfied" }
constexpr bool b8 = is_convertible_type (^^B, ^^S);   // { dg-error "preconditions not satisfied" }
constexpr bool b9 = is_convertible_type (^^S, ^^B[]);   // { dg-error "preconditions not satisfied" }
constexpr bool b10 = is_convertible_type (^^B[], ^^S);   // { dg-error "preconditions not satisfied" }
constexpr bool b11 = is_convertible_type (^^S, ^^void);   // { dg-error "preconditions not satisfied" }
constexpr bool b12 = is_convertible_type (^^void, ^^S);   // { dg-error "preconditions not satisfied" }
static_assert (!is_convertible_type (^^S[], ^^B));
static_assert (!is_convertible_type (^^B, ^^U[]));

constexpr bool b7n = is_nothrow_convertible_type (^^S, ^^B);   // { dg-error "preconditions not satisfied" }
constexpr bool b8n = is_nothrow_convertible_type (^^B, ^^S);   // { dg-error "preconditions not satisfied" }
constexpr bool b9n = is_nothrow_convertible_type (^^S, ^^B[]);   // { dg-error "preconditions not satisfied" }
constexpr bool b10n = is_nothrow_convertible_type (^^B[], ^^S);   // { dg-error "preconditions not satisfied" }
constexpr bool b11n = is_nothrow_convertible_type (^^S, ^^void);   // { dg-error "preconditions not satisfied" }
constexpr bool b12n = is_nothrow_convertible_type (^^void, ^^S);   // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_convertible_type (^^S[], ^^B));
static_assert (!is_nothrow_convertible_type (^^B, ^^U[]));

// T and U shall be complete types, cv void, or arrays of unknown bound.
constexpr bool b13 = is_layout_compatible_type (^^S, ^^int);  // { dg-error "preconditions not satisfied" }
constexpr bool b14 = is_layout_compatible_type (^^int, ^^S);  // { dg-error "preconditions not satisfied" }
static_assert (!is_layout_compatible_type (^^S[], ^^int));
static_assert (!is_layout_compatible_type (^^int, ^^S[]));
static_assert (!is_layout_compatible_type (^^void, ^^int));
static_assert (!is_layout_compatible_type (^^int, ^^void));

// Fn and all types in the template parameter pack ArgTypes shall be
// complete types, cv void, or arrays of unknown bound.
constexpr bool b17 = is_invocable_type (^^S, { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b18 = is_invocable_type (^^int, { ^^S }); // { dg-error "preconditions not satisfied" }
constexpr bool b19 = is_invocable_type (^^S[1], { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b20 = is_invocable_type (^^int, { ^^S[1] }); // { dg-error "preconditions not satisfied" }
static_assert (!is_invocable_type (^^S[], { ^^int }));
static_assert (!is_invocable_type (^^int, { ^^S[] }));
constexpr bool b21 = is_invocable_type (^^U, { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b22 = is_invocable_type (^^int, { ^^U });  // { dg-error "preconditions not satisfied" }
constexpr bool b23 = is_invocable_type (^^U[1], { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b24 = is_invocable_type (^^int, { ^^U[1] });  // { dg-error "preconditions not satisfied" }
static_assert (!is_invocable_type (^^U[], { ^^int }));
static_assert (!is_invocable_type (^^int, { ^^U[] }));
static_assert (!is_invocable_type (^^int, { ^^void }));

constexpr bool b17n = is_nothrow_invocable_type (^^S, { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b18n = is_nothrow_invocable_type (^^int, { ^^S }); // { dg-error "preconditions not satisfied" }
constexpr bool b19n = is_nothrow_invocable_type (^^S[1], { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b20n = is_nothrow_invocable_type (^^int, { ^^S[1] }); // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_invocable_type (^^S[], { ^^int }));
static_assert (!is_nothrow_invocable_type (^^int, { ^^S[] }));
constexpr bool b21n = is_nothrow_invocable_type (^^U, { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b22n = is_nothrow_invocable_type (^^int, { ^^U });  // { dg-error "preconditions not satisfied" }
constexpr bool b23n = is_nothrow_invocable_type (^^U[1], { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b24n = is_nothrow_invocable_type (^^int, { ^^U[1] });  // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_invocable_type (^^U[], { ^^int }));
static_assert (!is_nothrow_invocable_type (^^int, { ^^U[] }));
static_assert (!is_nothrow_invocable_type (^^int, { ^^void }));

// Fn, R, and all types in the template parameter pack ArgTypes shall be
// complete types, cv void, or arrays of unknown bound.
constexpr bool b25 = is_invocable_r_type (^^S, ^^int, { ^^int }); // { dg-message "required from here" }
constexpr bool b26 = is_invocable_r_type (^^int, ^^S, { ^^S });	  // { dg-message "required from here" }
constexpr bool b27 = is_invocable_r_type (^^S[1], ^^int, { ^^int }); // { dg-message "required from here" }
constexpr bool b28 = is_invocable_r_type (^^int, ^^int, { ^^S[1] }); // { dg-message "required from here" }
constexpr bool b29 = is_invocable_r_type (^^int, ^^S, { ^^int });   // { dg-message "required from here" }
static_assert (!is_invocable_r_type (^^S[], ^^int, { ^^int }));
static_assert (!is_invocable_r_type (^^int, ^^int, { ^^S[] }));
constexpr bool b30 = is_invocable_r_type (^^U, ^^int, { ^^int });  // { dg-message "required from here" }
constexpr bool b31 = is_invocable_r_type (^^int, ^^int, { ^^U });  // { dg-message "required from here" }
constexpr bool b32 = is_invocable_r_type (^^U[1], ^^int, { ^^int });  // { dg-message "required from here" }
constexpr bool b33 = is_invocable_r_type (^^int, ^^int, { ^^U[1] });  // { dg-message "required from here" }
constexpr bool b34 = is_invocable_r_type (^^int, ^^U, { ^^int });     // { dg-message "required from here" }
static_assert (!is_invocable_r_type (^^U[], ^^int, { ^^int }));
static_assert (!is_invocable_r_type (^^int, ^^int, { ^^U[] }));
static_assert (!is_invocable_r_type (^^int, ^^U[], { ^^int }));
static_assert (!is_invocable_r_type (^^int, ^^int, { ^^void }));

constexpr bool b25n = is_nothrow_invocable_r_type (^^S, ^^int, { ^^int }); // { dg-message "required from here" }
constexpr bool b26n = is_nothrow_invocable_r_type (^^int, ^^S, { ^^S });	  // { dg-message "required from here" }
constexpr bool b27n = is_nothrow_invocable_r_type (^^S[1], ^^int, { ^^int }); // { dg-message "required from here" }
constexpr bool b28n = is_nothrow_invocable_r_type (^^int, ^^int, { ^^S[1] }); // { dg-message "required from here" }
constexpr bool b29n = is_nothrow_invocable_r_type (^^int, ^^S, { ^^int });   // { dg-message "required from here" }
static_assert (!is_nothrow_invocable_r_type (^^S[], ^^int, { ^^int }));
static_assert (!is_nothrow_invocable_r_type (^^int, ^^int, { ^^S[] }));
constexpr bool b30n = is_nothrow_invocable_r_type (^^U, ^^int, { ^^int });  // { dg-message "required from here" }
constexpr bool b31n = is_nothrow_invocable_r_type (^^int, ^^int, { ^^U });  // { dg-message "required from here" }
constexpr bool b32n = is_nothrow_invocable_r_type (^^U[1], ^^int, { ^^int });  // { dg-message "required from here" }
constexpr bool b33n = is_nothrow_invocable_r_type (^^int, ^^int, { ^^U[1] });  // { dg-message "required from here" }
constexpr bool b34n = is_nothrow_invocable_r_type (^^int, ^^U, { ^^int });     // { dg-message "required from here" }
static_assert (!is_nothrow_invocable_r_type (^^U[], ^^int, { ^^int }));
static_assert (!is_nothrow_invocable_r_type (^^int, ^^int, { ^^U[] }));
static_assert (!is_nothrow_invocable_r_type (^^int, ^^U[], { ^^int }));
static_assert (!is_nothrow_invocable_r_type (^^int, ^^int, { ^^void }));
// { dg-error "static assertion failed" "" { target *-*-* } 0 }
