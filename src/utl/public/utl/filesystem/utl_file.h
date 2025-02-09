// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#include "utl/filesystem/utl_basic_operations.h"
#include "utl/filesystem/utl_file_error.h"
#include "utl/filesystem/utl_file_status.h"
#include "utl/filesystem/utl_file_type.h"
#include "utl/filesystem/utl_file_view.h"
#include "utl/filesystem/utl_path.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/memory/utl_allocator.h"
#include "utl/string/utl_basic_short_string.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_is_string_char.h"
#include "utl/system_error/utl_error_code.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/utility/utl_compressed_pair.h"
#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

template <typename Alloc>
class __UTL_PUBLIC_TEMPLATE basic_file {
    using allocator_type = Alloc;
    using path_type = basic_string<path_char, allocator_type>;
    template <file_type Type>
    using explicit_file = basic_explicit_file<Type, allocator_type>;
    using view_type = path_view;
    using snapshot = __UTL basic_file_snapshot<allocator_type>;
    template <file_type Type>
    using explicit_snapshot = __UTL basic_explicit_file_snapshot<Type, allocator_type>;

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_base_of(basic_file, remove_cvref_t<T>))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_base_of(basic_file, remove_cvref_t<T>))
    __UTL_HIDE_FROM_ABI static inline constexpr auto get_path(T&& t) noexcept
        -> decltype(__UTL forward_like<T>(__UTL declval<path_type&>())) {
        return __UTL forward_like<T>(((copy_cv_t<remove_reference_t<T>, basic_file>&)t).path_);
    }

public:
    __UTL_HIDE_FROM_ABI inline basic_file() = delete;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(path_type)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file& operator=(basic_file const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file& operator=(basic_file&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(path_type)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_file() noexcept = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(
        basic_file const& other, allocator_type const& a) UTL_THROWS
        : path_{other.path_, a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(path_type, path_type,
        allocator_type const&))
        : path_{__UTL move(other.path_), a} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(path_type&& path) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(path_type))
        : path_{__UTL move(path)} {}

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>)... Vs UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(
            bool_constant<(sizeof...(Vs) > 0)>, is_convertible<Vs, view_type>...))>
    UTL_CONSTRAINT_CXX20(sizeof...(Vs) > 0 && is_convertible_v<Vs, view_type>)
    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        explicit_file<file_type::directory>&& dir, Vs&&... tail)
        : path_{__UFS path::join(get_path(__UTL move(dir)), __UTL forward<Vs>(tail)...)} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        view_type view, allocator_type const& a = allocator_type{}) UTL_THROWS
        : path_{view, a} {}

    template <typename View UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_convertible(View, view_type))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_convertible(View, view_type))
    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        View const& path, allocator_type const& a = allocator_type{}) UTL_THROWS
        : path_{static_cast<view_type>(path), a} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        path_char const* path, allocator_type const& a = allocator_type{}) UTL_THROWS
        : basic_file{view_type(path), a} {}

    template <UTL_CONCEPT_CXX20(string_char) Char UTL_CONSTRAINT_CXX11(
        sizeof(Char) == sizeof(path_char))>
    UTL_CONSTRAINT_CXX20(sizeof(Char) == sizeof(path_char))
    __UTL_HIDE_FROM_ABI explicit inline basic_file(
        Char const* bytes, allocator_type const& a = allocator_type{}) UTL_THROWS
        : basic_file{view_type(reinterpret_cast<path_char*>(bytes)), a} {}

    template <typename View UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_convertible(View, file_view))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_convertible(View, file_view))
    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        View const& other, allocator_type const& a = allocator_type{}) UTL_THROWS
        : basic_file{static_cast<file_view>(other).path(), a} {}

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline constexpr zpath_view path() const noexcept UTL_LIFETIMEBOUND {
        return static_cast<zpath_view>{path_};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator file_view() const noexcept UTL_LIFETIMEBOUND {
        return file_view{path()};
    }

    __UTL_HIDE_FROM_ABI inline result<file_status> status() const noexcept {
        return __UFS status(path());
    }

    __UTL_HIDE_FROM_ABI inline result<file_status> symlink_status() const noexcept {
        return __UFS symlink_status(path());
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file<file_type::directory>
    parent_directory() const& noexcept {
        return explicit_file<file_type::directory>{__UFS path::dirname(path_)};
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file<file_type::directory>
    parent_directory() && noexcept {
        auto const dir = __UFS path::dirname(path_);
        if (path_.starts_with(dir)) {
            return explicit_file<file_type::directory>{__UTL move(path_).substr(0, dir.size())};
        }

        // dir is "."
        path_ = dir;
        return explicit_file<file_type::directory>{__UTL move(path_)};
    }

    __UTL_HIDE_FROM_ABI inline result<snapshot> to_snapshot() const& {
        return this->status().and_then([this](file_status const& status) {
            return result<snapshot>{__UTL in_place, *this, status, get_time(tempus::steady_clock)};
        });
    }

    __UTL_HIDE_FROM_ABI inline result<snapshot> to_snapshot() && noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(path_type)) {
        return this->status().and_then([&](file_status const& status) {
            return result<snapshot>{
                __UTL in_place, __UTL move(*this), status, get_time(tempus::steady_clock)};
        });
    }

    // Cast to explicit file is not provided because it either requires assuming the cast is valid
    // so that we may construct it cheaply or perform it properly which requires fetching
    // file_status and then immediately discard it.
    // 1. Assuming the cast is valid defeats the purpose of having an explicit file type, at which
    // point, just using the basic_file is sufficient. If users would like to assume a file is a
    // specific type, they may construct the explicit file type manually, which requires more
    // verbosity as it reduces the guarantee that the file type is correct at construction time.
    //
    // 2. Fetching file_status properly is quite an expensive operation as it requires going through
    // the kernel filesystem API, i.e. acquiring locks, making system calls, etc.
    //
    // It is better to expose a cast to explicit snapshot and let the user decide if they want to
    // discard the file_status from there, at which point file type can be guranteed to be valid at
    // construction time (although this could change throughout the application lifetime).

    template <file_type Type>
    __UTL_HIDE_FROM_ABI inline result<explicit_snapshot<Type>> to_snapshot() const& {
        return this->status().and_then([this](file_status const& status) {
            if (status.type == Type) {
                return result<explicit_snapshot<Type>>{
                    __UTL in_place, *this, status, get_time(tempus::steady_clock)};
            } else {
                return result<explicit_snapshot<Type>>{
                    __UTL unexpect, error_value::file_type_mismatch};
            }
        });
    }

    template <file_type Type>
    __UTL_HIDE_FROM_ABI inline result<explicit_snapshot<Type>> to_snapshot() && noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(path_type)) {
        return this->status().and_then([&](file_status const& status) {
            if (status.type == Type) {
                return result<explicit_snapshot<Type>>{
                    __UTL in_place, __UTL move(*this), status, get_time(tempus::steady_clock)};
            } else {
                return result<explicit_snapshot<Type>>{
                    __UTL unexpect, error_value::file_type_mismatch};
            }
        });
    }

private:
    path_type path_;
};

