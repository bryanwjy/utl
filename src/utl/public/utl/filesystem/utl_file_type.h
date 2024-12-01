// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_platform.h"

__UFS_NAMESPACE_BEGIN

enum class file_type : unsigned char {
    block_device,
    character_device,
    directory,
    fifo,
    symlink,
    regular,
    socket,
    junction,
    other,
    invalid,
    COUNT = invalid
};

template <file_type, typename Alloc>
class basic_explicit_file;

__UFS_NAMESPACE_END
