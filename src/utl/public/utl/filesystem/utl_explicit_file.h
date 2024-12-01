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
class UTL_PUBLIC_TEMPLATE basic_explicit_file : public basic_file<Alloc> {
    using base_type = basic_file<Alloc>;
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::invalid), "Invalid file type");

public:
    using basic_file<Alloc>::basic_file;
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
};

__UFS_NAMESPACE_END
