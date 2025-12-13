// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

#include "utl/type_traits/is_fundamental.h"

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_compound)
template <typename T>
inline constexpr bool is_compound_v = __is_compound(T);
template <typename T>
struct is_compound : bool_constant<__is_compound(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_compound)
template <typename T>
inline constexpr bool is_compound_v = !is_fundamental_v<T>;

template <typename T>
struct is_compound : bool_constant<is_compound_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_compound)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
