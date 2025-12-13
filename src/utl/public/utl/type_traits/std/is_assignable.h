// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#  include "utl/type_traits/std/add_lvalue_reference.h"
#  include "utl/type_traits/std/add_rvalue_reference.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_assignable)
template <typename T, typename R>
inline constexpr bool is_assignable_v = __is_assignable(T, R);
template <typename T>
inline constexpr bool is_copy_assignable_v =
    __is_assignable(T, add_lvalue_reference_t<T const>);
template <typename T>
inline constexpr bool is_move_assignable_v =
    __is_assignable(T, add_rvalue_reference_t<T>);

template <typename T, typename... Args>
struct is_assignable : bool_constant<__is_assignable(T, Args...)> {};
template <typename T>
struct is_copy_assignable :
    bool_constant<__is_assignable(T, add_lvalue_reference_t<T const>)> {};
template <typename T>
struct is_move_assignable :
    bool_constant<__is_assignable(T, add_rvalue_reference_t<T>)> {};

#else  // if __UTL_SHOULD_USE_BUILTIN(is_assignable)
template <typename T, typename R>
inline constexpr bool is_assignable_v =
    requires(R&& arg) { T(static_cast<R &&>(arg)); };

template <typename T>
inline constexpr bool is_copy_assignable_v =
    is_assignable_v<T, add_lvalue_reference_t<T const>>;
template <typename T>
inline constexpr bool is_move_assignable_v =
    is_assignable_v<T, add_rvalue_reference_t<T>>;

template <typename T, typename R>
struct is_assignable : bool_constant<is_assignable_v<T, R>> {};
template <typename T>
struct is_copy_assignable : bool_constant<is_copy_assignable_v<T>> {};
template <typename T>
struct is_move_assignable : bool_constant<is_move_assignable_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_assignable)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
