// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_reference)
template <typename T>
inline constexpr bool is_reference_v = __is_reference(T);
template <typename T>
struct is_reference : bool_constant<__is_reference(T)> {};
#else  // __UTL_SHOULD_USE_BUILTIN(is_reference)
template <typename T>
inline constexpr bool is_reference_v = false;
template <typename T>
inline constexpr bool is_reference_v<T&> = true;
template <typename T>
inline constexpr bool is_reference_v<T&&> = true;
template <typename T>
struct is_reference : bool_constant<is_reference_v<T>> {};
#endif // __UTL_SHOULD_USE_BUILTIN(is_reference)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
