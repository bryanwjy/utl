// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_file_status.h"
#include "utl/filesystem/utl_file_traits.h"
#include "utl/filesystem/utl_path.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/string/utl_basic_short_string.h"
#include "utl/tempus/utl_clock.h"

__UFS_NAMESPACE_BEGIN

/**
 * Queries the file status of a given path from the underlying filesystem,
 * if the target path is a symlink the status of the target is obtained.
 *
 * @param view - path to get the status of
 *
 * @return file_status if success, error otherwise
 */
__UTL_ABI_PUBLIC result<file_status> status(zpath_view view) noexcept;

template <UTL_CONCEPT_CXX20(details::path::container) P>
__UTL_HIDE_FROM_ABI inline result<file_status> status(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    return status(static_cast<zpath_view>(path));
}

/**
 * Retrieves the file status of a given path, if the target path is a symlink the status of the
 * symlink itself is obtained.
 *
 * @param view - path to get the status of
 *
 * @return file_status if success, error otherwise
 */
__UTL_ABI_PUBLIC result<file_status> symlink_status(zpath_view view) noexcept;

template <UTL_CONCEPT_CXX20(details::path::container) P>
__UTL_HIDE_FROM_ABI inline result<file_status> symlink_status(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    return symlink_status(static_cast<zpath_view>(path));
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

template <UTL_CONCEPT_CXX20(details::path::container) P1,
    UTL_CONCEPT_CXX20(details::path::container) P2>
UTL_CONSTRAINT_CXX20(__UTL_FILE_TYPE_OF(P1) == __UTL_FILE_TYPE_OF(P2) || details::path::container_of<file_type::symlink, P1> || details::path::container_of<file_type::symlink, P2> || !details::path::typed_container<P1> || !details::path::typed_container<P2>)
__UTL_HIDE_FROM_ABI inline result<bool> equivalent(P1 const& left, P2 const& right) noexcept(
    noexcept(details::path::terminated_path(left)) && noexcept(
        details::path::terminated_path(right))) {
    auto const lhs = details::path::terminated_path(left);
    auto const rhs = details::path::terminated_path(right);
    return equivalent(static_cast<zpath_view>(lhs), static_cast<zpath_view>(right));
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

template <UTL_CONCEPT_CXX20(details::path::container) P1,
    UTL_CONCEPT_CXX20(details::path::container) P2>
UTL_CONSTRAINT_CXX20(__UTL_FILE_TYPE_OF(P1) == __UTL_FILE_TYPE_OF(P2) || !details::path::typed_container<P1> && !details::path::typed_container<P2>)
__UTL_HIDE_FROM_ABI inline result<void> rename(P1 const& from, P2 const& to) noexcept(noexcept(
    details::path::terminated_path(from)) && noexcept(details::path::terminated_path(to))) {
    auto const from_path = details::path::terminated_path(from);
    auto const to_path = details::path::terminated_path(to);
    return rename(static_cast<zpath_view>(from_path), static_cast<zpath_view>(to_path));
}

struct storage_info {
    uintmax_t capacity;
    uintmax_t free;
    uintmax_t available;
#if UTL_CXX20
    friend bool operator==(storage_info const& left, storage_info const& right) noexcept = default;
#else
    friend inline bool operator==(storage_info const& left, storage_info const& right) noexcept {
        return __UTL_MEMCMP(&left, &right, sizeof(storage_info)) == 0;
    }
    friend inline bool operator!=(storage_info const& left, storage_info const& right) noexcept {
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

template <UTL_CONCEPT_CXX20(details::path::container) P>
__UTL_ABI_PUBLIC inline result<storage_info> storage(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    return storage(static_cast<zpath_view>(path));
}

/**
 * Unlinks a file path from the underlying inode; in other words, removes a file from the
 * filesystem.
 *
 * @note Only succeeds on non-directory files
 *
 * @param file - file to remove/unlink
 *
 * @return 1 if a file was removed successfully, 0 if the file does not exist, error
 * otherwise
 */
__UTL_ABI_PUBLIC result<uintmax_t> unlink(zpath_view file) noexcept;

template <UTL_CONCEPT_CXX20(details::path::container) P>
UTL_CONSTRAINT_CXX20(!details::path::container_of<file_type::directory, P>)
__UTL_HIDE_FROM_ABI inline result<uintmax_t> unlink(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    return unlink(static_cast<zpath_view>(path));
}

/**
 * Removes an empty directory from the filesystem
 *
 * @param dir - directory to remove
 *
 * @return 1 if a directory was removed successfully, 0 if the file does not exist, error otherwise
 */
__UTL_ABI_PUBLIC result<uintmax_t> remove_directory(zpath_view directory) noexcept;

template <UTL_CONCEPT_CXX20(details::path::container) P>
UTL_CONSTRAINT_CXX20(!details::path::typed_container<P> || details::path::container_of<P, file_type::directory>)
__UTL_HIDE_FROM_ABI inline result<uintmax_t> remove_directory(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    return remove_directory(static_cast<zpath_view>(path));
}

/**
 * Removes a file, if the file is a directory only suceeds if the directory is empty
 *
 * @tparam FileType - type of file path_view points to
 *
 * @param file - file to remove
 *
 * @return 1 if a file was removed successfully, 0 if the file does not exist, error otherwise
 */
template <file_type F, UTL_CONCEPT_CXX20(details::path::container) P>
UTL_CONSTRAINT_CXX20(!details::path::typed_container<P> || details::path::container_of<P, F>)
__UTL_HIDE_FROM_ABI inline result<uintmax_t> remove(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    UTL_IF_CONSTEXPR(F == file_type::directory) {
        return remove_directory(static_cast<zpath_view>(path));
    } else {
        return unlink(static_cast<zpath_view>(path));
    }
}

template <UTL_CONCEPT_CXX20(details::path::typed_container) P>
__UTL_HIDE_FROM_ABI inline result<uintmax_t> remove(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    return remove_recursive<__UFS_FILE_TYPE_OF(P)>(p);
}

/**
 * Removes a file, if the file is a directory, removes every file and subdirectory within it
 *
 * @param file - file to remove
 *
 * @return number of removed files if success, error otherwise
 */
template <file_type F>
__UTL_HIDE_FROM_ABI result<uintmax_t> remove_recursive(zpath_view) noexcept = delete;

template <>
__UTL_ABI_PUBLIC result<uintmax_t> remove_recursive<file_type::directory>(zpath_view) noexcept;

extern template __UTL_ABI_PUBLIC result<uintmax_t> remove_recursive<file_type::directory>(
    zpath_view) noexcept;

template <file_type F, UTL_CONCEPT_CXX20(details::path::container) P>
UTL_CONSTRAINT_CXX20(!details::path::typed_container<P> || details::path::container_of<P, F>)
__UTL_HIDE_FROM_ABI inline result<uintmax_t> remove_recursive(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    UTL_IF_CONSTEXPR(F != file_type::directory) {
        return unlink(static_cast<zpath_view>(path));
    } else {
        return remove_recursive<file_type::directory>(static_cast<zpath_view>(path));
    }
}

template <UTL_CONCEPT_CXX20(details::path::typed_container) P>
__UTL_HIDE_FROM_ABI inline result<uintmax_t> remove_recursive(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    return remove_recursive<__UFS_FILE_TYPE_OF(P)>(p);
}

template <UTL_CONCEPT_CXX20(details::path::container) P>
__UTL_HIDE_FROM_ABI inline result<uintmax_t> remove_recursive(P const& p) noexcept(
    noexcept(details::path::terminated_path(p))) {
    auto const path = details::path::terminated_path(p);
    return remove_directory(static_cast<zpath_view>(path)).or_else([&](__UTL error_code code) {
        if (code.value() == errc::not_a_directory) {
            return unlink(static_cast<zpath_view>(path));
        }

        if (code.value() == errc::directory_not_empty) {
            return remove_recursive<file_type::directory>(static_cast<zpath_view>(path));
        }

        return result<uintmax_t>{__UTL unexpect, code};
    });
}

__UFS_NAMESPACE_END
