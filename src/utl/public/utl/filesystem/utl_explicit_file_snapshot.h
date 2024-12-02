// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_file.h"
#include "utl/filesystem/utl_file_error.h"
#include "utl/filesystem/utl_file_status.h"
#include "utl/filesystem/utl_file_type.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

template <file_type Type, typename Alloc = __UTL allocator<path_char>>
class UTL_PUBLIC_TEMPLATE basic_explicit_file_snapshot : private basic_explicit_file<Type, Alloc> {
    using base_type = basic_explicit_file<Type, Alloc>;
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::invalid), "Invalid file type");

public:
    __UTL_HIDE_FROM_ABI inline basic_explicit_file_snapshot(path_container&& path,
        file_status const& status,
        time_type time) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, path_container))
        : base_type{__UTL move(path), status, time} {
        UTL_ASSERT(status().type == Type);
    }
    __UTL_HIDE_FROM_ABI inline constexpr basic_explicit_file_snapshot() = delete;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_explicit_file_snapshot const&) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_explicit_file_snapshot&&) noexcept(UTL_TRAIT_is_nothrow_move_constructible(base_type)) =
        default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot& operator=(
        basic_explicit_file_snapshot const&) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot&
    operator=(basic_explicit_file_snapshot&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(base_type)) = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_explicit_file_snapshot const& other, allocator_type const& a)
        : base_type{static_cast<base_type const&>(other), a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_explicit_file_snapshot&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, base_type,
        allocator_type const&))
        : base_type{static_cast<base_type&&>(other), a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_explicit_file_snapshot() noexcept = default;

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_file_snapshot<allocator_type> const& other) UTL_THROWS
        : base_type{(basic_file<allocator_type> const&)other}
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_file_snapshot<allocator_type>&&
            other) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type,
        basic_file_snapshot<allocator_type>))
        : base_type{(basic_file<allocator_type>&&)other}
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_file_snapshot<allocator_type> const& other, allocator_type const& a) UTL_THROWS
        : base_type{(basic_file<allocator_type> const&)other, a}
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_file_snapshot<allocator_type>&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type,
        basic_file_snapshot<allocator_type>, allocator_type const&))
        : base_type{(basic_file<allocator_type>&&)other, a}
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U> const&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U>&&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U> const&, allocator_type const&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U>&&, allocator_type const&) = delete;

    template <typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_constructible(base_type, Args...) &&
        UTL_TRAIT_is_explicit_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline basic_explicit_file_snapshot(Args&&... args)
        : base_type{__UTL forward<Args>(args)...} {
        UTL_ASSERT(base_type::status().type != Type);
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type const&() const& { return *this; }

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type&&() && { return __UTL move(*this); }

    __UTL_HIDE_FROM_ABI inline constexpr operator basic_file<allocator_type> const&() const& {
        return static_cast<base_type const&>(*this);
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator basic_file<allocator_type>&&() && {
        return static_cast<base_type&&>(*this);
    }

    using base_type::path;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 file_status const& status() const noexcept {
        return status_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 time_point<file_clock>
    time() const noexcept {
        return time_;
    }

private:
    file_status status_;
    time_point<file_clock> time_;
};

__UFS_NAMESPACE_END
