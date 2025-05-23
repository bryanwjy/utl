// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_floating_point;

using std::is_floating_point;

#  if UTL_CXX17
using std::is_floating_point_v;
#  elif UTL_CXX14 // if UTL_CXX17
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_floating_point_v = is_floating_point<T>::value;
#  endif          // if UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_floating_point)
#    define UTL_BUILTIN_is_floating_point(...) __is_floating_point(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_floating_point)

#  ifdef UTL_BUILTIN_is_floating_point

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_floating_point : bool_constant<UTL_BUILTIN_is_floating_point(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_floating_point_v = UTL_BUILTIN_is_floating_point(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_floating_point

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_floating_point : false_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<float> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<double> : true_type {};
template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<long double> : true_type {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<T const> : is_floating_point<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<T volatile> : is_floating_point<T> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<T const volatile> : is_floating_point<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_floating_point_v = is_floating_point<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_floating_point

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_floating_point 1

#if UTL_CXX23
#  include "utl/utlfloat.h"
UTL_NAMESPACE_BEGIN
#  ifdef UTL_SUPPORTS_FLOAT16
template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<float16> : true_type {};
#  endif // ifdef UTL_SUPPORTS_FLOAT16
#  ifdef UTL_SUPPORTS_FLOAT32
template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<float32> : true_type {};
#  endif // ifdef UTL_SUPPORTS_FLOAT32
#  ifdef UTL_SUPPORTS_FLOAT64
template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<float64> : true_type {};
#  endif // ifdef UTL_SUPPORTS_FLOAT64
#  ifdef UTL_SUPPORTS_FLOAT128
using std::float128;
template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<float128> : true_type {};
#  endif // ifdef UTL_SUPPORTS_FLOAT128
#  ifdef UTL_SUPPORTS_BFLOAT16
template <>
struct __UTL_PUBLIC_TEMPLATE is_floating_point<bfloat16> : true_type {};
#  endif // ifdef UTL_SUPPORTS_BFLOAT16
UTL_NAMESPACE_END
#endif // if UTL_CXX23

#if UTL_CXX14
#  define UTL_TRAIT_is_floating_point(...) __UTL is_floating_point_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_floating_point(...) __UTL is_floating_point<__VA_ARGS__>::value
#endif
