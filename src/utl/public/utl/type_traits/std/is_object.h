// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/is_array.h"
#include "utl/type_traits/is_class.h"
#include "utl/type_traits/is_scalar.h"
#include "utl/type_traits/is_union.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_object)
template <typename T>
inline constexpr bool is_object_v = __is_object(T);
template <typename T>
struct is_object : bool_constant<__is_object(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_object)
template <typename T>
inline constexpr bool is_object_v =
    is_scalar_v<T> || is_array_v<T> || is_union_v<T> || is_class_v<T>;

template <typename T>
struct is_object : bool_constant<is_object_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_object)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
