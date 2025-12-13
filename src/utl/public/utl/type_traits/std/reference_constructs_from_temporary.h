// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"

#  if !__UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary) && \
      __UTL_SHOULD_USE_BUILTIN(reference_binds_to_temporary)

#    include "utl/type_traits/is_convertible.h"
#    include "utl/type_traits/is_core_convertible.h"
#    include "utl/type_traits/remove_cvref.h"

#  endif

#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary)
template <typename T, typename R>
inline constexpr bool reference_constructs_from_temporary_v =
    __reference_constructs_from_temporary(T, R);
template <typename T, typename R>
struct reference_constructs_from_temporary :
    bool_constant<__reference_constructs_from_temporary(T, R)> {};

#elif __UTL_SHOULD_USE_BUILTIN(reference_binds_to_temporary)

template <typename, typename>
inline constexpr bool reference_constructs_from_temporary_v = false;

template <typename T, typename R>
requires requires {
    requires is_reference_v<T>;
    requires is_core_convertible_v<T, R>;
    requires !(
        is_reference_v<R> && is_same_v<remove_cvref_t<T>, remove_cvref_t<R>>);
    requires __reference_binds_to_temporary(T, R) ||
        is_convertible_v<remove_cvref_t<R>*, remove_cvref_t<T>*>;
}
inline constexpr bool reference_constructs_from_temporary_v = true;

template <typename T, typename R>
struct reference_constructs_from_temporary :
    bool_constant<reference_constructs_from_temporary_v<T, R>> {};

template <typename T, typename U>
struct __UTL_PUBLIC_TEMPLATE reference_constructs_from_temporary :
    false_type {};

#else // if __UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary)

template <typename T>
inline constexpr bool reference_constructs_from_temporary_v =
    unsupported_trait_v<T>;
template <typename T>
struct reference_constructs_from_temporary : unsupported_trait<T> {};

#endif // if __UTL_SHOULD_USE_BUILTIN(reference_constructs_from_temporary)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
