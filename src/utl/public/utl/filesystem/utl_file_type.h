// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

__UTL_HIDE_FROM_ABI constexpr bool is_valid(file_type type) noexcept {
    return to_underlying(type) < to_underlying(file_type::COUNT);
}

template <file_type Type>
__UTL_HIDE_FROM_ABI UTL_CONSTEVAL bool is_valid() noexcept {
    return is_valid(Type);
}

__UFS_NAMESPACE_END
