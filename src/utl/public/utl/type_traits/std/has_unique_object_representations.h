// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(has_unique_object_representations)
template <typename T>
inline constexpr bool has_unique_object_representations_v =
    __has_unique_object_representations(T);
template <typename T>
struct has_unique_object_representations :
    bool_constant<__has_unique_object_representations(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(has_unique_object_representations)
template <typename T>
inline constexpr bool has_unique_object_representations_v =
    unsupported_trait_v<T>;
template <typename T>
struct has_unique_object_representations : unsupported_trait<T> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(has_unique_object_representations)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
