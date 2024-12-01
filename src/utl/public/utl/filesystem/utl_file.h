// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_file_error.h"
#include "utl/filesystem/utl_file_status.h"
#include "utl/filesystem/utl_file_type.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/memory/utl_allocator.h"
#include "utl/string/utl_basic_short_string.h"
#include "utl/string/utl_basic_string_view.h"
#include "utl/string/utl_is_string_char.h"
#include "utl/string/utl_libc.h"
#include "utl/tempus/utl_libc.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/utility/utl_compressed_pair.h"

__UFS_NAMESPACE_BEGIN

template <typename Alloc = __UTL allocator<path_char>>
class UTL_PUBLIC_TEMPLATE basic_file {
    using allocator_type = Alloc;
    using path_container = basic_string<path_char, allocator_type>;
    template <file_type Type>
    using explicit_file = basic_explicit_file<Type, allocator_type>;
    using view_type = basic_string_view<path_char>;

    template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_base_of(basic_file, remove_cvref_t<T>))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_base_of(basic_file, remove_cvref_t<T>))
    __UTL_HIDE_FROM_ABI static inline constexpr auto get_path(T&& t) noexcept
        -> decltype(__UTL forward_like<T>(__UTL declval<path_container&>())) {
        return __UTL forward_like<T>(((copy_cv_t<remove_reference_t<T>, basic_file>&)t).path_);
    }

public:
    __UTL_HIDE_FROM_ABI inline basic_file() = delete;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file& operator=(basic_file const& other) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file& operator=(basic_file&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(path_container)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_file() noexcept = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file&& other) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(path_container)) = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(
        basic_file const& other, allocator_type const& a) UTL_THROWS
        : path_{other.path_, a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(basic_file&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(path_container,
        path_container, allocator_type const&))
        : path_{__UTL move(other.path_), a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(path_container&& path) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(path_container))
        : path_{__UTL move(path)} {}

    template <UTL_CONCEPT_CXX20(convertible_to<view_type>)... Vs UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_conjunction(
            bool_constant<(sizeof...(Vs) > 0)>, is_convertible<Vs, view_type>...))>
    UTL_CONSTRAINT_CXX20(sizeof...(Vs) > 0)
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file(
        explicit_file<file_type::directory>&& dir, Vs&&... tail)
        : path_{__UFS path::join(get_path(__UTL move(dir)), __UTL forward<Vs>(tail)...)} {}

    __UTL_HIDE_FROM_ABI
    explicit inline UTL_CONSTEXPR_CXX14 basic_file(allocator_type const& alloc) noexcept(
        UTL_TRAIT_is_nothrow_constructible(path_container, allocator_type const&))
        : path_{alloc} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        view_type view, allocator_type const& a = allocator_type{}) UTL_THROWS
        : path_{view, a} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_file(
        path_char const* path, allocator_type const& a = allocator_type{}) UTL_THROWS
        : basic_file{view_type(path), a} {}

#if UTL_SUPPORTS_CHAR8_T
    template <UTL_CONCEPT_CXX20(string_char) Char UTL_CONSTRAINT_CXX11(
        sizeof(Char) == sizeof(path_char))>
    UTL_CONSTRAINT_CXX20(sizeof(Char) == sizeof(path_char))
    __UTL_HIDE_FROM_ABI explicit inline basic_file(
        Char const* bytes, allocator_type const& a = allocator_type{}) UTL_THROWS
        : basic_file{view_type(reinterpret_cast<path_char*>(bytes)), a} {}
#endif

    __UTL_HIDE_FROM_ABI inline constexpr view_type path() const noexcept {
        return view_type{path_.data(), path_.size()};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator view_type() const noexcept {
        return view_type{path_.data(), path_.size()};
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 view_type basename() const noexcept {
        return __UFS path::basename(path());
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 view_type basename(
        view_type suffix_to_remove) const noexcept {
        return __UFS path::basename(path(), suffix_to_remove);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 view_type extension() const noexcept {
        return __UFS path::extension(path());
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 view_type dirname() const noexcept {
        return __UFS path::dirname(path());
    }

    __UTL_HIDE_FROM_ABI inline __UTL expected<file_status, file_error> status() const noexcept {
        return __UFS details::stat(path());
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file<file_type::directory>
    parent_directory() const& noexcept {
        return explicit_file<file_type::directory>{dirname()};
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file<file_type::directory>
    parent_directory() && noexcept {
        auto const dir = dirname();
        if (path_.starts_with(dir)) {
            return explicit_file<file_type::directory>{__UTL move(path_).substr(0, dir.size())};
        }

        // dir is "."
        path_.clear();
        path_ = dir;
        return explicit_file<file_type::directory>{__UTL move(path_)};
    }

private:
    path_container path_;
};

__UFS_NAMESPACE_END
