// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(reference_converts_from_temporary)
template <typename T, typename R>
inline constexpr bool reference_converts_from_temporary_v =
    __reference_converts_from_temporary(T, R);
template <typename T, typename R>
struct reference_converts_from_temporary :
    bool_constant<__reference_converts_from_temporary(T, R)> {};

#else // if __UTL_SHOULD_USE_BUILTIN(reference_converts_from_temporary)

template <typename T>
inline constexpr bool reference_converts_from_temporary_v =
    unsupported_trait_v<T>;
template <typename T>
struct reference_converts_from_temporary : unsupported_trait<T> {};

#endif // if __UTL_SHOULD_USE_BUILTIN(reference_converts_from_temporary)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
