// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_reference;

#  if UTL_CXX17

using std::is_reference_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_reference_v = is_reference<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_reference)
#    define UTL_BUILTIN_is_reference(...) __is_reference(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_reference)

#  ifdef UTL_BUILTIN_is_reference

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_reference : bool_constant<UTL_BUILTIN_is_reference(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_reference_v = UTL_BUILTIN_is_reference(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_reference

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_reference : false_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_reference<T&> : true_type {};

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_reference<T&&> : true_type {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_reference_v = is_reference<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_reference

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#define UTL_TRAIT_SUPPORTED_is_reference 1

#if UTL_CXX14
#  define UTL_TRAIT_is_reference(...) __UTL is_reference_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_reference(...) __UTL is_reference<__VA_ARGS__>::value
#endif
