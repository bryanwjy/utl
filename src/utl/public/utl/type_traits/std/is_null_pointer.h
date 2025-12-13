// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_null_pointer)
template <typename T>
inline constexpr bool is_null_pointer_v = __is_null_pointer(T);
template <typename T>
struct is_null_pointer : bool_constant<__is_null_pointer(T)> {};
#else  // __UTL_SHOULD_USE_BUILTIN(is_null_pointer)
template <typename T>
inline constexpr bool is_null_pointer_v = false;
template <>
inline constexpr bool is_null_pointer_v<decltype(nullptr)> = true;
template <>
inline constexpr bool is_null_pointer_v<decltype(nullptr) const> = true;
template <>
inline constexpr bool is_null_pointer_v<decltype(nullptr) volatile> = true;
template <>
inline constexpr bool is_null_pointer_v<decltype(nullptr) const volatile> =
    true;
template <typename T>
struct is_null_pointer : bool_constant<is_null_pointer_v<T>> {};
#endif // __UTL_SHOULD_USE_BUILTIN(is_null_pointer)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
