// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_class)
template <typename T>
inline constexpr bool is_class_v = __is_class(T);
template <typename T>
struct is_class : bool_constant<__is_class(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_class)
template <typename T>
inline constexpr bool is_class_v = unsupported_trait_v<T>;
template <typename T>
struct is_class : unsupported_trait<T> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_class)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
