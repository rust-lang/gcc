// { dg-do compile { target c++17 } }

#include <type_traits>

struct Incomplete;

static_assert(std::has_unique_object_representations_v<Incomplete>);       // { dg-error "here" }
static_assert(std::has_unique_object_representations_v<const Incomplete>); // { dg-error "here" }
static_assert(std::has_unique_object_representations_v<Incomplete[10]>);   // { dg-error "here" }
static_assert(std::has_unique_object_representations_v<Incomplete[]>);     // { dg-error "here" }

// { dg-prune-output "invalid use of incomplete type" }
// { dg-prune-output "template argument must be a complete" }
// { dg-prune-output "'value' is not a member of 'std::has_unique_object_representations" }
// { dg-prune-output "static assertion" }
