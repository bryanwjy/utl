// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_nothrow_destructible)
template <typename T>
inline constexpr bool is_nothrow_destructible_v = __is_nothrow_destructible(T);
template <typename T>
struct is_nothrow_destructible : bool_constant<__is_nothrow_destructible(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_nothrow_destructible)
template <typename T>
inline constexpr bool is_nothrow_destructible_v = requires(T* ptr) {
    { ptr->~T() } noexcept;
};
template <typename T>
inline constexpr bool is_nothrow_destructible_v<T const> =
    is_nothrow_destructible_v<T>;
template <typename T>
inline constexpr bool is_nothrow_destructible_v<T volatile> =
    is_nothrow_destructible_v<T>;
template <typename T>
inline constexpr bool is_nothrow_destructible_v<T const volatile> =
    is_nothrow_destructible_v<T>;

template <typename T, typename... Args>
struct is_nothrow_destructible :
    bool_constant<is_nothrow_destructible_v<T, Args...>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_nothrow_destructible)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
