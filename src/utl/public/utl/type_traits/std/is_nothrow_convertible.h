// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

#if !__UTL_SHOULD_USE_BUILTIN(is_nothrow_convertible)
#  include "utl/type_traits/std/is_void.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_nothrow_convertible)
template <typename From, typename To>
inline constexpr bool is_nothrow_convertible_v =
    __is_nothrow_convertible(From, To);
template <typename From, typename To>
struct is_nothrow_convertible :
    bool_constant<__is_nothrow_convertible(From, To)> {};
#else // if __UTL_SHOULD_USE_BUILTIN(is_nothrow_convertible)
template <typename From, typename To>
inline constexpr bool is_nothrow_convertible_v =
    is_void_v<From> && is_void_v<To> ||
    requires(void (*to)(To) noexcept, From (*from)() noexcept) {
        { to(from()) } noexcept;
    };

template <typename From, typename To>
struct is_convertible : bool_constant<is_nothrow_convertible_v<From, To>> {};

#endif // if __UTL_SHOULD_USE_BUILTIN(is_nothrow_convertible)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
