// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/type_traits/utl_common.h"

#if UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include <type_traits>

UTL_NAMESPACE_BEGIN

using std::is_layout_compatible;
using std::is_layout_compatible_v;

UTL_NAMESPACE_END

#  define UTL_TRAIT_SUPPORTED_is_layout_compatible 1

#else // ifdef UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#  include "utl/type_traits/utl_constants.h"

#  if __UTL_SHOULD_USE_BUILTIN(is_layout_compatible)
#    define UTL_BUILTIN_is_layout_compatible(...) __is_layout_compatible(__VA_ARGS__)
#  endif // __UTL_SHOULD_USE_BUILTIN(is_layout_compatible)

#  ifdef UTL_BUILTIN_is_layout_compatible

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_layout_compatible :
    bool_constant<UTL_BUILTIN_is_layout_compatible(T, U)> {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_layout_compatible_v = UTL_BUILTIN_is_layout_compatible(T, U);
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#    define UTL_TRAIT_SUPPORTED_is_layout_compatible 1

#  else // ifdef UTL_BUILTIN_is_layout_compatible

#    include "utl/type_traits/utl_undefined_trait.h"

UTL_NAMESPACE_BEGIN

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE is_layout_compatible : undefined_trait<T, U> {};
template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_layout_compatible<T, T> : true_type {};

#    if UTL_CXX14
template <typename T, typename U>
UTL_INLINE_CXX17 constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value;
#    endif // UTL_CXX14

UTL_NAMESPACE_END

#  endif // ifdef UTL_BUILTIN_is_layout_compatible

#endif // ifdef UTL_USE_STD_TYPE_TRAITS && UTL_CXX20

#if UTL_CXX14
#  define UTL_TRAIT_is_layout_compatible(...) __UTL is_layout_compatible_v<__VA_ARGS__>
#else
#  define UTL_TRAIT_is_layout_compatible(...) __UTL is_layout_compatible<__VA_ARGS__>::value
#endif
