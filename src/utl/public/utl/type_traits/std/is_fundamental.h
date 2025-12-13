// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/is_arithmetic.h"
#include "utl/type_traits/is_null_pointer.h"
#include "utl/type_traits/is_void.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_fundamental)
template <typename T>
inline constexpr bool is_fundamental_v = __is_fundamental(T);
template <typename T>
struct is_fundamental : bool_constant<__is_fundamental(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_fundamental)

template <typename T>
inline constexpr bool is_fundamental_v =
    is_void_v<T> || is_null_pointer_v<T> || is_arithmetic_v<T>;

template <typename T>
struct is_fundamental : bool_constant<is_fundamental_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_fundamental)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
