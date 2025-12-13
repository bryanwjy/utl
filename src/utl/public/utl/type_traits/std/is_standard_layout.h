// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_standard_layout)
template <typename T>
inline constexpr bool is_standard_layout_v = __is_standard_layout(T);
template <typename T>
struct is_standard_layout : bool_constant<__is_standard_layout(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_standard_layout)
template <typename T>
inline constexpr bool is_standard_layout_v = unsupported_trait_v<T>;
template <typename T>
struct is_standard_layout : unsupported_trait<T> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_volatile)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
