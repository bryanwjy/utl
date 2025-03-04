// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#if !defined(UTL_FILESYSTEM_FILE_HANDLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/expected/utl_expected.h"
#include "utl/filesystem/utl_file_flags.h"
#include "utl/filesystem/utl_path.h"

__UFS_NAMESPACE_BEGIN

namespace details {
namespace open {
template <typename>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int get_flag(...) noexcept {
    return 0;
}

UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(read_only_t) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(write_only_t) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(read_write_t) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(create_t) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(temporary_t) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(append_t<true>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(exclusive_t<true>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(close_on_exec_t<true>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(truncate_t<true>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(set_control_terminal_t<false>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(path_only_t<true>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(buffered_t<false>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(follow_symlink_t<false>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(blocking_t<false>) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(auto_file_sync_t) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int get_flag(auto_data_sync_t) noexcept;

template <typename F, UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_ENABLE_IF_CXX11(
    int, __UTL always_true<flag_element_t<F, Flags>>()) to_posix_flags(Flags const& f) noexcept {
    /// move this part into src
    return get_flag(get<F>(f));
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int to_posix_flags(...) noexcept {
    return 0;
}

template <UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int to_posix_flags(Flags f) noexcept {
    return to_posix_flags<access_t, Flags>(0) | to_posix_flags<exclusivity_t, Flags>(0) |
        to_posix_flags<control_terminal_t, Flags>(0) | to_posix_flags<truncation_t, Flags>(0) |
        to_posix_flags<descriptor_operation_t, Flags>(0) |
        to_posix_flags<buffer_behaviour_t, Flags>(0) | to_posix_flags<write_integrity_t, Flags>(0) |
        to_posix_flags<creation_t, Flags>(0) | to_posix_flags<exec_behaviour_t, Flags>(0) |
        to_posix_flags<append_mode_t, Flags>(0) | to_posix_flags<symlink_behaviour_t, Flags>(0);
}

template <file_type Type>
using file_type_constant_t = __UTL integral_constant<file_type, Type>;

template <file_type Type>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline UTL_CONSTEVAL int file_type_flag(
    file_type_constant_t<Type>) noexcept {
    return 0;
}

UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int file_type_flag(
    file_type_constant_t<file_type::directory>) noexcept;

UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) int file_type_flag(
    file_type_constant_t<file_type::symlink>) noexcept;

template <typename Flags>
UTL_CONSTRAINT_CXX20(has_flag<creation_t, Flags>::value)
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr UTL_ENABLE_IF_CXX11(
    int, has_flag<creation_t, Flags>::value) to_posix_mode(Flags f) noexcept {
    return to_underlying(get<creation_t>(f).mode());
}

UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int to_posix_mode(...) noexcept {
    return 0;
}

struct arguments_t {
    int flags;
    int mode;
};

UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) file_descriptor_t impl(
    read_file_handle const&, zpath_view, arguments_t) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) file_descriptor_t impl(zpath_view, arguments_t) noexcept;

UTL_ATTRIBUTES(_ABI_PUBLIC, CONST, NODISCARD) bool supports_symlink() noexcept;

template <file_type Type>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST, NODISCARD) inline constexpr int file_type_flag() noexcept {
    return file_type_flag(file_type_constant<Type>{});
}
} // namespace open
} // namespace details

template <typename T>
using open_result_t = __UTL expected<details::open::handle_type<T>, __UTL error_code>;

template <file_type Type, UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open(P const& path, Flags flags) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    // if P is a known symlink, Type can be specified to override symlink behaviour,
    // otherwise, P must either be unknown or equal to Type
    static_assert(__UFS_FILE_TYPE_OF(P) == file_type::unknown ||
            __UFS_FILE_TYPE_OF(P) == file_type::symlink || Type == __UFS_FILE_TYPE_OF(P),
        "Type mismatch");
    static_assert(details::open::ensure_valid_type_and_flags<Type, Flags>(), "Invalid input");
    UTL_IF_CONSTEXPR(Type == file_type::symlink) {
        if (!supports_symlink()) {
            return open_result_t<Flags>{
                __UTL unexpect, error_value::symlink_descriptors_unsupported};
        }
    }

    using native_handle = details::file_descriptor_t;
    details::open::arguments_t const args{
        details::open::to_posix_flags(flags) | details::open::file_type_flag<Type>(),
        details::open::to_posix_mode(flags)};

    details::open::handle_type<T> result =
        details::open::impl(details::path::terminated_path(path), args);
    if (result) {
        return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
    }

    return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
}

template <UTL_CONCEPT_CXX20(details::path::typed_container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open(P const& path, Flags flags) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    return open<__UFS_FILE_TYPE_OF(P)>(path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open(P const& path, Flags flags) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    static_assert(details::open::ensure_valid_flags<Flags>(), "Invalid input");
    using native_handle = details::file_descriptor_t;
    details::open::arguments_t const args{
        details::open::to_posix_flags(flags), details::open::to_posix_mode(flags)};

    details::open::handle_type<T> result =
        details::open::impl(details::path::terminated_path(path), args);

    if (result) {
        return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
    }

    return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
}

template <file_type Type, UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open_at(read_file_handle const& dir, P const& path, Flags flags) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    static_assert(__UFS_FILE_TYPE_OF(P) == file_type::unknown ||
            __UFS_FILE_TYPE_OF(P) == file_type::symlink || Type == __UFS_FILE_TYPE_OF(P),
        "Type mismatch");
    static_assert(details::open::ensure_valid_type_and_flags<Type, Flags>(), "Invalid input");

    UTL_IF_CONSTEXPR(Type == file_type::symlink) {
        if (!supports_symlink()) {
            return open_result_t<Flags>{
                __UTL unexpect, error_value::symlink_descriptors_unsupported};
        }
    }

    using native_handle = details::file_descriptor_t;
    details::open::arguments_t const args{
        details::open::to_posix_flags(flags) | details::open::file_type_flag<Type>(),
        details::open::to_posix_mode(flags)};

    details::open::handle_type<T> result =
        details::open::impl(dir, details::path::terminated_path(path), args);

    if (result) {
        return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
    }

    return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
}

template <UTL_CONCEPT_CXX20(details::path::typed_container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open_at(read_file_handle const& dir, P const& path, Flags flags) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    return open_at<__UFS_FILE_TYPE_OF(P)>(dir, path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD)
UTL_ENABLE_IF_CXX11(open_result_t<Flags>, details::open::is_valid_flags<Flags>::value)
open_at(read_file_handle const& dir, P const& path, Flags flags) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    using native_handle = details::file_descriptor_t;
    static_assert(details::open::ensure_valid_flags<Type, Flags>(), "Invalid input");

    details::open::arguments_t const args{
        details::open::to_posix_flags(flags), details::open::to_posix_mode(flags)};

    details::open::handle_type<T> result =
        details::open::impl(dir, details::path::terminated_path(path), args);

    if (result) {
        return open_result_t<Flags>{__UTL in_place, __UTL move(result)};
    }

    return open_result_t<Flags>{__UTL unexpect, errno, __UTL system_category()};
}

struct cwd_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr cwd_t() noexcept = default;
};

__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr cwd_t cwd{};

template <file_type Type, UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) auto open_at(cwd_t, P const& path, Flags flags) noexcept(
    noexcept(open<Type>(path, flags))) -> decltype(open<Type>(path, flags)) {
    return open<Type>(path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::container) P, UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) auto open_at(cwd_t, P const& path, Flags flags) noexcept(
    noexcept(open(path, flags))) -> decltype(open(path, flags)) {
    return open(path, flags);
}

template <UTL_CONCEPT_CXX20(details::path::container_of<file_type::regular_file>) P, UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) file_handle create(P const& path, perms mode) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    return open<file_type::regular_file>(
        path, file_flags::create_file(mode) | file_flags::read_write | file_flags::truncate);
}

template <UTL_CONCEPT_CXX20(details::path::container) P,
    UTL_CONCEPT_CXX20(details::open::valid_flags) Flags>
UTL_CONSTRAINT_CXX20(!details::path::typed_container<P>)
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) file_handle create(P const& path, perms mode) noexcept(
    noexcept(static_cast<zpath_view>(details::path::terminated_path(path)))) {
    return open<file_type::regular_file>(
        path, file_flags::create_file(mode) | file_flags::read_write | file_flags::truncate);
}

__UFS_NAMESPACE_END
