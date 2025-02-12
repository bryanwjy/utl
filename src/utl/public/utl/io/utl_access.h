// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/io/utl_io_fwd.h"

#include "utl/utility/utl_to_underlying.h"

UTL_NAMESPACE_BEGIN

namespace io {
enum class access : int {
    read = 1,
    write = 2,
    read_write = 3,
    execute = 4,
    all = 7
};

UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI, CONST)
inline constexpr access operator|(access lhs, access rhs) noexcept {
    return static_cast<access>(to_underlying(lhs) | to_underlying(rhs));
}
UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI, CONST)
inline constexpr access operator&(access lhs, access rhs) noexcept {
    return static_cast<access>(to_underlying(lhs) & to_underlying(rhs));
}

UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI)
inline UTL_CONSTEXPR_CXX14 access& operator|=(access& lhs, access rhs) noexcept {
    return lhs = (lhs | rhs);
}
UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI)
inline UTL_CONSTEXPR_CXX14 access& operator&=(access& lhs, access rhs) noexcept {
    return lhs = (lhs & rhs);
}
} // namespace io

UTL_NAMESPACE_END
