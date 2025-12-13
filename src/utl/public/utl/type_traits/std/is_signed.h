// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#if !__UTL_SHOULD_USE_BUILTIN(is_signed)
#  include "utl/type_traits/is_arithmetic.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_signed)
template <typename T>
inline constexpr bool is_signed_v = __is_signed(T);
template <typename T>
struct is_signed : bool_constant<__is_signed(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_signed)

template <typename T>
inline constexpr bool is_signed_v = false;
template <typename T>
requires is_arithmetic_v<T>
inline constexpr bool is_signed_v = (static_cast<T>(-1) < static_cast<T>(0));

template <typename T>
struct is_signed : bool_constant<is_signed_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_signed)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
