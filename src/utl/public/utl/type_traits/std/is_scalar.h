// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/is_arithmetic.h"
#include "utl/type_traits/is_enum.h"
#include "utl/type_traits/is_member_pointer.h"
#include "utl/type_traits/is_null_pointer.h"
#include "utl/type_traits/is_pointer.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_scalar)
template <typename T>
inline constexpr bool is_scalar_v = __is_scalar(T);
template <typename T>
struct is_scalar : bool_constant<__is_scalar(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_scalar)
template <typename T>
inline constexpr bool is_scalar_v = is_arithmetic_v<T> || is_enum_v<T> ||
    is_member_pointer_v<T> || is_null_pointer_v<T> || is_pointer_v<T>;

template <typename T>
struct is_scalar : bool_constant<is_scalar_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_scalar)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
