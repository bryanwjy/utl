// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#ifndef UTL_MODULES
#  include "utl/utl_config.h"

#  if !UTL_COMPILER_GNU_BASED
#    error Invalid compiler
#  endif

#  include "utl/atomic/gnu/utl_memory_barriers.h"
#  include "utl/concepts/core.h"
#  include "utl/type_traits/constants.h"
#  include "utl/type_traits/metafunctions.h"
#  include "utl/type_traits/type_categories.h"
#  include "utl/type_traits/type_modifications.h"
#  include "utl/utility/core.h"
#endif

#include "utl/numeric/utl_sized_integral.h"

#include <limits.h>
#include <stdint.h>

UTL_NAMESPACE_BEGIN

namespace atomics {
template <typename T, size_t = (alignof(T) == sizeof(T)) * sizeof(T)>
struct interpreted_type;
template <typename T>
using interpreted_type_t UTL_NODEBUG = typename interpreted_type<T>::type;

static_assert(CHAR_BIT == 8, "Unsupported platform");

template <typename T>
struct interpreted_type<T, 1> {
    using type UTL_NODEBUG = uint8_t;
};

template <typename T>
struct interpreted_type<T, 2> {
    using type UTL_NODEBUG = uint16_t;
};

template <typename T>
struct interpreted_type<T, 4> {
    using type UTL_NODEBUG = uint32_t;
};

template <typename T>
struct interpreted_type<T, 8> {
    using type UTL_NODEBUG = uint64_t;
};

#if UTL_SUPPORTS_INT128
template <typename T>
struct interpreted_type<T, 16> {
    using type UTL_NODEBUG = __uint128_t;
};
#endif

template <typename T>
concept interpretable_type = (implicit_lifetime<remove_cv_t<T>> &&
    UTL_TRAIT_is_class(T) && requires { typename interpreted_type<T>::type; });

template <memory_order O>
struct fence_operations {
protected:
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~fence_operations() noexcept = default;

public:
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void
    thread_fence() noexcept {
        __atomic_thread_fence(order);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void
    signal_fence() noexcept {
        __atomic_signal_fence(order);
    }
};

template <memory_order O>
struct load_operations {
protected:
    template <typename T>
    using value_type = remove_cv_t<T>;
    static_assert(is_load_order<O>(), "Invalid order");
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~load_operations() noexcept = default;

public:
    template <native_atomic_type T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx) noexcept {
        return __atomic_load_n(ctx, order);
    }

    template <typename T>
    requires std::is_enum_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx) noexcept {
        value_type<T> out;
        __atomic_load(ctx, &out, order);
        return out;
    }

    template <std::same_as<bool> T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool load(
        T const* ctx) noexcept {
        return __atomic_load_n(ctx, order);
    }

    template <interpretable_type T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx) noexcept {
        alignas(T) unsigned char buffer[sizeof(T)];
        auto ptr = reinterpret_cast<value_type<T>*>(buffer);
        __atomic_load(ctx, ptr, order);
        return *reinterpret_cast<value_type<T>*>(
            ::memmove(ptr, ptr, sizeof(T)));
    }
};

template <memory_order O>
struct store_operations {
private:
    template <typename T>
    using value_type = remove_cv_t<T>;

protected:
    static_assert(is_store_order<O>(), "Invalid order");
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~store_operations() noexcept = default;

public:
    template <native_atomic_type T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value) noexcept {
        __atomic_store(ctx, UTL_BUILTIN_addressof(value), order);
    }

    template <typename T>
    requires std::is_enum_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value) noexcept {
        __atomic_store(ctx, UTL_BUILTIN_addressof(value), order);
    }

    template <std::same_as<bool> T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, bool value) noexcept {
        __atomic_store(ctx, UTL_BUILTIN_addressof(value), order);
    }

    template <interpretable_type T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> const& value) noexcept {
        alignas(T) unsigned char buffer[sizeof(T)];
        auto* ptr = reinterpret_cast<value_type<T>*>(
            __UTL_MEMCPY(buffer, UTL_BUILTIN_addressof(value), sizeof(T)));
        __atomic_store(ctx, ptr, order);
    }
};

