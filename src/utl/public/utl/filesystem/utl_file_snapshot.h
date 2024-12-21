// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/exception/utl_system_exception.h"
#include "utl/filesystem/utl_file.h"
#include "utl/utility/utl_to_underlying.h"

__UFS_NAMESPACE_BEGIN

template <typename Alloc = __UTL allocator<path_char>>
class __UTL_PUBLIC_TEMPLATE basic_file_snapshot : public basic_file<Alloc> {
    using base_type = basic_file<Alloc>;
    using allocator_type = Alloc;
    using path_container = basic_string<path_char, Alloc>;
    using time_type = tempus::time_point<steady_clock_t>;
    using view_type = basic_string_view<path_char>;

    using base_type::status;
    using base_type::to_snapshot;

public:
    __UTL_HIDE_FROM_ABI inline basic_file_snapshot() = delete;
    template <typename Arg UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(base_type, Arg))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Arg))
    __UTL_HIDE_FROM_ABI inline basic_file_snapshot(Arg&& arg, file_status const& status,
        time_type time) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, Arg))
        : base_type{__UTL forward<Arg>(arg)}
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

    template<typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline basic_file_snapshot(Args&&... args) UTL_THROWS
        : base_type{__UTL forward<Args>(args)...},
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot(
        basic_file_snapshot&& other, allocator_type const& a) UTL_THROWS
        : basic_file_snapshot{static_cast<base_type&&>(__UTL move(other)), a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_file_snapshot(
        basic_file_snapshot const& other, allocator_type const& a) UTL_THROWS
        : basic_file_snapshot{static_cast<base_type const&>(other), a} {}

    __UTL_HIDE_FROM_ABI inline constexpr operator file_view() const noexcept UTL_LIFETIMEBOUND {
        return file_view{path()};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator file_view_snapshot() const noexcept UTL_LIFETIMEBOUND {
        return file_view_snapshot{path(), status(), time()};
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
            status_ = status;
            time_ = get_time(file_clock);
            return result<void>{};
        });
    }

private:
    file_status status_;
    time_type time_;
};

template <file_type Type, typename Alloc = __UTL allocator<path_char>>
class __UTL_PUBLIC_TEMPLATE basic_explicit_file_snapshot : public basic_explicit_file<Type, Alloc> {
    using allocator_type = Alloc;
    using base_type = basic_explicit_file<Type, allocator_type>;
    using generic_file_type = basic_file<allocator_type>;
    using generic_snapshot_type = basic_file_snapshot<allocator_type>;
    using time_type = tempus::time_point<steady_clock_t>;
    static_assert(
        __UTL to_underlying(Type) < __UTL to_underlying(file_type::invalid), "Invalid file type");

    struct private_tag {};

    __UTL_HIDE_FROM_ABI inline constexpr basic_explicit_file_snapshot(private_tag,
        explicit_file_view_snapshot<Type> const& other, allocator_type const& a) UTL_THROWS
        : base_type{other, a}
        , status_{other.status()}
        , time_{other.time()} {}

    __UTL_HIDE_FROM_ABI inline constexpr basic_explicit_file_snapshot(
        private_tag, file_view_snapshot const& other, allocator_type const& a) UTL_THROWS
        : base_type{other, a}
        , status_{other.status()}
        , time_{other.time()} {
        UTL_ASSERT(status().type == Type);
    }

