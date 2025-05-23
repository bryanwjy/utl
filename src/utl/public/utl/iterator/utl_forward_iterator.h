// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_incrementable.h"
#include "utl/iterator/utl_input_iterator.h"
#include "utl/iterator/utl_iterator_concept_t.h"
#include "utl/iterator/utl_iterator_tags.h"
#include "utl/iterator/utl_sentinel_for.h"

#if UTL_CXX20

#  include "utl/concepts/utl_derived_from.h"

UTL_NAMESPACE_BEGIN
template <typename T>
concept forward_iterator = __UTL input_iterator<T> &&
    __UTL details::iterator_concept::implements<T, __UTL forward_iterator_tag> &&
    __UTL incrementable<T> && __UTL sentinel_for<T, T>;

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_forward_iterator : __UTL bool_constant<forward_iterator<T>> {};

template <typename T>
inline constexpr bool is_forward_iterator_v = forward_iterator<T>;
#  define UTL_TRAIT_is_forward_iterator(...) __UTL forward_iterator<__VA_ARGS__>

UTL_NAMESPACE_END

#else // UTL_CXX20

#  include "utl/type_traits/utl_is_base_of.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_forward_iterator :
    __UTL conjunction<__UTL is_input_iterator<T>,
        __UTL details::iterator_concept::implements<__UTL forward_iterator_tag, T>,
        __UTL is_incrementable<T>, __UTL is_sentinel_for<T, T>> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;
#    define UTL_TRAIT_is_forward_iterator(...) __UTL is_forward_iterator_v<__VA_ARGS__>
#  else
#    define UTL_TRAIT_is_forward_iterator(...) __UTL is_forward_iterator<__VA_ARGS__>::value
#  endif

UTL_NAMESPACE_END

#endif // UTL_CXX20
