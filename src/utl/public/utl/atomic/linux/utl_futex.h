// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_FUTEX_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_TARGET_LINUX
#  error Invalid Target
#endif // UTL_TARGET_LINUX

#include "utl/memory/utl_addressof.h"
#include "utl/system_error/utl_errc.h"
#include "utl/system_error/utl_error_category.h"
#include "utl/system_error/utl_error_code.h"
#include "utl/tempus/utl_duration.h"
#include "utl/type_traits/utl_is_trivially_copyable.h"

#include <errno.h>
#include <linux/futex.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <unistd.h>

UTL_NAMESPACE_BEGIN

namespace futex {

#if UTL_CXX20

template <typename T>
concept waitable_type =
    (sizeof(T) == 4 && alignof(T) == 4 && UTL_TRAIT_is_trivially_copyable(T) &&
        !UTL_TRAIT_is_const(T) && !UTL_TRAIT_is_volatile(T));
template <typename T>
struct is_waitable : __UTL bool_constant<waitable_type<T>> {};
template <typename T>
inline constexpr bool is_waitable_v = waitable_type<T>;
#  define UTL_TRAIT_is_futex_waitable(TYPE) __UTL futex::waitable_type<TYPE>

#else // UTL_CXX20

namespace details {
template <typename T>
auto waitable_impl(float) noexcept -> __UTL false_type;
template <typename T>
auto waitable_impl(int) noexcept -> __UTL bool_constant<sizeof(T) == 4 &&
    alignof(T) == 4 && UTL_TRAIT_is_trivially_copyable(T) &&
    !UTL_TRAIT_is_const(T) && !UTL_TRAIT_is_volatile(T)>;
template <typename T>
using waitable UTL_NODEBUG = decltype(waitable_impl<T>(0));
} // namespace details

template <typename T>
struct is_waitable : details::waitable<T> {};

#  if UTL_CXX14
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_waitable_v =
    __UTL futex::details::waitable<T>::value;
#  endif // UTL_CXX14
#  define UTL_TRAIT_is_futex_waitable(TYPE) \
      __UTL futex::details::waitable<TYPE>::value
#endif // UTL_CXX20

template <waitable_type T>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline error_code wait(
    T* address, T const& value,
    __UTL tempus::duration t) noexcept {

    if (t == __UTL tempus::duration::zero()) {
        return make_error_code(errc::timed_out);
    }

    static constexpr uint32_t op = FUTEX_WAIT_PRIVATE;
    auto const timeout = static_cast<timespec>(t);
    auto const timeout_ptr = !t ? nullptr : &timeout;

    uint32_t readable_value = 0;
    __UTL_MEMCPY(&readable_value, __UTL addressof(value), sizeof(value));
    if (!syscall(SYS_futex, reinterpret_cast<uint32_t*>(address), op,
            readable_value, timeout_ptr)) {
        return error_code{};
    }

    auto const error = errno;
    if (error == EAGAIN) {
        return error_code{};
    }

    return error_code{error, system_category()};
}

template <waitable_type T>
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline void notify_one(T* address) noexcept {
    static constexpr uint32_t op = FUTEX_WAKE_PRIVATE;
    static constexpr int32_t wake_one = 1;
    syscall(SYS_futex, reinterpret_cast<uint32_t*>(address), op, wake_one);
}

template <waitable_type T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_futex_waitable(T))>
UTL_ATTRIBUTE(_HIDE_FROM_ABI) inline void notify_all(T* address) noexcept {
    static constexpr uint32_t op = FUTEX_WAKE_PRIVATE;
    static constexpr int32_t wake_all = INT_MAX;
    syscall(SYS_futex, reinterpret_cast<uint32_t*>(address), op, wake_all);
}

#undef UTL_TRAIT_is_futex_waitable
} // namespace futex

UTL_NAMESPACE_END
