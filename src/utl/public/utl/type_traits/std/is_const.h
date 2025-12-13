// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_const)
template <typename T>
inline constexpr bool is_const_v = __is_const(T);
template <typename T>
struct is_const : bool_constant<__is_const(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_const)
template <typename T>
inline constexpr bool is_const_v = false;
template <typename T>
inline constexpr bool is_const_v<T const> = true;
template <typename T>
struct is_const : bool_constant<is_const_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_const)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
