// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#if !defined(UTL_FILESYSTEM_FILE_HANDLE_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/expected/utl_expected.h"
#include "utl/filesystem/utl_path.h"

using PVOID = void*;
using HANDLE = PVOID;
using BOOL = int;

extern "C" BOOL CloseHandle(HANDLE);

__UFS_NAMESPACE_BEGIN

namespace io {
enum class open_flags : uint64_t;
}

namespace details {
template <size_t>
struct longptr_type;
template <>
struct longptr_type<4> {
    using type = long;
    static_assert(sizeof(type) == 4, "invalid implementation");
};
template <>
struct longptr_type<8> {
    using type = long long;
    static_assert(sizeof(type) == 8, "invalid implementation");
};
} // namespace details

class __UTL_ABI_PUBLIC file_handle {

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, CONST) static HANDLE invalid_handle() noexcept {
        using longptr = typename details::longptr_type<sizeof(HANDLE)>::type;
        static constexpr auto value = static_cast<longptr>(-1);
        return reinterpret_cast<HANDLE>(value);
    }
    class __UTL_ABI_PRIVATE construct {};

public:
    using native_handle_type = HANDLE;
    file_handle(file_handle const&) = delete;
    file_handle& operator=(file_handle const&) = delete;
    __UTL_HIDE_FROM_ABI inline constexpr file_handle() noexcept : handle{invalid_handle()} {}
    __UTL_HIDE_FROM_ABI inline constexpr file_handle(file_handle&& other) noexcept
        : handle{__UTL exchange(other.handle, invalid_handle())} {}
    __UTL_HIDE_FROM_ABI inline constexpr file_handle& operator=(file_handle&& other) noexcept {
        reset();
        handle = other.release();
        return *this;
    }

    __UTL_HIDE_FROM_ABI inline constexpr native_handle_type release() noexcept {
        return __UTL exchange(other.handle, invalid_handle());
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void reset() noexcept {
        if (is_open()) {
            ::CloseHandle(handle);
        }
    }

    __UTL_HIDE_FROM_ABI inline constexpr native_handle_type native_handle() const noexcept {
        return handle;
    }

    __UTL_HIDE_FROM_ABI explicit inline constexpr operator bool() const noexcept {
        return handle != invalid_handle();
    }

    __UTL_HIDE_FROM_ABI inline constexpr bool is_open() const noexcept {
        return static_cast<bool>(*this);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~file_handle() noexcept { reset(); }

private:
    __UTL_HIDE_FROM_ABI explicit file_handle(construct, native_handle_type handle) noexcept
        : handle{handle} {}
    native_handle_type handle;
};

__UFS_NAMESPACE_END
