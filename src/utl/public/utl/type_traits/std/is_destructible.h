// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_destructible)
template <typename T>
inline constexpr bool is_destructible_v = __is_destructible(T);
template <typename T>
struct is_destructible : bool_constant<__is_destructible(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_destructible)
template <typename T>
inline constexpr bool is_destructible_v = false;
template <typename T>
requires requires(T* ptr) { ptr->~T(); }
inline constexpr bool is_destructible_v = true;
template <typename T>
inline constexpr bool is_destructible_v<T const> = is_destructible_v<T>;
template <typename T>
inline constexpr bool is_destructible_v<T volatile> = is_destructible_v<T>;
template <typename T>
inline constexpr bool is_destructible_v<T const volatile> =
    is_destructible_v<T>;

template <typename T, typename... Args>
struct is_destructible : bool_constant<is_destructible_v<T, Args...>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_destructible)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
