// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_pointer)
template <typename T>
inline constexpr bool is_pointer_v = __is_pointer(T);
template <typename T>
struct is_pointer : bool_constant<__is_pointer(T)> {};
#else  // __UTL_SHOULD_USE_BUILTIN(is_pointer)
template <typename T>
inline constexpr bool is_pointer_v = false;
template <typename T>
inline constexpr bool is_pointer_v<T*> = true;
template <typename T>
inline constexpr bool is_pointer_v<T* const> = true;
template <typename T>
inline constexpr bool is_pointer_v<T* volatile> = true;
template <typename T>
inline constexpr bool is_pointer_v<T* const volatile> = true;
template <typename T>
struct is_pointer : bool_constant<is_pointer_v<T>> {};
#endif // __UTL_SHOULD_USE_BUILTIN(is_pointer)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
