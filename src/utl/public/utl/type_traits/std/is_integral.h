// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_integral)
template <typename T>
inline constexpr bool is_integral_v = __is_integral(T);
template <typename T>
struct is_integral : bool_constant<__is_integral(T)> {};
#else // if __UTL_SHOULD_USE_BUILTIN(is_integral)
template <typename T>
inline constexpr bool is_integral_v = false;
template <typename T>
inline constexpr bool is_integral_v<T const> = is_integral_v<T>;
template <typename T>
inline constexpr bool is_integral_v<T volatile> = is_integral_v<T>;
template <typename T>
inline constexpr bool is_integral_v<T const volatile> = is_integral_v<T>;
template <>
inline constexpr bool is_integral_v<bool> = true;
template <>
inline constexpr bool is_integral_v<char> = true;
template <>
inline constexpr bool is_integral_v<signed char> = true;
template <>
inline constexpr bool is_integral_v<unsigned char> = true;
template <>
inline constexpr bool is_integral_v<wchar_t> = true;
template <>
inline constexpr bool is_integral_v<char16_t> = true;
template <>
inline constexpr bool is_integral_v<char32_t> = true;
template <>
inline constexpr bool is_integral_v<short> = true;
template <>
inline constexpr bool is_integral_v<unsigned short> = true;
template <>
inline constexpr bool is_integral_v<int> = true;
template <>
inline constexpr bool is_integral_v<unsigned int> = true;
template <>
inline constexpr bool is_integral_v<long> = true;
template <>
inline constexpr bool is_integral_v<unsigned long> = true;
template <>
inline constexpr bool is_integral_v<long long> = true;
template <>
inline constexpr bool is_integral_v<unsigned long long> = true;
#  if UTL_SUPPORTS_CHAR8_T
template <>
inline constexpr bool is_integral_v<char8_t> = true;
#  endif // if UTL_SUPPORTS_CHAR8_T
#  if UTL_SUPPORTS_INT128
template <>
inline constexpr bool is_integral_v<__int128_t> = true;
template <>
inline constexpr bool is_integral_v<__uint128_t> = true;
#  endif // if UTL_SUPPORTS_INT128
template <typename T>
struct is_integral : bool_constant<is_integral_v<T>> {};
#endif   // if __UTL_SHOULD_USE_BUILTIN(is_integral)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
