// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/concepts/utl_convertible_to.h"
#include "utl/filesystem/utl_file_error.h"
#include "utl/filesystem/utl_file_status.h"
#include "utl/filesystem/utl_file_type.h"
#include "utl/filesystem/utl_path.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_is_string_char.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

template <file_type Type>
class __UTL_PUBLIC_TEMPLATE explicit_file_view;

class __UTL_ABI_PUBLIC file_view {
    template <file_type Type>
    using explicit_type = explicit_file_view<Type>;
    using view_type = basic_string_view<path_char>;
    using snapshot_type = __UTL file_view_snapshot;
    template <file_type Type>
    using explicit_snapshot_type = __UTL explicit_file_view_snapshot<Type>;
    using size_type = path::size_type;

public:
    __UTL_HIDE_FROM_ABI inline file_view() = delete;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view(file_view const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view(file_view&& other) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view& operator=(file_view const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view& operator=(file_view&& other) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~file_view() noexcept = default;

    template <typename Arg UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_convertible(Arg, view_type))>
    UTL_CONSTRAINT_CXX20(convertible_to<Arg, view_type>)
    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 file_view(Arg const& arg) noexcept(
        UTL_TRAIT_is_nothrow_convertible(Arg, view_type))
        : path_{static_cast<view_type>(arg)} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 file_view(view_type view) noexcept
        : path_{view} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 file_view(path_char const* path) noexcept
        : file_view{view_type(path)} {}

    template <UTL_CONCEPT_CXX20(string_char) Char UTL_CONSTRAINT_CXX11(
        sizeof(Char) == sizeof(path_char))>
    UTL_CONSTRAINT_CXX20(sizeof(Char) == sizeof(path_char))
    __UTL_HIDE_FROM_ABI explicit inline file_view(Char const* bytes) noexcept
        : file_view{view_type(reinterpret_cast<path_char*>(bytes))} {}

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr view_type path() const noexcept {
        return path_;
    }

    __UTL_HIDE_FROM_ABI inline result<file_status> status() const noexcept {
        return __UFS status(path());
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_type<file_type::directory>
    parent_directory() noexcept {
        return explicit_type<file_type::directory>{__UFS path::dirname(path())};
    }

    __UTL_HIDE_FROM_ABI inline result<snapshot_type> to_snapshot() const noexcept {
        return this->status().and_then([this](file_status const& status) {
            return result<snapshot_type>{__UTL in_place, path(), status, get_time(steady_clock)};
        });
    }

    template <file_type Type>
    __UTL_HIDE_FROM_ABI inline result<explicit_snapshot_type<Type>> to_snapshot() const noexcept {
        return this->status().and_then([this](file_status const& status) {
            if (status.type == Type) {
                return result<explicit_snapshot_type<Type>>{
                    __UTL in_place, path(), status, get_time(steady_clock)};
            } else {
                return details::make_error<fs_errc::file_type_mismatch,
                    explicit_snapshot_type<Type>>();
            }
        });
    }

private:
    path_container path_;
};

template <file_type Type>
class __UTL_PUBLIC_TEMPLATE explicit_file_view : public file_view {
    using base_type = file_view;
    using snapshot_type = __UTL file_view_snapshot<Type>;
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::invalid), "Invalid file type");

public:
    __UTL_HIDE_FROM_ABI inline constexpr explicit_file_view() = delete;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view(
        explicit_file_view const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view& operator=(
        explicit_file_view const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view(
        explicit_file_view&& other) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view& operator=(
        explicit_file_view&& other) noexcept = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~explicit_file_view() noexcept = default;

    template <file_type U>
    explicit_file_view(explicit_file_view<U> const&) = delete;
    template <file_type U>
    explicit_file_view(explicit_file_view<U>&&) = delete;
    template <file_type U>
    explicit_file_view(explicit_file_view<U> const&, allocator_type const&) = delete;
    template <file_type U>
    explicit_file_view(explicit_file_view<U>&&, allocator_type const&) = delete;

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline constexpr explicit_file_view(Args&&... args)
        : base_type{__UTL forward<Args>(args)...} {}

    using base_type::parent_directory;
    using base_type::path;

    // TODO: Maybe return error if file type doesn't match?
    using base_type::status;

    __UTL_HIDE_FROM_ABI
    inline result<snapshot_type> to_snapshot() const noexcept {
        return base_type::status().and_then([&](file_status const& stat) {
            if (stat.type == Type) {
                return result<snapshot_type>{__UTL in_place, static_cast<base_type const&>(*this),
                    stat, get_time(steady_clock)};
            } else {
                return details::make_error<fs_errc::file_type_mismatch, snapshot_type>();
            }
        });
    }
};

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
