/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/concepts/core.h"
#  include "utl/type_traits/operation_support.h" // IWYU pragma: export
#  include "utl/type_traits/type_categories.h"
#  include "utl/type_traits/type_modifications.h"
#  include "utl/type_traits/type_transformations.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

template <typename T>
void as_const(T&&) noexcept = delete;

template <typename T>
UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI)
constexpr T const& as_const(T const& value UTL_LIFETIMEBOUND) noexcept {
    return value;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, ALWAYS_INLINE, _HIDE_FROM_ABI)
constexpr T const& as_const(T& value UTL_LIFETIMEBOUND) noexcept {
    return value;
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, INTRINSIC, _HIDE_FROM_ABI)
constexpr T&& forward(
    remove_reference_t<T>& value UTL_LIFETIMEBOUND) noexcept {
    return static_cast<T&&>(value);
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, INTRINSIC, _HIDE_FROM_ABI)
constexpr T&& forward(
    remove_reference_t<T>&& value UTL_LIFETIMEBOUND) noexcept {
    static_assert(
        !is_lvalue_reference_v<T>, "cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(value);
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, INTRINSIC, _HIDE_FROM_ABI)
constexpr remove_reference_t<T>&& move(
    T&& value UTL_LIFETIMEBOUND) noexcept {
    return static_cast<remove_reference_t<T>&&>(value);
}

template <typename T>
UTL_ATTRIBUTES(NODISCARD, INTRINSIC, _HIDE_FROM_ABI)
constexpr decltype(auto) move_if_noexcept(
    T&& value UTL_LIFETIMEBOUND) noexcept {
    if constexpr (is_nothrow_move_constructible_v<T> ||
        !is_copy_constructible_v<T>) {
        return static_cast<remove_reference_t<T>&&>(value);
    } else {
        return static_cast<T const&>(value);
    }
}

template <typename T, typename U = T>
requires is_move_constructible_v<T> && is_assignable_v<T&, U>
__UTL_HIDE_FROM_ABI constexpr T exchange(T& obj, U&& new_value) noexcept(
    is_nothrow_move_constructible_v<T> && is_nothrow_assignable_v<T&, U>) {
    T previous(move(obj));
    obj = forward<U>(new_value);
    return previous;
}

template <integral T>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI)
constexpr make_unsigned_t<T> to_unsigned(T val) noexcept {
    return (make_unsigned_t<T>)val;
}

template <integral T>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI)
constexpr make_signed_t<T> to_signed(T val) noexcept {
    return (make_signed_t<T>)val;
}

template <typename T>
requires is_enum_v<T>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI)
constexpr auto to_underlying(T value) noexcept {
    return static_cast<underlying_type_t<T>>(value);
}

UTL_ATTRIBUTES(NORETURN, _HIDE_FROM_ABI) inline void unreachable() {
    assert(false);
    UTL_BUILTIN_unreachable();
}

/**
 * Exported from type_traits.operation_support

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

*/

UTL_PUBLIC_DEFAULT_NAMESPACE_END
