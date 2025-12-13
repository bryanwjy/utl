// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#  if !__UTL_SHOULD_USE_BUILTIN(builtin_is_implicit_lifetime) && \
      !__UTL_SHOULD_USE_BUILTIN(is_implicit_lifetime)
#    include "utl/type_traits/std/is_aggregate.h"
#    include "utl/type_traits/std/is_array.h"
#    include "utl/type_traits/std/is_copy_constructible.h"
#    include "utl/type_traits/std/is_default_constructible.h"
#    include "utl/type_traits/std/is_destructible.h"
#    include "utl/type_traits/std/is_move_constructible.h"
#    include "utl/type_traits/std/is_scalar.h"
#  endif
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(builtin_is_implicit_lifetime)
template <typename T>
inline constexpr bool is_implicit_lifetime_v =
    __builtin_is_implicit_lifetime(T);
template <typename T>
struct is_implicit_lifetime :
    bool_constant<__builtin_is_implicit_lifetime(T)> {};

#elif __UTL_SHOULD_USE_BUILTIN(is_implicit_lifetime)
template <typename T>
inline constexpr bool is_implicit_lifetime_v = __is_implicit_lifetime(T);
template <typename T>
struct is_implicit_lifetime : bool_constant<__is_implicit_lifetime(T)> {};

#else // if __UTL_SHOULD_USE_BUILTIN(is_implicit_lifetime)

template <typename T>
inline constexpr bool is_implicit_lifetime_v = is_scalar_v<T> ||
    is_array_v<T> || (is_aggregate_v<T> && is_trivially_destructible_v<T>) ||
    (is_trivially_destructible_v<T> &&
        (is_trivially_move_constructible_v<T> ||
            is_trivially_copy_constructible_v<T> ||
            is_trivially_default_constructible_v<T>));
template <typename T>
inline constexpr bool is_implicit_lifetime_v<T const> =
    is_implicit_lifetime_v<T>;
template <typename T>
inline constexpr bool is_implicit_lifetime_v<T volatile> =
    is_implicit_lifetime_v<T>;
template <typename T>
inline constexpr bool is_implicit_lifetime_v<T const volatile> =
    is_implicit_lifetime_v<T>;

template <typename T>
struct is_implicit_lifetime : bool_constant<is_implicit_lifetime_v<T>> {};

#endif // if __UTL_SHOULD_USE_BUILTIN(is_implicit_lifetime)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
