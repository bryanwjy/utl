// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_volatile)
template <typename T>
inline constexpr bool is_volatile_v = __is_volatile(T);
template <typename T>
struct is_volatile : bool_constant<__is_volatile(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_volatile)
template <typename T>
inline constexpr bool is_volatile_v = false;
template <typename T>
inline constexpr bool is_volatile_v<T volatile> = true;
template <typename T>
struct is_volatile : bool_constant<is_volatile_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_volatile)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
