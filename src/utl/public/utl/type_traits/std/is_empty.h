// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_empty)
template <typename T>
inline constexpr bool is_empty_v = __is_empty(T);
template <typename T>
struct is_empty : bool_constant<__is_empty(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_empty)
template <typename T>
inline constexpr bool is_empty_v = unsupported_trait_v<T>;
template <typename T>
struct is_empty : unsupported_trait<T> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_empty)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
