// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#include "utl/filesystem/utl_permissions.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/tempus/utl_clock.h"

__UFS_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC file_status {
    uintmax_t file_size;
    time_point last_accessed;
    time_point last_modified;
    time_point last_changed;
    uintmax_t hard_links = 0;
    perms permissions = perms::unknown;
    file_type type = file_type::invalid;
};

__UFS_NAMESPACE_END
