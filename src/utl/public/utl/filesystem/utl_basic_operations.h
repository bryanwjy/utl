// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_file_status.h"
#include "utl/filesystem/utl_path.h"
#include "utl/filesystem/utl_permissions.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/string/utl_basic_short_string.h"
#include "utl/tempus/utl_clock.h"

__UFS_NAMESPACE_BEGIN

namespace details {
using path_string = __UTL basic_short_string<path_char, 256>;
}

/**
 * Retrieves the file status of a given path WITHOUT follwoing symlinks
 *
 * @param view - path to get the status of
 *
 * @return file_status if success, error otherwise
 */
__UTL_ABI_PUBLIC result<file_status> status(zpath_view view) noexcept;
__UTL_HIDE_FROM_ABI inline result<file_status> status(path_view view) UTL_THROWS {
    details::path_string path{view};
    return status(static_cast<zpath_view>(path));
}

/**
 * Checks if two paths point to the same file
 *
 * @param left - path to compare
 * @param right - path to compare
 *
 * @return boolean indicating if the paths are equivalent if success, error otherwise
 */
__UTL_ABI_PUBLIC result<bool> equivalent(zpath_view left, zpath_view right) noexcept;
__UTL_HIDE_FROM_ABI inline result<bool> equivalent(zpath_view left, path_view right) UTL_THROWS {
    details::path_string other{right};
    return equivalent(left, static_cast<zpath_view>(other));
}
__UTL_HIDE_FROM_ABI inline result<bool> equivalent(path_view left, zpath_view right) UTL_THROWS {
    details::path_string other{left};
    return equivalent(static_cast<zpath_view>(other), right);
}
__UTL_ABI_PUBLIC inline result<bool> equivalent(path_view left, path_view right) UTL_THROWS {
    details::path_string lpath{left}, rpath{right};
    return equivalent(static_cast<zpath_view>(lpath), static_cast<zpath_view>(rpath));
}

/**
 * Renames a file
 *
 * @param from - path to rename from (should exist)
 * @param right - path to rename to (should not exist)
 *
 * @return nothing if success, error otherwise
 */
__UTL_ABI_PUBLIC result<void> rename(zpath_view from, zpath_view to) noexcept;
__UTL_HIDE_FROM_ABI inline result<void> rename(zpath_view from, path_view to) UTL_THROWS {
    details::path_string to_path{to};
    return rename(from, static_cast<zpath_view>(to_path));
}
__UTL_HIDE_FROM_ABI inline result<void> rename(path_view from, zpath_view to) UTL_THROWS {
    details::path_string from_path{from};
    return rename(static_cast<zpath_view>(from_path), to);
}
__UTL_HIDE_FROM_ABI inline result<void> rename(path_view from, path_view to) UTL_THROWS {
    details::path_string from_path{from}, to_path{to};
    return rename(static_cast<zpath_view>(from_path), static_cast<zpath_view>(to_path));
}

/**
 * Unlinks a file path from the underlying inode; in other words, removes a file from the
 * filesystem.
 *
 * @note Only succeeds on non-directory files
 *
 * @param file - file to remove/unlink
 *
 * @return nothing if success, error otherwise
 */
__UTL_ABI_PUBLIC result<void> unlink(zpath_view file) noexcept;
__UTL_HIDE_FROM_ABI result<void> unlink(path_view file) UTL_THROWS {
    details::path_string path{file};
    return unlink(static_cast<zpath_view>(path));
}

/**
 * Removes an empty directory from the filesystem
 *
 * @param dir - directory to remove
 *
 * @return nothing if success, error otherwise
 */
__UTL_ABI_PUBLIC result<void> remove_directory(zpath_view directory) noexcept;
__UTL_HIDE_FROM_ABI result<void> remove_directory(path_view directory) UTL_THROWS {
    details::path_string path{directory};
    return remove_directory(static_cast<zpath_view>(path));
}

struct storage_info {
    uintmax_t capacity;
    uintmax_t free;
    uintmax_t available;
#if UTL_CXX20
    friend bool operator==(storage_info const& left, storage_info const& right) noexcept = default;
#else
    friend bool operator==(storage_info const& left, storage_info const& right) noexcept {
        return __UTL_MEMCMP(&left, &right, sizeof(storage_info)) == 0;
    }
    friend bool operator!=(storage_info const& left, storage_info const& right) noexcept {
        return !(left == right);
    }
#endif
};

/**
 * Retrieves the storage information of the filesystem that contains a resource at the given path
 *
 * @param path - File path of a resource in the filesystem to query the information on
 *
 * @return storage info if success, error otherwise
 */
__UTL_ABI_PUBLIC result<storage_info> storage(zpath_view path) noexcept;
__UTL_ABI_PUBLIC result<storage_info> storage(path_view path) noexcept {
    details::path_string path_with_term{path};
    return storage(static_cast<zpath_view>(path_with_term));
}

/**
 * Removes a file, if the file is a directory only suceeds if the directory is empty
 *
 * @tparam FileType - type of file path_view points to
 *
 * @param file - file to remove
 *
 * @return nothing if success, error otherwise
 */
template <file_type FileType>
__UTL_HIDE_FROM_ABI result<void> remove(zpath_view file) noexcept {
    return unlink(file);
}
template <>
__UTL_HIDE_FROM_ABI result<void> remove<file_type::directory>(zpath_view file) noexcept {
    return remove_directory(file);
}
template <file_type FileType>
__UTL_HIDE_FROM_ABI result<void> remove(path_view file) UTL_THROWS {
    details::path_string path{file};
    return remove<FileType>(static_cast<zpath_view>(path));
}

/**
 * Removes a file, if the file is a directory, removes every file and subdirectory within it
 *
 * @param file - file to remove
 *
 * @return number of if success, error otherwise
 */
template <file_type Type>
__UTL_HIDE_FROM_ABI result<uintmax_t> remove_recursive(zpath_view file) noexcept {
    return unlink(file).and_then([]() { return 1u; });
}
template <file_type Type>
__UTL_HIDE_FROM_ABI result<uintmax_t> remove_recursive(path_view file) UTL_THROWS {
    details::path_string path{file};
    return remove_recursive<Type>(static_cast<zpath_view>(path));
}
// template <>
// __UTL_HIDE_FROM_ABI result<uintmax_t> remove_recursive<file_type::directory>(
//     zpath_view path) UTL_THROWS {
//     uintmax_t count = 0;
//     walk(views::directory{path}, bottom_up,
//         make_file_visitor(
//             [&](views::directory dir) {
//                 auto const path = file.path();
//                 // walk always returns null-terminated paths
//                 zpath_view view{path.path(), path.size()};
//                 count += remove_directory(view).has_value();
//             },
//             [&](views::file file) {
//                 auto const path = file.path();
//                 // walk always returns null-terminated paths
//                 zpath_view view{path.path(), path.size()};
//                 count += unlink(view).has_value();
//             }));
//     return remove_directory(path);
// }

__UFS_NAMESPACE_END
