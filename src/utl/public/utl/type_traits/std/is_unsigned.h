// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#if !__UTL_SHOULD_USE_BUILTIN(is_unsigned)
#  include "utl/type_traits/is_arithmetic.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_unsigned)
template <typename T>
inline constexpr bool is_unsigned_v = __is_unsigned(T);
template <typename T>
struct is_unsigned : bool_constant<__is_unsigned(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_unsigned)

template <typename T>
inline constexpr bool is_unsigned_v = false;
template <typename T>
requires is_arithmetic_v<T>
inline constexpr bool is_unsigned_v = (static_cast<T>(0) < static_cast<T>(-1));

template <typename T>
struct is_unsigned : bool_constant<is_unsigned_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_unsigned)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