template <file_type Type, typename Alloc>
class __UTL_PUBLIC_TEMPLATE basic_explicit_file : public basic_file<Alloc> {
    using allocator_type = Alloc;
    using base_type = basic_file<allocator_type>;
    using snapshot_type = __UTL basic_explicit_file_snapshot<Type, allocator_type>;
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::invalid), "Invalid file type");
    using base_type::symlink_status;

public:
    __UTL_HIDE_FROM_ABI inline constexpr basic_explicit_file() = delete;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file(
        basic_explicit_file const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file& operator=(
        basic_explicit_file const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file(basic_explicit_file&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file&
    operator=(basic_explicit_file&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file(basic_explicit_file&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, base_type,
        allocator_type const&))
        : base_type{static_cast<base_type&&>(other), a} {}

    template <typename View UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_convertible(View, explicit_file_view<Type>))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_convertible(View, explicit_file_view<Type>))
    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file(
        View const& other, allocator_type const& a = allocator_type{}) UTL_THROWS
        : base_type{static_cast<explicit_file_view<Type>>(other).path(), a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_explicit_file() noexcept = default;

    template <file_type U>
    basic_explicit_file(basic_explicit_file<U> const&) = delete;
    template <file_type U>
    basic_explicit_file(basic_explicit_file<U>&&) = delete;
    template <file_type U>
    basic_explicit_file(basic_explicit_file<U> const&, allocator_type const&) = delete;
    template <file_type U>
    basic_explicit_file(basic_explicit_file<U>&&, allocator_type const&) = delete;

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline constexpr basic_explicit_file(Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(base_type, Args...))
        : base_type{__UTL forward<Args>(args)...} {}

    __UTL_HIDE_FROM_ABI inline constexpr operator explicit_file_view<Type>() const noexcept UTL_LIFETIMEBOUND {
        return explicit_file_view<Type>{path()};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator file_view() const noexcept UTL_LIFETIMEBOUND {
        return file_view{path()};
    }

    using base_type::parent_directory;
    using base_type::path;

    __UTL_HIDE_FROM_ABI inline result<file_status> status() const noexcept {
        auto output =
            Type == file_type::symlink ? base_type::symlink_status() : base_type::status();
        if (!output) {
            return output;
        }

        if (output->status != Type) {
            return result<file_status>{__UTL unexpect, error_value::file_type_mismatch};
        }

        return output;
    }

    template <file_type>
    result<snapshot_type> to_snapshot() const& noexcept = delete;
    template <file_type>
    result<snapshot_type> to_snapshot() && noexcept = delete;

    __UTL_HIDE_FROM_ABI
    result<snapshot_type> to_snapshot() const& UTL_THROWS {
        return this->status().transform([&](file_status&& stat) {
            return snapshot_type{static_cast<base_type const&>(*this), __UTL move(stat),
                get_time(tempus::steady_clock)};
        });
    }

    __UTL_HIDE_FROM_ABI
    result<snapshot_type> to_snapshot() && noexcept(UTL_TRAIT_is_nothrow_constructible(
        snapshot_type, base_type, file_status const&, time_point)) {
        return this->status().transform([&](file_status&& stat) {
            return snapshot_type{
                static_cast<base_type&&>(*this), __UTL move(stat), get_time(tempus::steady_clock)};
        });
    }
};

using file = basic_file<>;
using regular_file = basic_explicit_file<file_type::regular>;
using directory = basic_explicit_file<file_type::directory>;
using block_device = basic_explicit_file<file_type::block_device>;
using character_device = basic_explicit_file<file_type::character_device>;
using fifo = basic_explicit_file<file_type::fifo>;
using symlink = basic_explicit_file<file_type::symlink>;
using socket = basic_explicit_file<file_type::socket>;
using unknown_file = basic_explicit_file<file_type::unknown>;

__UFS_NAMESPACE_END
