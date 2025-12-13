// Copyright 2023-2024 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/concepts/core.h"
#  include "utl/config.h"
#  include "utl/type_traits/constants.h"
#  include "utl/type_traits/metafunctions.h"
#  include "utl/type_traits/type_categories.h"
#  include "utl/type_traits/type_modifications.h"
#  include "utl/utility/core.h"
#endif
/**
 * UTL Atomic
 * Current support:
 * * GNU based compilers that rely on the GNU atomic intrinsics
 * * MSVC using the Interlock intrinsics
 *
 * * Class types must be trivially destructible and either trivially copyable or
 * trivially movable
 * * If data type is a class with padding bits, it is the user's responsibility
 * to zero them out
 */

#if UTL_COMPILER_GNU_BASED

#  if !defined(__ATOMIC_RELAXED) || !defined(__ATOMIC_CONSUME) || \
      !defined(__ATOMIC_ACQUIRE) || !defined(__ATOMIC_RELEASE) || \
      !defined(__ATOMIC_ACQ_REL) || !defined(__ATOMIC_SEQ_CST)
#    error Undefined atomic semantics
#  endif

UTL_NAMESPACE_BEGIN
enum class memory_order : int {
    relaxed = __ATOMIC_RELAXED,
    consume = __ATOMIC_CONSUME,
    acquire = __ATOMIC_ACQUIRE,
    release = __ATOMIC_RELEASE,
    acq_rel = __ATOMIC_ACQ_REL,
    seq_cst = __ATOMIC_SEQ_CST,
};
UTL_NAMESPACE_END
#else // UTL_COMPILER_GNU_BASED

UTL_NAMESPACE_BEGIN
enum class memory_order : int {
    relaxed,
    consume,
    acquire,
    release,
    acq_rel,
    seq_cst,
};
UTL_NAMESPACE_END

#endif // UTL_COMPILER_GNU_BASED

UTL_NAMESPACE_BEGIN

template <memory_order O>
using memory_order_type = value_constant<memory_order, O>;

inline constexpr memory_order_type<memory_order::relaxed>
    memory_order_relaxed{};
inline constexpr memory_order_type<memory_order::acquire>
    memory_order_acquire{};
inline constexpr memory_order_type<memory_order::consume>
    memory_order_consume{};
inline constexpr memory_order_type<memory_order::release>
    memory_order_release{};
inline constexpr memory_order_type<memory_order::acq_rel>
    memory_order_acq_rel{};
inline constexpr memory_order_type<memory_order::seq_cst>
    memory_order_seq_cst{};

template <memory_order>
struct atomic_operations;
using atomic_relaxed = atomic_operations<memory_order::relaxed>;
using atomic_consume = atomic_operations<memory_order::consume>;
using atomic_acquire = atomic_operations<memory_order::acquire>;
using atomic_release = atomic_operations<memory_order::release>;
using atomic_acq_rel = atomic_operations<memory_order::acq_rel>;
using atomic_seq_cst = atomic_operations<memory_order::seq_cst>;

template <typename T>
concept native_atomic_type =
    (arithmetic_type<T> && !std::same_as<bool, std::remove_cv_t<T>>) ||
    is_pointer_v<T>;

/**
 * It's not supported on any compilers anyway
 */
template <typename T>
T kill_dependency(T v) noexcept {
    return v;
}

namespace atomics {
template <memory_order O>
using compare_exchange_failure = memory_order_type<O>;
using relaxed_failure_t = compare_exchange_failure<memory_order::relaxed>;
using acquire_failure_t = compare_exchange_failure<memory_order::acquire>;
using consume_failure_t = compare_exchange_failure<memory_order::consume>;
using seq_cst_failure_t = compare_exchange_failure<memory_order::seq_cst>;
inline constexpr relaxed_failure_t relaxed_failure{};
inline constexpr acquire_failure_t acquire_failure{};
inline constexpr consume_failure_t consume_failure{};
inline constexpr seq_cst_failure_t seq_cst_failure{};

template <memory_order O>
consteval bool is_load_order() noexcept {
    return O != memory_order::release || O != memory_order::acq_rel;
}

template <memory_order O>
consteval bool is_store_order() noexcept {
    return O != memory_order::consume || O != memory_order::acquire ||
        O != memory_order::acq_rel;
}
} // namespace atomics

