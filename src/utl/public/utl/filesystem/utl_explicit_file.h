// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_file.h"
#include "utl/filesystem/utl_file_status.h"
#include "utl/filesystem/utl_file_type.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/system_error/utl_error_code.h"
#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

template <file_type Type, typename Alloc = __UTL allocator<path_char>>
class UTL_PUBLIC_TEMPLATE basic_explicit_file : private basic_file<Alloc> {
    using allocator_type = Alloc;
    using base_type = basic_file<allocator_type>;
    using snapshot_type = __UTL basic_explicit_file_snapshot<Type, allocator_type>;
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::invalid), "Invalid file type");

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

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file(
        basic_explicit_file const& other, allocator_type const& a)
        : base_type(static_cast<base_type const&>(other), a) {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file(basic_explicit_file&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, base_type,
        allocator_type const&))
        : base_type(static_cast<base_type&&>(other), a) {}

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
    __UTL_HIDE_FROM_ABI explicit inline constexpr basic_explicit_file(Args&&... args)
        : base_type{__UTL forward<Args>(args)...} {}

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type const&() const& { return *this; }

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type&&() && { return __UTL move(*this); }

    using base_type::path;

    __UTL_HIDE_FROM_ABI
    result<snapshot_type> to_snapshot() const& UTL_THROWS {
        return base_type::status().and_then([&](file_status const& stat) {
            if (stat.type == Type) {
                return result<snapshot_type>{__UTL in_place, static_cast<base_type const&>(*this),
                    stat, get_time(file_clock)};
            } else {
                return details::make_type_error<explicit_snapshot<Type>>();
            }
        });
    }

    __UTL_HIDE_FROM_ABI
    result<snapshot_type> to_snapshot() && noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) {
        return base_type::status().and_then([&](file_status const& stat) {
            if (stat.type == Type) {
                return result<snapshot_type>{
                    __UTL in_place, static_cast<base_type&&>(*this), stat, get_time(file_clock)};
            } else {
                // It may be possible for file to change type, e.g. other process delete and
                // recreate as different type
                return details::make_type_error<explicit_snapshot<Type>>();
            }
        });
    }
};

using regular_file = basic_explicit_file<file_type::regular>;
using directory = basic_explicit_file<file_type::directory>;
using block_device = basic_explicit_file<file_type::block_device>;
using character_device = basic_explicit_file<file_type::character_device>;
using fifo = basic_explicit_file<file_type::fifo>;
using symlink = basic_explicit_file<file_type::symlink>;
using socket = basic_explicit_file<file_type::socket>;
using unknown_file = basic_explicit_file<file_type::unknown>;

__UFS_NAMESPACE_END
