// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_aggregate)
template <typename T>
inline constexpr bool is_aggregate_v = __is_aggregate(T);
template <typename T>
struct is_aggregate : bool_constant<__is_aggregate(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_aggregate)
template <typename T>
inline constexpr bool is_aggregate_v = unsupported_trait_v<T>;
template <typename T>
struct is_aggregate : unsupported_trait<T> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_aggregate)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
