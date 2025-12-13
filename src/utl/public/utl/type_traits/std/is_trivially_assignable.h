// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/std/add_lvalue_reference.h"
#include "utl/type_traits/std/add_rvalue_reference.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_trivially_assignable)
template <typename T, typename... Args>
inline constexpr bool is_trivially_assignable_v =
    __is_trivially_assignable(T, Args...);
template <typename T>
inline constexpr bool is_trivially_copy_assignable_v =
    __is_trivially_assignable(T, add_lvalue_reference_t<T const>);
template <typename T>
inline constexpr bool is_trivially_move_assignable_v =
    __is_trivially_assignable(T, add_rvalue_reference_t<T>);

template <typename T, typename... Args>
struct is_trivially_assignable :
    bool_constant<__is_trivially_assignable(T, Args...)> {};
template <typename T>
struct is_trivially_copy_assignable :
    bool_constant<__is_trivially_assignable(
        T, add_lvalue_reference_t<T const>)> {};
template <typename T>
struct is_trivially_move_assignable :
    bool_constant<__is_trivially_assignable(T, add_rvalue_reference_t<T>)> {};

#else  // if __UTL_SHOULD_USE_BUILTIN(is_trivially_assignable)
template <typename T, typename... Args>
inline constexpr bool is_trivially_assignable_v =
    requires(Args&&... args) { T(static_cast<Args &&>(args)...); };

template <typename T>
inline constexpr bool is_trivially_copy_assignable_v =
    is_trivially_assignable_v<T, add_lvalue_reference_t<T const>>;
template <typename T>
inline constexpr bool is_trivially_move_assignable_v =
    is_trivially_assignable_v<T, add_rvalue_reference_t<T>>;

template <typename T, typename... Args>
struct is_trivially_assignable :
    bool_constant<is_trivially_assignable_v<T, Args...>> {};
template <typename T>
struct is_trivially_copy_assignable :
    bool_constant<is_trivially_copy_assignable_v<T>> {};
template <typename T>
struct is_trivially_move_assignable :
    bool_constant<is_trivially_move_assignable_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_trivially_assignable)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
