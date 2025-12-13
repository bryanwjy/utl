// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(builtin_is_pointer_interconvertible_base_of)
template <typename T, typename U>
inline constexpr bool is_pointer_interconvertible_base_of_v =
    __builtin_is_pointer_interconvertible_base_of(T, U);
template <typename T, typename U>
struct is_pointer_interconvertible_base_of :
    bool_constant<__builtin_is_pointer_interconvertible_base_of(T, U)> {};
#elif __UTL_SHOULD_USE_BUILTIN(is_pointer_interconvertible_base_of)
template <typename T, typename U>
inline constexpr bool is_pointer_interconvertible_base_of_v =
    __is_pointer_interconvertible_base_of(T, U);
template <typename T, typename U>
struct is_pointer_interconvertible_base_of :
    bool_constant<__is_pointer_interconvertible_base_of(T, U)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_pointer_interconvertible_base_of)
template <typename T, typename U>
inline constexpr bool is_pointer_interconvertible_base_of_v =
    unsupported_trait_v<T, U>;
template <typename T>
inline constexpr bool is_pointer_interconvertible_base_of_v<T, T> = true;
template <typename T, typename U>
requires (!is_same_v<T &&, T> && !is_same_v<U &&, U>) &&
    requires(T&& from) { const_cast<U &&>(from); }
inline constexpr bool is_pointer_interconvertible_base_of_v<T, U> = true;

template <typename T>
struct is_pointer_interconvertible_base_of :
    bool_constant<is_pointer_interconvertible_base_of_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_pointer_interconvertible_base_of)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