    using base_type::status;
    using base_type::to_snapshot;

public:
    template <typename Arg UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(base_type, Arg))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Arg))
    __UTL_HIDE_FROM_ABI inline basic_explicit_file_snapshot(Arg&& arg, file_status const& status,
        time_type time) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, Arg))
        : base_type{__UTL forward<Arg>(arg)}
        , status_{status}
        , time_{time} {}

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
        basic_explicit_file_snapshot&& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, base_type,
        allocator_type const&))
        : base_type{static_cast<base_type&&>(__UTL move(other)), a}
        , status_{other.status()}
        , time_{other.time()} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        basic_explicit_file_snapshot const& other,
        allocator_type const& a) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type, base_type,
        allocator_type const&))
        : base_type{static_cast<base_type const&>(other), a}
        , status_{other.status()}
        , time_{other.time()} {}

    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        generic_snapshot_type&& other,
        allocator_type const& a =
            allocator_type{}) noexcept(UTL_TRAIT_is_nothrow_constructible(base_type,
        generic_snapshot_type, allocator_type const&))
        : base_type{static_cast<generic_file_type&&>(__UTL move(other)), a}
        , status_{other.status()}
        , time_{other.time()} {
        UTL_ASSERT(status().type == Type);
    }

    template <typename View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, explicit_file_view_snapshot<Type>))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_convertible(View, explicit_file_view_snapshot<Type>))
    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        View const& other, allocator_type const& a = allocator_type{})
        : base_type{private_tag{}, static_cast<explicit_file_view_snapshot<Type>>(other), a} {}

    template <typename View UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_convertible(View, file_view_snapshot))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_convertible(View, file_view_snapshot))
    __UTL_HIDE_FROM_ABI explicit inline UTL_CONSTEXPR_CXX14 basic_explicit_file_snapshot(
        View const& other, allocator_type const& a = allocator_type{})
        : base_type{private_tag{}, static_cast<file_view_snapshot>(other), a} {}

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_explicit_file_snapshot() noexcept = default;

    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U> const&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U>&&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U> const&, allocator_type const&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file<U>&&, allocator_type const&) = delete;

    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file_snapshot<U> const&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file_snapshot<U>&&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(
        basic_explicit_file_snapshot<U> const&, allocator_type const&) = delete;
    template <file_type U>
    basic_explicit_file_snapshot(basic_explicit_file_snapshot<U>&&, allocator_type const&) = delete;

    template <typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_constructible(base_type, Args...))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_constructible(base_type, Args...))
    __UTL_HIDE_FROM_ABI explicit inline basic_explicit_file_snapshot(Args&&... args) UTL_THROWS
        : base_type{__UTL forward<Args>(args)...}
        , status_{base_type::status().value()}
        , time_{get_time(file_clock)} {
        UTL_THROW_IF(status().type != Type,
            __UTL error_code_exception(
                __UTL error_code{error_value::file_type_mismatch}, "file type mismatch"));
    }

    __UTL_HIDE_FROM_ABI explicit inline constexpr operator file_view() const noexcept UTL_LIFETIMEBOUND {
        return file_view{path()};
    }

    __UTL_HIDE_FROM_ABI explicit inline constexpr operator file_view_snapshot() const noexcept UTL_LIFETIMEBOUND {
        return file_view_snapshot{path(), status(), time()};
    }

    __UTL_HIDE_FROM_ABI explicit inline constexpr operator generic_snapshot_type() const& {
        return generic_snapshot_type{static_cast<generic_file_type const&>(*this), status_, time_};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator explicit_file_view<Type>() const noexcept UTL_LIFETIMEBOUND {
        return explicit_file_view<Type>{path()};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator explicit_file_view_snapshot<Type>() const noexcept UTL_LIFETIMEBOUND {
        return explicit_file_view_snapshot<Type>{path(), status(), time()};
    }

    __UTL_HIDE_FROM_ABI inline constexpr operator generic_snapshot_type() && noexcept(
        UTL_TRAIT_is_nothrow_constructible(
            generic_snapshot_type, generic_file_type, file_status const&, time_point const&)) {
        return generic_snapshot_type{static_cast<generic_file_type&&>(*this), status_, time_};
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
                time_ = get_time(file_clock);
                return result<void>{};
            } else {
                return return result<void>{__UTL unexpect, error_value::file_type_mismatch};
            }
        });
    }

private:
    file_status status_;
    time_type time_;
};

using file_snapshot = basic_file_snapshot<>;
using regular_file_snapshot = basic_explicit_file_snapshot<file_type::regular>;
using directory_snapshot = basic_explicit_file_snapshot<file_type::directory>;
using block_device_snapshot = basic_explicit_file_snapshot<file_type::block_device>;
using character_device_snapshot = basic_explicit_file_snapshot<file_type::character_device>;
using fifo_snapshot = basic_explicit_file_snapshot<file_type::fifo>;
using symlink_snapshot = basic_explicit_file_snapshot<file_type::symlink>;
using socket_snapshot = basic_explicit_file_snapshot<file_type::socket>;
using unknown_file_snapshot = basic_explicit_file_snapshot<file_type::unknown>;

__UFS_NAMESPACE_END
