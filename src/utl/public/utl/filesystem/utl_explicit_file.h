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
class UTL_PUBLIC_TEMPLATE basic_explicit_file : private basic_file<Alloc> {
    using base_type = basic_file<Alloc>;
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
};

__UFS_NAMESPACE_END
