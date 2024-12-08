// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_permissions.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/tempus/utl_clock.h"

__UFS_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC file_status {
    uintmax_t file_size;
    tempus::time_point<file_clock> last_accessed = 0;
    tempus::time_point<file_clock> last_modified = 0;
    tempus::time_point<file_clock> last_changed = 0;
    uintmax_t hard_links = 0;
    perms perimissions = perms::invalid;
    file_type type = file_type::invalid;
};

__UTL_ABI_PUBLIC result<file_status> status(path_view view) noexcept;

__UFS_NAMESPACE_END
