// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if __UTL_SHOULD_USE_BUILTIN(builtin_invoke)
template <typename... Args>
using invoke_result_t =
    decltype(__builtin_invoke(static_cast<Args && (*)()>(0)()...));

template <typename... Args>
inline constexpr bool is_invocable_v =
    requires { typename invoke_result_t<Args...>; };

#elif __UTL_SHOULD_USE_BUILTIN(is_invocable)

// TODO

#endif // if __UTL_SHOULD_USE_BUILTIN(is_invocable)

template <typename... Args>
struct is_invocable : bool_constant<is_invocable_v<Args...>> {};

UTL_PUBLIC_DEFAULT_NAMESPACE_END
