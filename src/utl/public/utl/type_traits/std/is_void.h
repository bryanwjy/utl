// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_void)
template <typename T>
inline constexpr bool is_void_v = __is_void(T);
template <typename T>
struct is_void : bool_constant<__is_void(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_void)
template <typename T>
inline constexpr bool is_void_v = false;
template <>
inline constexpr bool is_void_v<void> = true;
template <>
inline constexpr bool is_void_v<void const> = true;
template <>
inline constexpr bool is_void_v<void volatile> = true;
template <>
inline constexpr bool is_void_v<void const volatile> = true;
template <typename T>
struct is_void : bool_constant<is_void_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_void)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
