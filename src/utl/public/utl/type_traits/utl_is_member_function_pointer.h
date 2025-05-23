// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_member_function_pointer;

#  if UTL_CXX17

using std::is_member_function_pointer_v;

#  elif UTL_CXX14 // if UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

#  endif // if UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_member_function_pointer 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_member_function_pointer)
#    define UTL_BUILTIN_is_member_function_pointer(...) __is_member_function_pointer(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_member_function_pointer)

#  ifdef UTL_BUILTIN_is_member_function_pointer

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_member_function_pointer :
    bool_constant<UTL_BUILTIN_is_member_function_pointer(T)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_function_pointer_v = UTL_BUILTIN_is_member_function_pointer(T);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  else // ifdef UTL_BUILTIN_is_member_function_pointer

#    include "utl/type_traits/utl_is_function.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_member_function_pointer : false_type {};

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_member_function_pointer<T U::*> : is_function<T> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_member_function_pointer

#  define UTL_TRAIT_SUPPORTED_is_member_function_pointer 1

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_member_function_pointer(...) __UTL is_member_function_pointer_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_member_function_pointer(...) \
      __UTL is_member_function_pointer<__VA_ARGS__>::value
#endif
