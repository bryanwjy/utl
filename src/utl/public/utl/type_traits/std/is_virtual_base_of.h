// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(builtin_is_virtual_base_of)
template <typename Base, typename Derived>
inline constexpr bool is_virtual_base_of_v =
    __builtin_is_virtual_base_of(Base, Derived);
template <typename Base, typename Derived>
struct is_virtual_base_of :
    bool_constant<__builtin_is_virtual_base_of(Base, Derived)> {};
#else  // if __UTL_SHOULD_USE_BUILTIN(builtin_is_virtual_base_of)
template <typename Base, typename Derived>
inline constexpr bool is_virtual_base_of_v = unsupported_trait_v<Base, Derived>;
template <typename Base>
inline constexpr bool is_virtual_base_of_v<Base, Base> = true;

template <typename Base, typename Derived>
struct is_virtual_base_of :
    bool_constant<is_virtual_base_of_v<Base, Derived>> {};
#endif // if __UTL_SHOULD_USE_BUILTIN(builtin_is_virtual_base_of)

UTL_PUBLIC_DEFAULT_NAMESPACE_END
