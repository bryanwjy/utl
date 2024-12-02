// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_platform.h"

__UFS_NAMESPACE_BEGIN

template <typename Alloc = __UTL allocator<path_char>>
class UTL_PUBLIC_TEMPLATE basic_file_snapshot : private basic_file<Alloc> {
    using base_type = basic_file<Alloc>;
    using allocator_type = Alloc;
    using path_container = basic_string<path_char, Alloc>;
    using time_type = time_point<file_clock_t>;
    using view_type = basic_string_view<path_char>;

public:
    __UTL_HIDE_FROM_ABI inline basic_file_snapshot() = delete;
    __UTL_HIDE_FROM_ABI inline basic_file_snapshot(path_container&& path, file_status const& status,
        time_type time) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, path_container))
        : base_type{__UTL move(path)}
        , status_{status}
        , time_{time} {}
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot(basic_file_snapshot const&) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot(basic_file_snapshot&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot&
    operator=(basic_file_snapshot const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot& operator=(
        basic_file_snapshot&&) noexcept(UTL_TRAIT_is_nothrow_move_assignable(base_type)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_file_snapshot() noexcept = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot(
        basic_file_snapshot const& other, allocator_type const& a)
        : base_type{static_cast<base_type const&>(other), a}
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot(basic_file_snapshot&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, base_type,
        allocator_type const&))
        : base_type{static_cast<base_type&&>(other), a}
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    template<typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline basic_file_snapshot(Args&&... args)
        : base_type{__UTL forward<Args>(args)...},
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type const&() const& { return *this; }

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type&&() && { return __UTL move(*this); }

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
