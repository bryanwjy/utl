// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#if !defined(UTL_FILESYSTEM_FILE_HANDLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/expected/utl_expected.h"
#include "utl/filesystem/utl_path.h"
#include "utl/io/utl_access.h"

extern "C" int close(int);

__UFS_NAMESPACE_BEGIN

namespace details {
enum file_descriptor_t : int {
    invalid = -1
};
namespace file_handle {
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) file_descriptor_t duplicate(file_descriptor_t) noexcept;
}
} // namespace details

template <io::access A>
class __UTL_PUBLIC_TEMLATE basic_file_handle {
    template <io::access A>
    friend basic_file_handle;

public:
    using native_handle_type = details::file_descriptor_t;

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend inline constexpr basic_file_handle duplicate(
        basic_file_handle const& other) noexcept {
        if (other) {
            return basic_file_handle{details::file_handle::duplicate(other.native_handle())};
        }

        return {};
    }

    __UTL_HIDE_FROM_ABI inline constexpr basic_file_handle() noexcept
        : handle_{native_handle_type::invalid} {}
    __UTL_HIDE_FROM_ABI explicit basic_file_handle(native_handle_type handle) noexcept
        : handle_{handle} {}

    basic_file_handle(basic_file_handle const&) = delete;
    basic_file_handle& operator=(basic_file_handle const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr basic_file_handle(basic_file_handle&& other) noexcept
        : handle_{__UTL exchange(other.handle_, native_handle_type::invalid)} {}

    __UTL_HIDE_FROM_ABI inline constexpr basic_file_handle& operator=(
        basic_file_handle&& other) noexcept {
        reset();
        handle_ = other.release();
        return *this;
    }

    template <io::access O UTL_CONSTRAINT_CXX11((O & A) == A)>
    UTL_CONSTRAINT_CXX20((O & A) == A)
    __UTL_HIDE_FROM_ABI inline constexpr basic_file_handle(basic_file_handle<O>&& other) noexcept
        : handle_{__UTL exchange(other.handle_, native_handle_type::invalid)} {}

    template <io::access O UTL_CONSTRAINT_CXX11((O & A) == A)>
    UTL_CONSTRAINT_CXX20((O & A) == A)
    __UTL_HIDE_FROM_ABI inline constexpr basic_file_handle& operator=(
        basic_file_handle<O>&& other) noexcept {
        reset();
        handle_ = other.release();
        return *this;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr native_handle_type release() noexcept {
        return __UTL exchange(other.handle_, native_handle_type::invalid);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void reset() noexcept {
        auto const old = release();
        if (old != native_handle_type::invalid) {
            ::close(old);
        }
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr native_handle_type
    native_handle() const noexcept {
        return handle_;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) explicit inline constexpr operator bool() const noexcept {
        return handle_ != native_handle_type::invalid;
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr bool is_open() const noexcept {
        return static_cast<bool>(*this);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~basic_file_handle() noexcept { reset(); }

private:
    native_handle_type handle_;
};

__UFS_NAMESPACE_END
