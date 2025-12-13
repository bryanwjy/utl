// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/is_assignable.h"
#include "utl/type_traits/is_constructible.h"
#include "utl/type_traits/is_nothrow_assignable.h"
#include "utl/type_traits/is_nothrow_constructible.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN
template <typename T>
requires is_move_constructible_v<T> && is_move_assignable_v<T>
__UTL_HIDE_FROM_ABI constexpr void swap(T& left, T& right) noexcept(
    is_nothrow_move_constructible_v<T> && is_nothrow_move_assignable_v<T>) {
    T tmp(__UTL move(left));
    left = __UTL move(right);
    right = __UTL move(tmp);
}

template <typename T, size_t N>
requires is_swappable_v<T>
__UTL_HIDE_FROM_ABI constexpr void swap(T (&left)[N], T (&right)[N]) noexcept(
    is_nothrow_swappable_v<T>) {
    for (auto i = 0u; i < N; ++i) {
        swap(left[i], right[i]);
    }
}
UTL_PUBLIC_DEFAULT_NAMESPACE_END

UTL_DEFAULT_NAMESPACE_BEGIN
namespace details {
using ::utl::swap;

template <typename L, typename R>
concept swap_callable_with = requires(L&& left, R&& right) {
    swap(static_cast<L &&>(left), static_cast<R &&>(right));
    swap(static_cast<R &&>(right), static_cast<L &&>(left));
};

template <typename L, typename R>
concept nothrow_swap_callable_with =
    swap_callable_with<L, R> && requires(L&& left, R&& right) {
        { swap(static_cast<L &&>(left), static_cast<R &&>(right)) } noexcept;
        { swap(static_cast<R &&>(right), static_cast<L &&>(left)) } noexcept;
    };
} // namespace details

UTL_DEFAULT_NAMESPACE_END

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

template <typename L, typename R>
inline constexpr bool is_swappable_with_v = details::swap_callable_with<L, R>;
template <typename L, typename R>
inline constexpr bool is_nothrow_swappable_with_v =
    details::nothrow_swap_callable_with<L, R>;

template <typename L, typename R>
struct is_swappable_with : bool_constant<details::swap_callable_with<L, R>> {};
template <typename L, typename R>
struct is_nothrow_swappable_with :
    bool_constant<details::nothrow_swap_callable_with<L, R>> {};

template <typename T>
inline constexpr bool is_swappable_v = details::swap_callable_with<T&, T&>;
template <typename T>
inline constexpr bool is_nothrow_swappable_v =
    details::nothrow_swap_callable_with<T&, T&>;

template <typename T>
struct is_swappable : bool_constant<details::swap_callable_with<T&, T&>> {};
template <typename T>
struct is_nothrow_swappable :
    bool_constant<details::nothrow_swap_callable_with<T&, T&>> {};

UTL_PUBLIC_DEFAULT_NAMESPACE_END
