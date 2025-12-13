// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"

#  include "utl/type_traits/constants.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

template <typename T>
inline constexpr size_t alignment_of_v = alignof(T);
template <typename T>
struct alignment_of : size_constant<alignment_of_v<T>> {};

UTL_PUBLIC_DEFAULT_NAMESPACE_END
