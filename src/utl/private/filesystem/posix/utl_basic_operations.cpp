// Copyright 2023-2024 Bryan Wong

#include "utl/filesystem/utl_basic_operations.h"

// POSIX
#if (UTL_TARGET_APPLE | UTL_TARGET_LINUX | UTL_TARGET_BSD) && !UTL_TARGET_SONY && \
    !UTL_TARGET_NINTENDO
#  include "utl/system_error/utl_system_error.h"

#  include <sys/stat.h>
#  include <sys/statvfs.h>
#  include <sys/types.h>
#  include <unistd.h>

#  include <cerrno>
#  include <cstdlib>

__UFS_NAMESPACE_BEGIN

using file_clock_traits = tempus::clock_traits<tempus::file_clock_t>;

static file_status to_file_status(struct stat const& data) noexcept {
    file_status status;
    status.file_size = data.st_size;
    status.last_accessed = file_clock_traits::construct(data.st_atime);
    status.last_modified = file_clock_traits::construct(data.st_mtime);
    status.last_changed = file_clock_traits::construct(data.st_ctime);
    status.hard_links = data.st_nlink;
    status.permissions = static_cast<perms>(data.st_mode & static_cast<mode_t>(perms::mask));

    switch (data.st_mode & S_IFMT) {
    case S_IFBLK:
        status.type = file_type::block_device;
        break;
    case S_IFCHR:
        status.type = file_type::character_device;
        break;
    case S_IFDIR:
        status.type = file_type::directory;
        break;
    case S_IFIFO:
        status.type = file_type::fifo;
        break;
    case S_IFLNK:
        status.type = file_type::symlink;
        break;
    case S_IFREG:
        status.type = file_type::regular;
        break;
    case S_IFSOCK:
        status.type = file_type::socket;
        break;
    default:
        status.type = file_type::unknown;
        break;
    }
    return status;
}

template <typename T>
static result<T> error_from_errno() noexcept {
    return result<T>{__UTL unexpect, errno, __UTL system_category()};
}

result<file_status> status(zpath_view view) noexcept {
    struct stat intermediate;
    if (::lstat(reinterpret_cast<char const*>(view.data()), &intermediate) != EXIT_SUCCESS) {
        return error_from_errno<file_status>();
    }

    return to_file_status(intermediate);
}

result<bool> equivalent(zpath_view left, zpath_view right) noexcept {
    struct stat stats[2];
    if (::stat(reinterpret_cast<char const*>(left.data()), stats) != EXIT_SUCCESS) {
        return error_from_errno<bool>();
    }

    if (::stat(reinterpret_cast<char const*>(right.data()), stats + 1) != EXIT_SUCCESS) {
        return error_from_errno<bool>();
    }

    return stats[0].st_dev == stats[1].st_dev && stats[0].st_ino == stats[1].st_ino;
}

result<void> rename(zpath_view from, zpath_view to) noexcept {
    if (::rename(reinterpret_cast<char const*>(from.data()),
            reinterpret_cast<char const*>(to.data())) != EXIT_SUCCESS) {
        return error_from_errno<void>();
    }

    return {};
}

result<void> unlink(zpath_view file) noexcept {
    if (::unlink(reinterpret_cast<char const*>(file.data())) != EXIT_SUCCESS) {
        return error_from_errno<void>();
    }

    return {};
}

result<void> remove_directory(zpath_view file) noexcept {
    if (::rmdir(reinterpret_cast<char const*>(file.data())) != EXIT_SUCCESS) {
        return error_from_errno<void>();
    }

    return {};
}

result<storage_info> storage(zpath_view path) noexcept {
    struct statvfs fs_stat;
    if (::statvfs(reinterpret_cast<char const*>(path.data()), &fs_stat) != EXIT_SUCCESS) {
        return error_from_errno<storage_info>();
    }

    storage_info info;
    info.capacity = fs_stat.f_blocks * fs_stat.f_frsize;
    info.free = fs_stat.f_bfree * fs_stat.f_frsize;
    info.available = fs_stat.f_bavail * fs_stat.f_frsize;
    return info;
}

__UFS_NAMESPACE_END
#endif // (UTL_TARGET_APPLE | UTL_TARGET_LINUX | UTL_TARGET_BSD) && !UTL_TARGET_SONY &&
       // !UTL_TARGET_NINTENDO
