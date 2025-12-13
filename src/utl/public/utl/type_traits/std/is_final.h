// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_final)
template <typename T>
inline constexpr bool is_final_v = __is_final(T);
template <typename T>
struct is_final : bool_constant<__is_final(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(is_final)
template <typename T>
inline constexpr bool is_final_v = unsupported_trait_v<T>;
template <typename T>
struct is_final : unsupported_trait<T> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(is_final)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