UTL_NAMESPACE_END
#define UTL_ATOMIC_PRIVATE_HEADER_GUARD
#if UTL_COMPILER_GNU_BASED
#  include "utl/atomic/gnu/utl_atomic.h"
#elif UTL_COMPILER_MSVC
#  include "utl/atomic/winapi/utl_atomic.h"
#else
#  error Unsupported platform
#endif
#undef UTL_ATOMIC_PRIVATE_HEADER_GUARD
UTL_NAMESPACE_BEGIN

template <>
struct atomic_operations<memory_order::relaxed> :
    atomics::load_operations<memory_order::relaxed>,
    atomics::store_operations<memory_order::relaxed>,
    atomics::exchange_operations<memory_order::relaxed>,
    atomics::fence_operations<memory_order::relaxed>,
    atomics::compare_exchange_operations<memory_order::relaxed,
        memory_order::relaxed> {
    using atomics::compare_exchange_operations<memory_order::relaxed,
        memory_order::relaxed>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::relaxed,
        memory_order::relaxed>::compare_exchange_weak;
};

template <>
struct atomic_operations<memory_order::acquire> :
    atomics::load_operations<memory_order::acquire>,
    atomics::exchange_operations<memory_order::acquire>,
    atomics::fence_operations<memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::relaxed> {
    using atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::acquire>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::consume>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::relaxed>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::acquire>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::consume>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::acquire,
        memory_order::relaxed>::compare_exchange_weak;
};

template <>
struct atomic_operations<memory_order::consume> :
    atomics::load_operations<memory_order::consume>,
    atomics::exchange_operations<memory_order::consume>,
    atomics::fence_operations<memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::consume,
        memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::consume,
        memory_order::relaxed> {
    using atomics::compare_exchange_operations<memory_order::consume,
        memory_order::consume>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::consume,
        memory_order::relaxed>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::consume,
        memory_order::consume>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::consume,
        memory_order::relaxed>::compare_exchange_weak;
};

template <>
struct atomic_operations<memory_order::release> :
    atomics::store_operations<memory_order::release>,
    atomics::exchange_operations<memory_order::release>,
    atomics::fence_operations<memory_order::release>,
    atomics::compare_exchange_operations<memory_order::release,
        memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::release,
        memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::release,
        memory_order::relaxed> {
    using atomics::compare_exchange_operations<memory_order::release,
        memory_order::acquire>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::release,
        memory_order::consume>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::release,
        memory_order::relaxed>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::release,
        memory_order::acquire>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::release,
        memory_order::consume>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::release,
        memory_order::relaxed>::compare_exchange_weak;
};

template <>
struct atomic_operations<memory_order::acq_rel> :
    atomics::exchange_operations<memory_order::acq_rel>,
    atomics::fence_operations<memory_order::acq_rel>,
    atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::relaxed> {
    using atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::acquire>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::consume>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::relaxed>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::acquire>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::consume>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::acq_rel,
        memory_order::relaxed>::compare_exchange_weak;
};

template <>
struct atomic_operations<memory_order::seq_cst> :
    atomics::load_operations<memory_order::seq_cst>,
    atomics::store_operations<memory_order::seq_cst>,
    atomics::exchange_operations<memory_order::seq_cst>,
    atomics::fence_operations<memory_order::seq_cst>,
    atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::seq_cst>,
    atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::acquire>,
    atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::consume>,
    atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::relaxed> {
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::seq_cst>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::acquire>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::consume>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::relaxed>::compare_exchange_strong;
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::seq_cst>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::acquire>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::consume>::compare_exchange_weak;
    using atomics::compare_exchange_operations<memory_order::seq_cst,
        memory_order::relaxed>::compare_exchange_weak;
};

UTL_NAMESPACE_END
