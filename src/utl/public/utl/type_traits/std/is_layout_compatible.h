// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(builtin_is_layout_compatible)
template <typename T, typename U>
inline constexpr bool is_layout_compatible_v =
    __builtin_is_layout_compatible(T, U);
template <typename T, typename U>
struct is_layout_compatible :
    bool_constant<__builtin_is_layout_compatible(T, U)> {};
#elif __UTL_SHOULD_USE_BUILTIN(is_layout_compatible)
template <typename T, typename U>
inline constexpr bool is_layout_compatible_v = __is_layout_compatible(T, U);
template <typename T, typename U>
struct is_layout_compatible : bool_constant<__is_layout_compatible(T, U)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_layout_compatible)
template <typename T, typename U>
inline constexpr bool is_layout_compatible_v = unsupported_trait_v<T, U>;
template <typename T>
inline constexpr bool is_layout_compatible_v<T, T> = true;
template <typename T, typename U>
requires (!is_same_v<T &&, T> && !is_same_v<U &&, U>) &&
    requires(T&& from) { const_cast<U &&>(from); }
inline constexpr bool is_layout_compatible_v<T, U> = true;

template <typename T>
struct is_layout_compatible : bool_constant<is_layout_compatible_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_layout_compatible)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
