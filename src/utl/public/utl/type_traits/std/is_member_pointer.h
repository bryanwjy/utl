// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/std/is_function.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_member_pointer)
template <typename T>
inline constexpr bool is_member_pointer_v = __is_member_pointer(T);
template <typename T>
struct is_member_pointer : bool_constant<__is_member_pointer(T)> {};
#else  // __UTL_SHOULD_USE_BUILTIN(is_member_pointer)
template <typename T>
inline constexpr bool is_member_pointer_v = false;
template <typename T>
inline constexpr bool is_member_pointer_v<T const> = is_member_pointer_v<T>;
template <typename T>
inline constexpr bool is_member_pointer_v<T volatile> = is_member_pointer_v<T>;
template <typename T>
inline constexpr bool is_member_pointer_v<T const volatile> =
    is_member_pointer_v<T>;
template <typename C, typename T>
inline constexpr bool is_member_pointer_v<T C::*> = true;

template <typename T>
struct is_member_pointer : bool_constant<is_member_pointer_v<T>> {};
#endif // __UTL_SHOULD_USE_BUILTIN(is_member_pointer)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
