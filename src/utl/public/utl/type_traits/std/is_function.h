// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/std/is_const.h"
#include "utl/type_traits/std/is_reference.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_function)
template <typename T>
inline constexpr bool is_function_v = __is_function(T);
template <typename T>
struct is_function : bool_constant<__is_function(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_function)
template <typename T>
inline constexpr bool is_function_v =
    !is_const_v<T const> && !is_reference_v<T>;

template <typename T>
struct is_function : bool_constant<is_function_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_function)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
