// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_same)
template <typename T, typename U>
inline constexpr bool is_same_v = __is_same(T, U);
template <typename T, typename U>
struct is_same : bool_constant<__is_same(T, U)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_same)
template <typename, typename>
inline constexpr bool is_same_v = false;
template <typename T>
inline constexpr bool is_same_v<T, T> = true;
template <typename T>
struct is_same : bool_constant<is_same_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_same)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
