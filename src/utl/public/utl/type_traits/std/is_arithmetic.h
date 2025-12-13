// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/is_floating_point.h"
#include "utl/type_traits/is_intergral.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_arithmetic)
template <typename T>
inline constexpr bool is_arithmetic_v = __is_arithmetic(T);
template <typename T>
struct is_arithmetic : bool_constant<__is_arithmetic(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_arithmetic)
template <typename T>
inline constexpr bool is_arithmetic_v =
    is_integral_v<T> || is_floating_point_v<T>;

template <typename T>
struct is_arithmetic : bool_constant<is_arithmetic_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_arithmetic)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
