// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_assignable_from.h"
#include "utl/concepts/utl_copy_constructible.h"
#include "utl/concepts/utl_empty_type.h"
#include "utl/concepts/utl_equality_comparable.h"
#include "utl/concepts/utl_move_constructible.h"
#include "utl/concepts/utl_swappable.h"
#include "utl/type_traits/utl_is_swappable.h"
#include "utl/utility/utl_move.h"

UTL_DISABLE_WARNING_PUSH()
#if UTL_COMPILER_CLANG_BASED
UTL_DISABLE_WARNING("-Wlogical-op-parentheses")
#endif

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

namespace details {
namespace allocator {

template <typename T>
concept comparability = empty_type<T> || requires {
    typename T::is_always_equal;
    requires T::is_always_equal::value;
} || __UTL equality_comparable<T> && requires(T const& l, T const& r) {
    { l == r } noexcept;
    { l != r } noexcept;
};

template <typename T>
concept copy_propogation = requires { typename T::propagate_on_container_copy_assignment; } &&
    T::propagate_on_container_copy_assignment::value;

template <typename T>
concept move_propogation = requires { typename T::propagate_on_container_move_assignment; } &&
    T::propagate_on_container_move_assignment::value;

template <typename T>
concept swap_propogation =
    requires { typename T::propagate_on_container_swap; } && T::propagate_on_container_swap::value;

template <typename T>
concept copy_assignability =
    !copy_propogation<T> || assignable_from<T&, T const&> && requires(T& l, T const& r) {
        { l = r } noexcept;
    };

template <typename T>
concept move_assignability =
    !move_propogation<T> || assignable_from<T&, T&&> && requires(T& l, T&& r) {
        { l = __UTL move(r) } noexcept;
    };

template <typename T>
concept swappability =
    !swap_propogation<T> || __UTL swappable<T> && __UTL is_nothrow_swappable_v<T>;

} // namespace allocator
} // namespace details

template <typename T>
concept allocator_type = copy_constructible<T> && move_constructible<T> &&
    requires(T t) {
        { T(t) } noexcept;
        { T(__UTL move(t)) } noexcept;
    } && details::allocator::comparability<T> && details::allocator::copy_assignability<T> &&
    details::allocator::move_assignability<T> && details::allocator::swappability<T>;

UTL_NAMESPACE_END
#endif

UTL_DISABLE_WARNING_POP()
