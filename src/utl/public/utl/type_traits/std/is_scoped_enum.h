// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

#if !__UTL_SHOULD_USE_BUILTIN(is_scoped_enum)
#  include "utl/type_traits/underlying_type.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(is_scoped_enum)

template <typename T>
inline constexpr bool is_scoped_enum_v = __is_scoped_enum(T);
template <typename T>
struct is_scoped_enum : bool_constant<__is_scoped_enum(T)> {};

#else

template <typename T>
inline constexpr bool is_scoped_enum_v = false;

template <typename T>
requires requires {
    typename underlying_type_t<T>;
    requires requires(void (*func)(underlying_type_t<T>), T val) { func(val); };
};
inline constexpr bool is_scoped_enum_v = true;

template <typename T>
struct is_scoped_enum : bool_constant<is_scoped_enum_v(T)> {};

#endif // if __UTL_SHOULD_USE_BUILTIN(is_scoped_enum)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
