// PR c++/125901
// { dg-do compile { target c++26 } }
// { dg-additional-options "-freflection" }

#include <meta>

template<typename T>
struct S {
  T t;
};

static_assert(is_trivially_copyable_type(^^S<int>));
static_assert(!is_trivially_copyable_type(^^std::vector<int>));
static_assert(is_standard_layout_type(^^std::vector<int>));
static_assert(is_default_constructible_type(^^std::vector<int>));
static_assert(!is_trivially_default_constructible_type(^^std::vector<int>));
static_assert(!is_trivially_copy_constructible_type(^^std::vector<int>));
static_assert(!is_trivially_move_constructible_type(^^std::vector<int>));
static_assert(is_copy_assignable_type(^^std::vector<int>));
static_assert(!is_implicit_lifetime_type(^^std::vector<int>));
static_assert(!has_virtual_destructor(^^std::vector<int>));
static_assert(!has_unique_object_representations(^^std::vector<int>));

const std::vector<int> vec{};
const S<int> s{};

static_assert(is_trivially_copyable_type(^^S<int>));
static_assert(!is_trivially_copyable_type(^^std::vector<int>));
static_assert(is_standard_layout_type(^^std::vector<int>));
static_assert(is_default_constructible_type(^^std::vector<int>));
static_assert(!is_trivially_default_constructible_type(^^std::vector<int>));
static_assert(!is_trivially_copy_constructible_type(^^std::vector<int>));
static_assert(!is_trivially_move_constructible_type(^^std::vector<int>));
static_assert(is_copy_assignable_type(^^std::vector<int>));
static_assert(!is_implicit_lifetime_type(^^std::vector<int>));
static_assert(!has_virtual_destructor(^^std::vector<int>));
static_assert(!has_unique_object_representations(^^std::vector<int>));
