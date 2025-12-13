// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_floating_point)
template <typename T>
inline constexpr bool is_floating_point_v = __is_floating_point(T);
template <typename T>
struct is_floating_point : bool_constant<__is_floating_point(T)> {};
#else
template <typename T>
inline constexpr bool is_floating_point_v = false;
template <typename T>
inline constexpr bool is_floating_point_v<T const> = is_floating_point_v<T>;
template <typename T>
inline constexpr bool is_floating_point_v<T volatile> = is_floating_point_v<T>;
template <typename T>
inline constexpr bool is_floating_point_v<T const volatile> =
    is_floating_point_v<T>;
template <>
inline constexpr bool is_floating_point_v<float> = true;
template <>
inline constexpr bool is_floating_point_v<double> = true;
template <>
inline constexpr bool is_floating_point_v<long double> = true;

#  if UTL_SUPPORTS_FLOAT16
template <>
inline constexpr bool is_floating_point_v<decltype(0.0f16)> = true;
#  endif // if UTL_SUPPORTS_FLOAT16
#  if UTL_SUPPORTS_FLOAT32
template <>
inline constexpr bool is_floating_point_v<decltype(0.0f32)> = true;
#  endif // if UTL_SUPPORTS_FLOAT32
#  if UTL_SUPPORTS_FLOAT64
template <>
inline constexpr bool is_floating_point_v<decltype(0.0f64)> = true;
#  endif // if UTL_SUPPORTS_FLOAT64
#  if UTL_SUPPORTS_FLOAT128
using std::float128;
template <>
inline constexpr bool is_floating_point_v<decltype(0.0f128)> = true;
#  endif // if UTL_SUPPORTS_FLOAT128
#  if UTL_SUPPORTS_BFLOAT16
template <>
inline constexpr bool is_floating_point_v<decltype(0.0bf16)> = true;
#  endif // if UTL_SUPPORTS_BFLOAT16
template <typename T>
struct is_floating_point : bool_constant<is_floating_point_v<T>> {};
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_END
