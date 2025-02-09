// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI, CONST)
inline constexpr perms operator|(perms lhs, perms rhs) noexcept {
    return static_cast<perms>(__UTL to_underlying(lhs) | __UTL to_underlying(rhs));
}
UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI, CONST)
inline constexpr perms operator&(perms lhs, perms rhs) noexcept {
    return static_cast<perms>(__UTL to_underlying(lhs) & __UTL to_underlying(rhs));
}

UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI)
inline constexpr perms& operator|=(perms& lhs, perms rhs) noexcept {
    return lhs = (lhs | rhs);
}
UTL_ATTRIBUTE(NODISCARD, _HIDE_FROM_ABI)
inline constexpr perms& operator&=(perms& lhs, perms rhs) noexcept {
    return lhs = (lhs & rhs);
}

__UFS_NAMESPACE_END
