// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_permissions.h"
#include "utl/filesystem/utl_platform.h"

__UFS_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC file_metadata {
    explicit file_metadata(basic_string_view<path_char> path) noexcept
        : path_size_((unsigned short)path.size()) {}

    size_t file_size;                         // 8
    time_point<file_clock> created = 0;       // 8
    time_point<file_clock> last_accessed = 0; // 8
    time_point<file_clock> last_modified = 0; // 8
    size_t hard_links = 0;                    // 8
    perms perimissions = perms::invalid;      // 4
    uint16_t aux16 = 0;                       // 2
    uint8_t aux8 = 0;                         // 1
    file_type type = file_type::invalid;      // 1

    __UTL_HIDE_FROM_ABI inline constexpr bool valid() const noexcept {
        return static_cast<uint8_t>(type) < static_cast<uint8_t>(file_type::invalid);
    }
};

__UFS_NAMESPACE_END
