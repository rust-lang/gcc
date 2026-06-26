// PR c++/125901
// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }
// [meta.unary.prop]

#include <meta>

template<typename>
struct S;
union U;

// remove_all_extents_t<T> shall be a complete type or cv void.
constexpr bool b0 = is_trivially_copyable_type (^^S<int>);    // { dg-error "preconditions not satisfied" }
constexpr bool b1 = is_trivially_copyable_type (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
constexpr bool b0u = is_trivially_copyable_type (^^U);    // { dg-error "preconditions not satisfied" }
constexpr bool b1u = is_trivially_copyable_type (^^U[1]); // { dg-error "preconditions not satisfied" }
static_assert (!is_trivially_copyable_type (^^void));

constexpr bool b2 = is_standard_layout_type (^^S<int>);	      // { dg-error "preconditions not satisfied" }
constexpr bool b3 = is_standard_layout_type (^^S<int>[1]);    // { dg-error "preconditions not satisfied" }
constexpr bool b2u = is_standard_layout_type (^^U);	      // { dg-error "preconditions not satisfied" }
constexpr bool b3u = is_standard_layout_type (^^U[1]);    // { dg-error "preconditions not satisfied" }
static_assert (!is_standard_layout_type (^^void));

constexpr bool b11 = is_structural_type (^^S<int>);	      // { dg-error "preconditions not satisfied" }
constexpr bool b12 = is_structural_type (^^S<int>[1]);    // { dg-error "preconditions not satisfied" }
constexpr bool b11u = is_structural_type (^^U);	      // { dg-error "preconditions not satisfied" }
constexpr bool b12u = is_structural_type (^^U[1]);    // { dg-error "preconditions not satisfied" }
static_assert (!is_structural_type (^^void));

// If T is a non-union class type, T shall be a complete type.
constexpr bool b4 = is_empty_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
static_assert (!is_empty_type (^^S<int>[1]));
static_assert (!is_empty_type (^^U));
static_assert (!is_empty_type (^^U[1]));
static_assert (!is_empty_type (^^void));

constexpr bool b5 = is_polymorphic_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
static_assert (!is_polymorphic_type (^^S<int>[1]));
static_assert (!is_polymorphic_type (^^U));
static_assert (!is_polymorphic_type (^^U[1]));
static_assert (!is_polymorphic_type (^^void));

constexpr bool b6 = is_abstract_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
static_assert (!is_abstract_type (^^S<int>[1]));
static_assert (!is_abstract_type (^^U));
static_assert (!is_abstract_type (^^U[1]));
static_assert (!is_abstract_type (^^void));

// If T is a class type, T shall be a complete type.
constexpr bool b7 = is_final_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
constexpr bool b8 = is_final_type (^^U);	// { dg-error "preconditions not satisfied" }
static_assert (!is_final_type (^^S<int>[1]));
static_assert (!is_final_type (^^U[1]));
static_assert (!is_final_type (^^void));

// T shall be an array type, a complete type, or cv void.
static_assert (is_aggregate_type (^^S<int>[1]));
static_assert (is_aggregate_type (^^U[]));
static_assert (is_aggregate_type (^^S<int>[]));
static_assert (is_aggregate_type (^^U[1]));
static_assert (!is_aggregate_type (^^void));
constexpr bool b9 = is_aggregate_type (^^S<int>); // { dg-error "preconditions not satisfied" }
constexpr bool b10 = is_aggregate_type (^^U);	// { dg-error "preconditions not satisfied" }

// T and all types in the template parameter pack Args shall be
// complete types, cv void, or arrays of unknown bound.
constexpr bool b13 = is_constructible_type (^^S<int>, { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b14 = is_constructible_type (^^int, { ^^S<int> }); // { dg-error "preconditions not satisfied" }
constexpr bool b13a = is_constructible_type (^^S<int>[1], { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b14a = is_constructible_type (^^int, { ^^S<int>[1] }); // { dg-error "preconditions not satisfied" }
static_assert (!is_constructible_type (^^S<int>[], { ^^int }));
static_assert (!is_constructible_type (^^int, { ^^S<int>[] }));
constexpr bool b15 = is_constructible_type (^^U, { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b16 = is_constructible_type (^^int, { ^^U });  // { dg-error "preconditions not satisfied" }
constexpr bool b15a = is_constructible_type (^^U[1], { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b16a = is_constructible_type (^^int, { ^^U[1] });  // { dg-error "preconditions not satisfied" }
static_assert (!is_constructible_type (^^U[], { ^^int }));
static_assert (!is_constructible_type (^^int, { ^^U[] }));
static_assert (!is_constructible_type (^^int, { ^^void }));

// T shall be a complete type, cv void, or an array of unknown bound.
constexpr bool b17 = is_default_constructible_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
constexpr bool b18 = is_default_constructible_type (^^U);	// { dg-error "preconditions not satisfied" }
constexpr bool b19 = is_default_constructible_type (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
constexpr bool b20 = is_default_constructible_type (^^U[1]);	// { dg-error "preconditions not satisfied" }
static_assert (!is_default_constructible_type (^^S<int>[]));
static_assert (!is_default_constructible_type (^^U[]));
static_assert (!is_default_constructible_type (^^void));

constexpr bool b21 = is_copy_constructible_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
constexpr bool b22 = is_copy_constructible_type (^^U);	// { dg-error "preconditions not satisfied" }
constexpr bool b23 = is_copy_constructible_type (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
constexpr bool b24 = is_copy_constructible_type (^^U[1]);	// { dg-error "preconditions not satisfied" }
static_assert (!is_copy_constructible_type (^^S<int>[]));
static_assert (!is_copy_constructible_type (^^U[]));
static_assert (!is_copy_constructible_type (^^void));

constexpr bool b25 = is_move_constructible_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
constexpr bool b26 = is_move_constructible_type (^^U);	// { dg-error "preconditions not satisfied" }
constexpr bool b27 = is_move_constructible_type (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
constexpr bool b28 = is_move_constructible_type (^^U[1]);	// { dg-error "preconditions not satisfied" }
static_assert (!is_move_constructible_type (^^S<int>[]));
static_assert (!is_move_constructible_type (^^U[]));
static_assert (!is_move_constructible_type (^^void));

// T and U shall be complete types, cv void, or arrays of unknown bound.
constexpr bool b29 = is_assignable_type (^^S<int>, ^^int);  // { dg-error "preconditions not satisfied" }
constexpr bool b30 = is_assignable_type (^^int, ^^S<int>);  // { dg-error "preconditions not satisfied" }
static_assert (!is_assignable_type (^^S<int>[], ^^int));
static_assert (!is_assignable_type (^^int, ^^S<int>[]));
static_assert (!is_assignable_type (^^void, ^^int));
static_assert (!is_assignable_type (^^int, ^^void));

// T shall be a complete type, cv void, or an array of unknown bound.
constexpr bool b31 = is_copy_assignable_type (^^S<int>);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
constexpr bool b31u = is_copy_assignable_type (^^U);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
static_assert (!is_copy_assignable_type (^^S<int>[]));
static_assert (!is_copy_assignable_type (^^U[]));
static_assert (!is_copy_assignable_type (^^void));

constexpr bool b32 = is_move_assignable_type (^^S<int>);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
constexpr bool b32u = is_move_assignable_type (^^U);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
static_assert (!is_move_assignable_type (^^S<int>[]));
static_assert (!is_move_assignable_type (^^U[]));
static_assert (!is_move_assignable_type (^^void));

// T and U shall be complete types, cv void, or arrays of unknown bound.
constexpr bool b33 = is_swappable_with_type (^^S<int>, ^^int);	// { dg-message "required from here" }
constexpr bool b34 = is_swappable_with_type (^^int, ^^S<int>);	// { dg-message "required from here" }
constexpr bool b33u = is_swappable_with_type (^^U, ^^int);	// { dg-message "required from here" }
constexpr bool b34u = is_swappable_with_type (^^int, ^^U);	// { dg-message "required from here" }
// { dg-error "static assertion failed" "" { target *-*-* } 0 }
static_assert (!is_swappable_with_type (^^S<int>[], ^^int));
static_assert (!is_swappable_with_type (^^int, ^^S<int>[]));
static_assert (!is_swappable_with_type (^^U[], ^^int));
static_assert (!is_swappable_with_type (^^int, ^^U[]));
static_assert (!is_swappable_with_type (^^void, ^^int));
static_assert (!is_swappable_with_type (^^int, ^^void));

constexpr bool b33n = is_nothrow_swappable_with_type (^^S<int>, ^^int);	// { dg-message "required from here" }
constexpr bool b34n = is_nothrow_swappable_with_type (^^int, ^^S<int>);	// { dg-message "required from here" }
static_assert (!is_nothrow_swappable_with_type (^^S<int>[], ^^int));
static_assert (!is_nothrow_swappable_with_type (^^int, ^^S<int>[]));
static_assert (!is_nothrow_swappable_with_type (^^void, ^^int));
static_assert (!is_nothrow_swappable_with_type (^^int, ^^void));

struct W;
constexpr bool b35 = is_swappable_type (^^W);	// { dg-message "required from here" }
static_assert (!is_swappable_type (^^S<int>[]));
static_assert (!is_swappable_type (^^void));
constexpr bool b35n = is_nothrow_swappable_type (^^W);	// { dg-message "required from here" }
static_assert (!is_nothrow_swappable_type (^^S<int>[]));
static_assert (!is_nothrow_swappable_type (^^void));
// { dg-error "invalid use of incomplete type .struct W." "" { target *-*-* } 0 }

//T shall be a complete type, cv void, or an array of unknown bound.
constexpr bool b36 = is_destructible_type (^^S<int>); // { dg-error "preconditions not satisfied" }
constexpr bool b36u = is_destructible_type (^^U); // { dg-error "preconditions not satisfied" }
static_assert (!is_destructible_type (^^S<int>[]));
static_assert (!is_destructible_type (^^void));

constexpr bool b36n = is_nothrow_destructible_type (^^S<int>); // { dg-error "preconditions not satisfied" }
constexpr bool b36nu = is_nothrow_destructible_type (^^U); // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_destructible_type (^^S<int>[]));
static_assert (!is_nothrow_destructible_type (^^U[]));
static_assert (!is_nothrow_destructible_type (^^void));

// T and all types in the template parameter pack Args shall be complete
// types, cv void, or arrays of unknown bound.
constexpr bool b37 = is_trivially_constructible_type (^^S<int>, {});  // { dg-error "preconditions not satisfied" }
constexpr bool b38 = is_trivially_constructible_type (^^int, { ^^S<int> });  // { dg-error "preconditions not satisfied" }
static_assert (!is_trivially_constructible_type (^^S<int>[], {}));
static_assert (!is_trivially_constructible_type (^^int, { ^^S<int>[] }));
static_assert (!is_trivially_constructible_type (^^void, {}));
static_assert (!is_trivially_constructible_type (^^int, { ^^void }));

// T shall be a complete type, cv void, or an array of unknown bound.
constexpr bool b39 = is_trivially_default_constructible_type (^^S<int>);  // { dg-error "preconditions not satisfied" }
constexpr bool b39u = is_trivially_default_constructible_type (^^U);  // { dg-error "preconditions not satisfied" }
static_assert (!is_trivially_default_constructible_type (^^S<int>[]));
static_assert (!is_trivially_default_constructible_type (^^U[]));
static_assert (!is_trivially_default_constructible_type (^^void));

constexpr bool b40 = is_trivially_copy_constructible_type (^^S<int>);  // { dg-error "preconditions not satisfied" }
constexpr bool b40u = is_trivially_copy_constructible_type (^^U);  // { dg-error "preconditions not satisfied" }
static_assert (!is_trivially_copy_constructible_type (^^S<int>[]));
static_assert (!is_trivially_copy_constructible_type (^^U[]));
static_assert (!is_trivially_copy_constructible_type (^^void));

constexpr bool b41 = is_trivially_move_constructible_type (^^S<int>);  // { dg-error "preconditions not satisfied" }
constexpr bool b41u = is_trivially_move_constructible_type (^^U);  // { dg-error "preconditions not satisfied" }
static_assert (!is_trivially_move_constructible_type (^^S<int>[]));
static_assert (!is_trivially_move_constructible_type (^^U[]));
static_assert (!is_trivially_move_constructible_type (^^void));

// T and U shall be complete types, cv void, or arrays of unknown bound.
constexpr bool b42 = is_trivially_assignable_type (^^S<int>, ^^int);  // { dg-error "preconditions not satisfied" }
constexpr bool b43 = is_trivially_assignable_type (^^int, ^^S<int>);  // { dg-error "preconditions not satisfied" }
static_assert (!is_trivially_assignable_type (^^S<int>[], ^^int));
static_assert (!is_trivially_assignable_type (^^int, ^^S<int>[]));
static_assert (!is_trivially_assignable_type (^^void, ^^int));
static_assert (!is_trivially_assignable_type (^^int, ^^void));

// T shall be a complete type, cv void, or an array of unknown bound.
constexpr bool b44 = is_trivially_copy_assignable_type (^^S<int>);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
constexpr bool b44u = is_trivially_copy_assignable_type (^^U);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
static_assert (!is_trivially_copy_assignable_type (^^S<int>[]));
static_assert (!is_trivially_copy_assignable_type (^^U[]));
static_assert (!is_trivially_copy_assignable_type (^^void));

constexpr bool b45 = is_trivially_move_assignable_type (^^S<int>);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
constexpr bool b45u = is_trivially_move_assignable_type (^^U);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
static_assert (!is_trivially_move_assignable_type (^^S<int>[]));
static_assert (!is_trivially_move_assignable_type (^^U[]));
static_assert (!is_trivially_move_assignable_type (^^void));

constexpr bool b46 = is_trivially_destructible_type (^^S<int>);	// { dg-error "preconditions not satisfied" }
constexpr bool b46u = is_trivially_destructible_type (^^U);	// { dg-error "preconditions not satisfied" }
static_assert (!is_trivially_destructible_type (^^S<int>[]));
static_assert (!is_trivially_destructible_type (^^U[]));
static_assert (!is_trivially_destructible_type (^^void));

// T and all types in the template parameter pack Args shall be complete
// types, cv void, or arrays of unknown bound.
constexpr bool b47 = is_nothrow_constructible_type (^^S<int>, { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b48 = is_nothrow_constructible_type (^^int, { ^^S<int> }); // { dg-error "preconditions not satisfied" }
constexpr bool b49 = is_nothrow_constructible_type (^^S<int>[1], { ^^int }); // { dg-error "preconditions not satisfied" }
constexpr bool b50 = is_nothrow_constructible_type (^^int, { ^^S<int>[1] }); // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_constructible_type (^^S<int>[], { ^^int }));
static_assert (!is_nothrow_constructible_type (^^int, { ^^S<int>[] }));
constexpr bool b51 = is_nothrow_constructible_type (^^U, { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b52 = is_nothrow_constructible_type (^^int, { ^^U });  // { dg-error "preconditions not satisfied" }
constexpr bool b53 = is_nothrow_constructible_type (^^U[1], { ^^int });  // { dg-error "preconditions not satisfied" }
constexpr bool b54 = is_nothrow_constructible_type (^^int, { ^^U[1] });  // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_constructible_type (^^U[], { ^^int }));
static_assert (!is_nothrow_constructible_type (^^int, { ^^U[] }));
static_assert (!is_nothrow_constructible_type (^^int, { ^^void }));

// T shall be a complete type, cv void, or an array of unknown bound.
constexpr bool b55 = is_nothrow_default_constructible_type (^^S<int>); // { dg-error "preconditions not satisfied" }
constexpr bool b56 = is_nothrow_default_constructible_type (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_default_constructible_type (^^S<int>[]));
constexpr bool b57 = is_nothrow_default_constructible_type (^^U);  // { dg-error "preconditions not satisfied" }
constexpr bool b58 = is_nothrow_default_constructible_type (^^U[1]);  // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_default_constructible_type (^^U[]));
static_assert (!is_nothrow_default_constructible_type (^^void));

constexpr bool b59 = is_nothrow_copy_constructible_type (^^S<int>); // { dg-error "preconditions not satisfied" }
constexpr bool b60 = is_nothrow_copy_constructible_type (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_copy_constructible_type (^^S<int>[]));
constexpr bool b61 = is_nothrow_copy_constructible_type (^^U);  // { dg-error "preconditions not satisfied" }
constexpr bool b62 = is_nothrow_copy_constructible_type (^^U[1]);  // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_copy_constructible_type (^^U[]));
static_assert (!is_nothrow_copy_constructible_type (^^void));

constexpr bool b63 = is_nothrow_move_constructible_type (^^S<int>); // { dg-error "preconditions not satisfied" }
constexpr bool b64 = is_nothrow_move_constructible_type (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_move_constructible_type (^^S<int>[]));
constexpr bool b65 = is_nothrow_move_constructible_type (^^U);  // { dg-error "preconditions not satisfied" }
constexpr bool b66 = is_nothrow_move_constructible_type (^^U[1]);  // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_move_constructible_type (^^U[]));
static_assert (!is_nothrow_move_constructible_type (^^void));

constexpr bool b67 = is_nothrow_assignable_type (^^S<int>, ^^int);  // { dg-error "preconditions not satisfied" }
constexpr bool b68 = is_nothrow_assignable_type (^^int, ^^S<int>);  // { dg-error "preconditions not satisfied" }
static_assert (!is_nothrow_assignable_type (^^S<int>[], ^^int));
static_assert (!is_nothrow_assignable_type (^^int, ^^S<int>[]));
static_assert (!is_nothrow_assignable_type (^^void, ^^int));
static_assert (!is_nothrow_assignable_type (^^int, ^^void));

constexpr bool b69 = is_nothrow_copy_assignable_type (^^S<int>);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
constexpr bool b70 = is_nothrow_copy_assignable_type (^^U);	  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
static_assert (!is_nothrow_copy_assignable_type (^^S<int>[]));
static_assert (!is_nothrow_copy_assignable_type (^^void));

constexpr bool b71 = is_nothrow_move_assignable_type (^^S<int>);  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
constexpr bool b72 = is_nothrow_move_assignable_type (^^U);	  // { dg-error "preconditions not satisfied" "PR126073" { xfail *-*-* } }
static_assert (!is_nothrow_move_assignable_type (^^S<int>[]));
static_assert (!is_nothrow_move_assignable_type (^^void));

// T shall be an array type, a complete type, or cv void.
constexpr bool b73 = is_implicit_lifetime_type (^^S<int>);  // { dg-error "preconditions not satisfied" }
constexpr bool b74 = is_implicit_lifetime_type (^^U);  // { dg-error "preconditions not satisfied" }
static_assert (!is_implicit_lifetime_type (^^S<int>[1]));
static_assert (!is_implicit_lifetime_type (^^void));

// If T is a non-union class type, T shall be a complete type.
constexpr bool b75 = has_virtual_destructor (^^S<int>);	// { dg-error "preconditions not satisfied" }
static_assert (!has_virtual_destructor (^^S<int>[1]));
static_assert (!has_virtual_destructor (^^U));
static_assert (!has_virtual_destructor (^^U[1]));
static_assert (!has_virtual_destructor (^^void));

// remove_all_extents_t<T> shall be a complete type or cv void.
constexpr bool b76 = has_unique_object_representations (^^S<int>);    // { dg-error "preconditions not satisfied" }
constexpr bool b77 = has_unique_object_representations (^^S<int>[1]); // { dg-error "preconditions not satisfied" }
constexpr bool b78 = has_unique_object_representations (^^U);    // { dg-error "preconditions not satisfied" }
constexpr bool b79 = has_unique_object_representations (^^U[1]); // { dg-error "preconditions not satisfied" }
static_assert (!has_unique_object_representations (^^void));

// T and U shall be complete types, cv void, or arrays of unknown bound.
constexpr bool b80 = reference_constructs_from_temporary (^^S<int>, ^^int);  // { dg-error "preconditions not satisfied" }
constexpr bool b81 = reference_constructs_from_temporary (^^int, ^^S<int>);  // { dg-error "preconditions not satisfied" }
static_assert (!reference_constructs_from_temporary (^^S<int>[], ^^int));
static_assert (!reference_constructs_from_temporary (^^int, ^^S<int>[]));
static_assert (!reference_constructs_from_temporary (^^void, ^^int));
static_assert (!reference_constructs_from_temporary (^^int, ^^void));

constexpr bool b82 = reference_converts_from_temporary (^^S<int>, ^^int);  // { dg-error "preconditions not satisfied" }
constexpr bool b83 = reference_converts_from_temporary (^^int, ^^S<int>);  // { dg-error "preconditions not satisfied" }
static_assert (!reference_converts_from_temporary (^^S<int>[], ^^int));
static_assert (!reference_converts_from_temporary (^^int, ^^S<int>[]));
static_assert (!reference_converts_from_temporary (^^void, ^^int));
static_assert (!reference_converts_from_temporary (^^int, ^^void));
