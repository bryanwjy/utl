// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#  include "utl/type_traits/std/add_lvalue_reference.h"
#  include "utl/type_traits/std/add_rvalue_reference.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_nothrow_constructible)
template <typename T, typename... Args>
inline constexpr bool is_nothrow_constructible_v =
    __is_nothrow_constructible(T, Args...);
template <typename T>
inline constexpr bool is_nothrow_default_constructible_v =
    __is_nothrow_constructible(T);
template <typename T>
inline constexpr bool is_nothrow_copy_constructible_v =
    __is_nothrow_constructible(T, add_lvalue_reference_t<T const>);
template <typename T>
inline constexpr bool is_nothrow_move_constructible_v =
    __is_nothrow_constructible(T, add_rvalue_reference_t<T>);

template <typename T, typename... Args>
struct is_nothrow_constructible :
    bool_constant<__is_nothrow_constructible(T, Args...)> {};
template <typename T>
struct is_nothrow_default_constructible :
    bool_constant<__is_nothrow_constructible(T)> {};
template <typename T>
struct is_nothrow_copy_constructible :
    bool_constant<__is_nothrow_constructible(
        T, add_lvalue_reference_t<T const>)> {};
template <typename T>
struct is_nothrow_move_constructible :
    bool_constant<__is_nothrow_constructible(T, add_rvalue_reference_t<T>)> {};

#else  // if __UTL_SHOULD_USE_BUILTIN(is_nothrow_constructible)
template <typename T, typename... Args>
inline constexpr bool is_nothrow_constructible_v = requires(Args&&... args) {
    { T(static_cast<Args &&>(args)...) } noexcept;
};

template <typename T>
inline constexpr bool is_nothrow_default_constructible_v =
    is_nothrow_constructible_v<T>;

template <typename T>
inline constexpr bool is_nothrow_copy_constructible_v =
    is_nothrow_constructible_v<T, add_lvalue_reference_t<T const>>;
template <typename T>
inline constexpr bool is_nothrow_move_constructible_v =
    is_nothrow_constructible_v<T, add_rvalue_reference_t<T>>;

template <typename T, typename... Args>
struct is_nothrow_constructible :
    bool_constant<is_nothrow_constructible_v<T, Args...>> {};
template <typename T>
struct is_nothrow_default_constructible :
    bool_constant<is_nothrow_default_constructible_v<T>> {};
template <typename T>
struct is_nothrow_copy_constructible :
    bool_constant<is_nothrow_copy_constructible_v<T>> {};
template <typename T>
struct is_nothrow_move_constructible :
    bool_constant<is_nothrow_move_constructible_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_nothrow_constructible)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
