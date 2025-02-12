// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/memory/utl_allocator_fwd.h"
#include "utl/string/utl_string_fwd.h"
#include "utl/tempus/utl_clock_fwd.h"

#include "utl/filesystem/utl_platform.h"
#include "utl/io/utl_access.h"

__UFS_NAMESPACE_BEGIN
enum class file_type : unsigned char {
    block_device,
    character_device,
    directory,
    fifo,
    symlink,
    regular,
    socket,
    unknown,
    invalid,
    COUNT = invalid
};

enum class perms : int {
    none = 0,
    owner_read = 0400,
    owner_write = 0200,
    owner_exec = 0100,
    owner_all = 0700,
    group_read = 040,
    group_write = 020,
    group_exec = 010,
    group_all = 070,
    others_read = 04,
    others_write = 02,
    others_exec = 01,
    others_all = 0777,
    set_uid = 04000,
    set_gid = 02000,
    sticky_bit = 01000,
    mask = 07777,
    unknown = 0xffff
};

using time_point = __UTL tempus::time_point<tempus::file_clock_t>;
using path_view = __UTL basic_string_view<path_char>;
using zpath_view = __UTL basic_zstring_view<path_char>;

struct __UTL_ABI_PUBLIC file_status;
struct __UTL_ABI_PUBLIC storage_info;

template <io::access A>
class __UTL_PUBLIC_TEMPLATE basic_file_handle;

using file_handle = basic_file_handle<io::access::read_write>;
using read_file_handle = basic_file_handle<io::access::read>;
using write_file_handle = basic_file_handle<io::access::write>;

template <typename... Os>
struct __UTL_PUBLIC_TEMPLATE open_flags_t;

class __UTL_ABI_PUBLIC file_view;

class __UTL_ABI_PUBLIC file_view_snapshot;

template <typename Alloc = __UTL allocator<path_char>>
class __UTL_PUBLIC_TEMPLATE basic_file;

template <file_type Type, typename Alloc = __UTL allocator<path_char>>
class __UTL_PUBLIC_TEMPLATE basic_explicit_file;

template <typename Alloc = __UTL allocator<path_char>>
class __UTL_PUBLIC_TEMPLATE basic_file_snapshot;

template <file_type Type, typename Alloc = __UTL allocator<path_char>>
class __UTL_PUBLIC_TEMPLATE basic_explicit_file_snapshot;

template <file_type Type>
class __UTL_PUBLIC_TEMPLATE explicit_file_view;

template <file_type Type>
class __UTL_PUBLIC_TEMPLATE explicit_file_view_snapshot;

namespace views {
using file = file_view;
using regular_file = explicit_file_view<file_type::regular>;
using directory = explicit_file_view<file_type::directory>;
using block_device = explicit_file_view<file_type::block_device>;
using character_device = explicit_file_view<file_type::character_device>;
using fifo = explicit_file_view<file_type::fifo>;
using symlink = explicit_file_view<file_type::symlink>;
using socket = explicit_file_view<file_type::socket>;
using unknown_file = explicit_file_view<file_type::unknown>;
} // namespace views

__UFS_NAMESPACE_END