template <memory_order O>
struct exchange_operations {
private:
    template <typename T>
    using value_type = remove_cv_t<T>;

protected:
    static constexpr int order = (int)O;
    UTL_CONSTEXPR_CXX20 ~exchange_operations() noexcept = default;

public:
    template <native_atomic_type T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value) noexcept {
        return __atomic_exchange_n(ctx, value, order);
    }

    template <std::same_as<bool> T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, bool value) noexcept {
        return __atomic_exchange_n(ctx, value, order);
    }

    template <typename T>
    requires std::is_enum_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value) noexcept {
        __atomic_exchange(ctx, UTL_BUILTIN_addressof(value),
            UTL_BUILTIN_addressof(value), order);
        return value;
    }

    template <interpretable_type T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> const& value) noexcept {
        alignas(T) unsigned char buffer[sizeof(T)];
        auto ptr = reinterpret_cast<value_type<T>*>(
            __UTL_MEMCPY(buffer, UTL_BUILTIN_addressof(value), sizeof(T)));
        __atomic_exchange(ctx, ptr, ptr, order);
        return *ptr;
    }

    template <integral T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> delta) noexcept {
        return __atomic_fetch_add(ctx, delta, order);
    }

    template <typename T>
    requires is_pointer_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, ptrdiff_t offset) noexcept {
        return __atomic_fetch_add(ctx, offset * sizeof(T), order);
    }

    template <integral T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_sub(
        T* ctx, value_type<T> delta) noexcept {
        return __atomic_fetch_sub(ctx, delta, order);
    }

    template <typename T>
    requires is_pointer_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_sub(
        T* ctx, ptrdiff_t offset) noexcept {
        return __atomic_fetch_sub(ctx, offset * sizeof(T), order);
    }

    template <integral T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> mask) noexcept {
        return __atomic_fetch_and(ctx, mask, order);
    }

    template <integral T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> mask) noexcept {
        return __atomic_fetch_or(ctx, mask, order);
    }

    template <integral T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> mask) noexcept {
        return __atomic_fetch_xor(ctx, mask, order);
    }
};

template <memory_order S, memory_order F>
struct compare_exchange_operations {
private:
    template <typename T>
    using value_type = remove_cv_t<T>;
    template <typename T>
    using pointer = value_type<T>*;

protected:
    static_assert(is_load_order<F>(), "Invalid order");
    static constexpr int success = (int)S;
    static constexpr int fail = (int)F;
    static constexpr bool strong = false;
    static constexpr bool weak = true;
    UTL_CONSTEXPR_CXX20 ~compare_exchange_operations() noexcept = default;

private:
    template <integral T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool
    compare_exchange_strong_(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(
            ctx, expected, value, strong, success, fail);
    }

    template <integral T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak_(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(
            ctx, expected, value, weak, success, fail);
    }

    template <typename T>
    requires is_pointer_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool
    compare_exchange_strong_(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(
            ctx, expected, value, strong, success, fail);
    }

    template <typename T>
    requires is_pointer_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak_(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange_n(
            ctx, expected, value, weak, success, fail);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool
    compare_exchange_strong_(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange(
            ctx, expected, UTL_BUILTIN_addressof(value), strong, success, fail);
    }

    template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak_(
        T* ctx, pointer<T> expected, value_type<T> value) noexcept {
        return __atomic_compare_exchange(
            ctx, expected, UTL_BUILTIN_addressof(value), weak, success, fail);
    }

    template <UTL_CONCEPT_CXX20(interpretable_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_atomic_interpretable(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool
    compare_exchange_strong_(
        T* ctx, pointer<T> expected, value_type<T> const& value) noexcept {
        alignas(T) unsigned char buffer[sizeof(T)];
        auto desired = reinterpret_cast<value_type<T>*>(
            __UTL_MEMCPY(buffer, UTL_BUILTIN_addressof(value), sizeof(T)));
        return __atomic_compare_exchange(
            ctx, expected, desired, strong, success, fail);
    }

    template <UTL_CONCEPT_CXX20(interpretable_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_atomic_interpretable(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange_weak_(
        T* ctx, pointer<T> expected, value_type<T> const& value) noexcept {
        alignas(T) unsigned char buffer[sizeof(T)];
        auto desired = reinterpret_cast<value_type<T>*>(
            __UTL_MEMCPY(buffer, UTL_BUILTIN_addressof(value), sizeof(T)));
        return __atomic_compare_exchange(
            ctx, expected, desired, weak, success, fail);
    }

public:
    template <typename T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto
    compare_exchange_strong(T* ctx, pointer<T> expected, value_type<T> value,
        compare_exchange_failure<F>)
        -> decltype(compare_exchange_strong_(ctx, expected, value)) {
        return compare_exchange_strong_(ctx, expected, value);
    }

    template <typename T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto compare_exchange_weak(
        T* ctx, pointer<T> expected, value_type<T> value,
        compare_exchange_failure<F>)
        -> decltype(compare_exchange_weak_(ctx, expected, value)) {
        return compare_exchange_weak_(ctx, expected, value);
    }
};
} // namespace atomics

UTL_NAMESPACE_END
