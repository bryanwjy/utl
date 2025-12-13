// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(has_virtual_destructor)
template <typename T>
inline constexpr bool has_virtual_destructor_v = __has_virtual_destructor(T);
template <typename T>
struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(has_virtual_destructor)
template <typename T>
inline constexpr bool has_virtual_destructor_v = unsupported_trait_v<T>;
template <typename T>
struct has_virtual_destructor : unsupported_trait<T> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(has_virtual_destructor)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
