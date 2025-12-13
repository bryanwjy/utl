// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_unbounded_array)
template <typename T>
inline constexpr bool is_unbounded_array_v = __is_unbounded_array(T);
template <typename T>
struct is_unbounded_array : bool_constant<__is_unbounded_array(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_unbounded_array)
template <typename T>
inline constexpr bool is_unbounded_array_v = false;
template <typename T, size_t N>
inline constexpr bool is_unbounded_array_v<T[]> = true;
template <typename T>
struct is_unbounded_array : bool_constant<is_unbounded_array_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_unbounded_array)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
