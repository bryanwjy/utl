// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/exception/utl_system_exception.h"
#include "utl/filesystem/utl_file_view.h"
#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

class __UTL_PUBLIC_TEMPLATE file_view_snapshot : private file_view {
    using base_type = file_view;
    using time_type = tempus::time_point<steady_clock_t>;
    using view_type = basic_string_view<path_char>;

public:
    __UTL_HIDE_FROM_ABI inline file_view_snapshot() = delete;
    __UTL_HIDE_FROM_ABI inline constexpr file_view_snapshot(
        view_type path, file_status const& status, time_type time) noexcept
        : base_type{path}
        , status_{status}
        , time_{time} {}
    __UTL_HIDE_FROM_ABI inline constexpr file_view_snapshot(
        base_type const& file, file_status const& status, time_type time) noexcept
        : base_type{file}
        , status_{status}
        , time_{time} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view_snapshot(
        file_view_snapshot const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view_snapshot(
        file_view_snapshot&&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view_snapshot& operator=(
        file_view_snapshot const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 file_view_snapshot& operator=(
        file_view_snapshot&&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~file_view_snapshot() noexcept = default;

    template<typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline file_view_snapshot(Args&&... args) UTL_THROWS
        : base_type{__UTL forward<Args>(args)...},
        , status_{base_type::status().value()}
        , time_{get_time(steady_clock)} {}

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type() const noexcept { return *this; }

    using base_type::path;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 file_status const& status() const noexcept {
        return status_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 time_type time() const noexcept {
        return time_;
    }

    __UTL_HIDE_FROM_ABI inline result<void> refresh_status() noexcept {
        return base_type::status().and_then([this](file_status const& status) {
            status_ = status;
            time_ = get_time(steady_clock);
            return result<void>{};
        });
    }

private:
    file_status status_;
    time_type time_;
};

template <file_type Type>
class __UTL_PUBLIC_TEMPLATE explicit_file_view_snapshot : private explicit_file_view<Type> {
    using base_type = explicit_file_view<Type>;
    using time_type = tempus::time_point<steady_clock_t>;
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::invalid), "Invalid file type");

public:
    __UTL_HIDE_FROM_ABI inline explicit_file_view_snapshot(
        view_type path, file_status const& status, time_type time) noexcept
        : base_type{path}
        , status_{status}
        , time_{time} {
        UTL_ASSERT(status().type == Type);
    }

    __UTL_HIDE_FROM_ABI inline explicit_file_view_snapshot(
        file_view const& file, file_status const& status, time_type time) noexcept
        : base_type{file}
        , status_{status}
        , time_{time} {
        UTL_ASSERT(status().type == Type);
    }

    __UTL_HIDE_FROM_ABI inline explicit_file_view_snapshot(
        base_type const& file, file_status const& status, time_type time) noexcept
        : base_type{file}
        , status_{status}
        , time_{time} {
        UTL_ASSERT(status().type == Type);
    }

    __UTL_HIDE_FROM_ABI inline constexpr explicit_file_view_snapshot() = delete;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view_snapshot(
        explicit_file_view_snapshot const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view_snapshot(
        explicit_file_view_snapshot&&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view_snapshot& operator=(
        explicit_file_view_snapshot const&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 explicit_file_view_snapshot& operator=(
        explicit_file_view_snapshot&&) noexcept = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~explicit_file_view_snapshot() noexcept = default;

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 explicit_file_view_snapshot(
        file_view_snapshot const& other) noexcept
        : base_type{static_cast<file_view>(other)}
        , status_{other.status()}
        , time_{other.time()} {
        UTL_ASSERT(status().type == Type);
    }

    template <file_type U>
    explicit_file_view_snapshot(basic_explicit_file<U> const&) = delete;
    template <file_type U>
    explicit_file_view_snapshot(basic_explicit_file<U>&&) = delete;
    template <file_type U>
    explicit_file_view_snapshot(basic_explicit_file<U> const&, allocator_type const&) = delete;
    template <file_type U>
    explicit_file_view_snapshot(basic_explicit_file<U>&&, allocator_type const&) = delete;

    template <file_type U>
    explicit_file_view_snapshot(explicit_file_view_snapshot<U> const&) = delete;
    template <file_type U>
    explicit_file_view_snapshot(explicit_file_view_snapshot<U>&&) = delete;
    template <file_type U>
    explicit_file_view_snapshot(
        explicit_file_view_snapshot<U> const&, allocator_type const&) = delete;
    template <file_type U>
    explicit_file_view_snapshot(explicit_file_view_snapshot<U>&&, allocator_type const&) = delete;

    template <typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_constructible(base_type, Args...) &&
        UTL_TRAIT_is_explicit_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline explicit_file_view_snapshot(Args&&... args) UTL_THROWS
        : base_type{__UTL forward<Args>(args)...}
        , status_{base_type::status().value()}
        , time_{get_time(steady_clock)} {
        UTL_THROW_IF(status().type != Type,
            __UTL error_code_exception(
                __UTL error_code{fs_errc::file_type_mismatch}, "file type mismatch"));
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator base_type() const noexcept { return *this; }

    __UTL_HIDE_FROM_ABI inline constexpr operator file_view() const noexcept {
        return static_cast<file_view>(static_cast<base_type const&>(*this));
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator file_view_snapshot() const noexcept {
        return file_view_snapshot{static_cast<file_view const&>(*this), status_, time_};
    }

    using base_type::path;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 file_status const& status() const noexcept {
        return status_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) inline UTL_CONSTEXPR_CXX14 time_type time() const noexcept {
        return time_;
    }

    __UTL_HIDE_FROM_ABI inline result<void> refresh_status() noexcept {
        return base_type::status().and_then([this](file_status const& status) {
            if (status.type == Type) {
                status_ = status;
                time_ = get_time(steady_clock);
                return result<void>{};
            } else {
                return details::make_error<fs_errc::file_type_mismatch, void>();
            }
        });
    }

private:
    file_status status_;
    time_type time_;
};

namespace views {
using file_snapshot = file_view_snapshot;
using regular_file_snapshot = explicit_file_view_snapshot<file_type::regular>;
using directory_snapshot = explicit_file_view_snapshot<file_type::directory>;
using block_device_snapshot = explicit_file_view_snapshot<file_type::block_device>;
using character_device_snapshot = explicit_file_view_snapshot<file_type::character_device>;
using fifo_snapshot = explicit_file_view_snapshot<file_type::fifo>;
using symlink_snapshot = explicit_file_view_snapshot<file_type::symlink>;
using socket_snapshot = explicit_file_view_snapshot<file_type::socket>;
using unknown_file_snapshot = explicit_file_view_snapshot<file_type::unknown>;
} // namespace views

__UFS_NAMESPACE_END
