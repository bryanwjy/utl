// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_copy_constructible;

#  if UTL_CXX17

using std::is_copy_constructible_v;

#  elif UTL_CXX14 // UTL_CXX17

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

#  endif // UTL_CXX17

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_copy_constructible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS

#  include "utl/type_traits/utl_add_lvalue_reference.h"
#  include "utl/type_traits/utl_is_constructible.h"

#  ifdef UTL_BUILTIN_is_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_copy_constructible :
    bool_constant<UTL_BUILTIN_is_constructible(T, add_lvalue_reference_t<T const>)> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_copy_constructible_v =
    UTL_BUILTIN_is_constructible(T, add_lvalue_reference_t<T const>);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_copy_constructible 1

#  else // ifdef UTL_BUILTIN_is_constructible

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_copy_constructible :
    is_constructible<T, add_lvalue_reference_t<T const>> {};

#    if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_copy_constructible_v =
    is_constructible_v<T, add_lvalue_reference_t<T const>>;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_copy_constructible UTL_TRAIT_SUPPORTED_is_constructible

#  endif // ifdef UTL_BUILTIN_is_constructible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS

#if UTL_CXX14
#  define UTL_TRAIT_is_copy_constructible(...) __UTL is_copy_constructible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_copy_constructible(...) __UTL is_copy_constructible<__VA_ARGS__>::value
#endif
