// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

template <typename From, typename To>
inline constexpr bool is_core_convertible_v =
    requires(void (*to)(To), From (*from)()) { to(from()); };

template <typename From, typename To>
struct is_core_convertible : bool_constant<is_core_convertible_v<From, To>> {};

UTL_PUBLIC_DEFAULT_NAMESPACE_END
