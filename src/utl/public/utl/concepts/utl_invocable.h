// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_invoke.h"

#if UTL_CXX20
UTL_NAMESPACE_BEGIN

template <typename F, typename... Args>
concept invocable = is_invocable_v<F, Args...>;

template <typename F, typename... Args>
concept regular_invocable = invocable<F, Args...>;

UTL_NAMESPACE_END
#endif
