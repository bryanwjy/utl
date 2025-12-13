// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(array_rank)
template <typename T>
inline constexpr size_t rank_v = __array_rank(T);
template <typename T>
struct rank : size_constant<__array_rank(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(array_rank)
template <typename T>
inline constexpr size_t rank_v = 0;
template <typename T>
inline constexpr size_t rank_v<T[]> = 1 + rank_v<T>;
template <typename T, size_t N>
inline constexpr size_t rank_v<T[N]> = 1 + rank_v<T>;
template <typename T>
struct rank : size_constant<rank_v<T>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(array_rank)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
