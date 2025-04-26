// Copyright 2023-2024 Bryan Wong

#pragma once

#if !defined(UTL_ATOMIC_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#if !UTL_COMPILER_MSVC
#  error Invalid compiler
#endif

#include "utl/atomic/winapi/utl_interlocked.h"
#include "utl/atomic/winapi/utl_memory_access.h"
#include "utl/atomic/winapi/utl_memory_barriers.h"
#include "utl/concepts/utl_implicit_lifetime.h"
#include "utl/numeric/utl_sized_integral.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_copy_cv.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_is_class.h"
#include "utl/type_traits/utl_is_implicit_lifetime.h"
#include "utl/type_traits/utl_is_trivially_copyable.h"
#include "utl/type_traits/utl_make_unsigned.h"
#include "utl/type_traits/utl_remove_cv.h"
#include "utl/type_traits/utl_underlying_type.h"
#include "utl/utility/utl_to_underlying.h"

UTL_NAMESPACE_BEGIN

namespace atomics {

template <typename T, size_t = (alignof(T) == sizeof(T)) * sizeof(T)>
struct interpreted_type;
template <typename T>
using interpreted_type_t UTL_NODEBUG = typename interpreted_type<T>::type;

template <typename T>
struct interpreted_type<T, 1> {
    using type UTL_NODEBUG = char;
};

template <typename T>
struct interpreted_type<T, 2> {
    using type UTL_NODEBUG = short;
};

template <typename T>
struct interpreted_type<T, 4> {
    using type UTL_NODEBUG = long;
};

template <typename T>
struct interpreted_type<T, 8> {
    using type UTL_NODEBUG = __int64;
};

// TODO: Could we support int128?

#if UTL_CXX20
template <typename T>
concept interpretable_type = (implicit_lifetime<remove_cv_t<T>> && UTL_TRAIT_is_class(T) &&
    UTL_TRAIT_is_trivially_copyable(remove_cv_t<T>) &&
    requires { typename interpreted_type<T>::type; });

template <typename T>
struct is_interpretable : bool_constant<interpretable_type<T>> {};
#  define UTL_TRAIT_is_atomic_interpretable(TYPE) __UTL atomics::interpretable_type<TYPE>
#else // UTL_CXX20
namespace details {
template <typename T>
auto interpretable_impl(float) noexcept -> __UTL false_type;
template <typename T>
auto interpretable_impl(int) noexcept -> __UTL
    bool_constant<UTL_TRAIT_is_class(T) && UTL_TRAIT_is_implicit_lifetime(remove_cv_t<T>) &&
        UTL_TRAIT_is_trivially_copyable(remove_cv_t<T>) &&
        __UTL always_true<typename interpreted_type<T>::type>()>;
template <typename T>
using interpretable UTL_NODEBUG = decltype(__UTL atomics::details::interpretable_impl<T>(0));
#  define UTL_TRAIT_is_atomic_interpretable(TYPE) __UTL atomics::details::interpretable<TYPE>::value
} // namespace details
template <typename T>
struct is_interpretable : details::interpretable<T> {};
#endif // UTL_CXX20

namespace details {
template <typename T, typename Interlock>
struct basic_adaptor {
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using interlocked_type = Interlock;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return (interlocked_type)value;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type volatile* to_interlocked(
        volatile_pointer value) noexcept {
        return reinterpret_cast<interlocked_type volatile*>(value);
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return reinterpret_cast<interlocked_type const volatile*>(value);
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        // TODO: verify this is compilable when Interlock is void*
        return (value_type)value;
    }
};
} // namespace details

#if UTL_CXX20
template <typename T>
struct interlocked_adaptor;
#else
template <typename T, typename = void>
struct interlocked_adaptor;
#endif

#if UTL_CXX20
template <sized_integral<1> T>
struct interlocked_adaptor<T> : details::basic_adaptor<T, char> {};
#else
template <typename T>
struct interlocked_adaptor<T, enable_if_t<UTL_TRAIT_is_sized_integral(1, T)>> :
    details::basic_adaptor<T, char> {};
#endif

#if UTL_CXX20
template <sized_integral<2> T>
struct interlocked_adaptor<T> : details::basic_adaptor<T, short> {};
#else
template <typename T>
struct interlocked_adaptor<T, enable_if_t<UTL_TRAIT_is_sized_integral(2, T)>> :
    details::basic_adaptor<T, short> {};
#endif

#if UTL_CXX20
template <sized_integral<4> T>
struct interlocked_adaptor<T> : details::basic_adaptor<T, long> {};
#else
template <typename T>
struct interlocked_adaptor<T, enable_if_t<UTL_TRAIT_is_sized_integral(4, T)>> :
    details::basic_adaptor<T, long> {};
#endif

#if UTL_CXX20
template <sized_integral<8> T>
struct interlocked_adaptor<T> : details::basic_adaptor<T, __int64> {};
#else
template <typename T>
struct interlocked_adaptor<T, enable_if_t<UTL_TRAIT_is_sized_integral(8, T)>> :
    details::basic_adaptor<T, __int64> {};
#endif

#if UTL_CXX20
template <typename T>
requires is_pointer_v<T>
struct interlocked_adaptor<T> : details::basic_adaptor<T, void*> {};
#else
template <typename T>
struct interlocked_adaptor<T, enable_if_t<UTL_TRAIT_is_pointer(T)>> :
    details::basic_adaptor<T, void*> {};
#endif

#if UTL_CXX20
template <enum_type T>
struct interlocked_adaptor<T> : private interlocked_adaptor<underlying_type_t<T>> {
#else
template <typename T>
struct interlocked_adaptor<T, enable_if_t<UTL_TRAIT_is_enum(T)>> :
    private interlocked_adaptor<underlying_type_t<T>> {
#endif
private:
    using base_type = interlocked_adaptor<underlying_type_t<T>>;

public:
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using typename base_type::interlocked_type;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type value) noexcept {
        return base_type::to_interlocked(__UTL to_underlying(value));
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type volatile* to_interlocked(
        volatile_pointer value) noexcept {
        return reinterpret_cast<interlocked_type volatile*>(value);
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return reinterpret_cast<interlocked_type const volatile*>(value);
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        return static_cast<value_type>(base_type::to_value(value));
    }
};

#if UTL_CXX20
template <interpretable_type T>
struct interlocked_adaptor<T> {
#else
template <typename T>
struct interlocked_adaptor<T, enable_if_t<interpretable_type<T>::value>> {
#endif
public:
    using value_type = remove_cv_t<T>;
    using pointer = value_type*;
    using volatile_pointer = value_type volatile*;
    using const_pointer = value_type const*;
    using const_volatile_pointer = value_type const volatile*;
    using interlocked_type = interpreted_type_t<T>;

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type to_interlocked(value_type const& value) noexcept {
        interlocked_type ret;
        __UTL_MEMCPY(&ret, __UTL addressof(value), sizeof(value_type));
        return ret;
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type volatile* to_interlocked(
        volatile_pointer value) noexcept {
        return reinterpret_cast<interlocked_type volatile*>(value);
    }
    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr interlocked_type const volatile* to_interlocked(
        const_volatile_pointer value) noexcept {
        return reinterpret_cast<interlocked_type const volatile*>(value);
    }

    UTL_ATTRIBUTE(__HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD)
    static inline constexpr value_type to_value(interlocked_type value) noexcept {
        alignas(value_type) unsigned char buffer[sizeof(value_type)];
        // no need to launder, memcpy starts lifetime
        return *reinterpret_cast<value_type*>(__UTL_MEMCPY(buffer, &value, sizeof(value_type)));
    }
};

struct fence_traits {
private:
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using acquire_type = memory_order_type<memory_order::acquire>;
    using consume_type = memory_order_type<memory_order::consume>;
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(relaxed_type) noexcept {}
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence(relaxed_type) noexcept {}
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(acquire_type) noexcept {
        __UTL_ACQUIRE_BARRIER();
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(consume_type) noexcept {
        __UTL_ACQUIRE_BARRIER();
    }
    template <memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence(
        memory_order_type<O>) noexcept {
        __UTL_MEMORY_BARRIER();
    }
    template <memory_order O>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence(
        memory_order_type<O>) noexcept {
        UTL_COMPILER_BARRIER();
    }

public:
    template <memory_order O>
    struct operations {
    protected:
        static constexpr memory_order_type<O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void thread_fence() noexcept {
            thread_fence(this_order);
        }
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void signal_fence() noexcept {
            signal_fence(this_order);
        }
    };
};

struct load_traits {
private:
    template <typename T>
    using value_type = typename interlocked_adaptor<T>::value_type;
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using seq_cst_type = memory_order_type<memory_order::seq_cst>;

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, relaxed_type) noexcept {
        return (value_type<T>)__iso_volatile_load8((__int8 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, relaxed_type) noexcept {
        return (value_type<T>)__iso_volatile_load16((__int16 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, relaxed_type) noexcept {
        return (value_type<T>)__iso_volatile_load32((__int32 const volatile*)ctx);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, relaxed_type) noexcept {
        return (value_type<T>)__iso_volatile_load64((__int64 const volatile*)ctx);
    }
#if UTL_ARCH_ARM
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, seq_cst_type) noexcept {
        auto const val = (value_type<T>)__ldar8((unsigned __int8 const volatile*)ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, seq_cst_type) noexcept {
        auto const val = (value_type<T>)__ldar16((unsigned __int16 const volatile*)ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, seq_cst_type) noexcept {
        auto const val = (value_type<T>)__ldar32((unsigned __int32 const volatile*)ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, seq_cst_type) noexcept {
        auto const val = (value_type<T>)__ldar64((unsigned __int64 const volatile*)ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }
#  ifdef UTL_ENABLE_RCPC_LOAD
#    define __UTL_LOAD_ACQ(WIDTH, TARGET) \
        __ldapr##WIDTH((unsigned __int##WIDTH const volatile*)TARGET)
#  else
#    define __UTL_LOAD_ACQ(WIDTH, TARGET) \
        __ldar##WIDTH((unsigned __int##WIDTH const volatile*)TARGET)
#  endif
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, memory_order_type<O>) noexcept {
        auto const val = (value_type<T>)__UTL_LOAD_ACQ(8, ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, memory_order_type<O>) noexcept {
        auto const val = (value_type<T>)__UTL_LOAD_ACQ(16, ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, memory_order_type<O>) noexcept {
        auto const val = (value_type<T>)__UTL_LOAD_ACQ(32, ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, memory_order_type<O>) noexcept {
        auto const val = (value_type<T>)__UTL_LOAD_ACQ(64, ctx);
        UTL_COMPILER_BARRIER();
        return val;
    }
#  undef __UTL_LOAD_ACQ
#else
    template <UTL_CONCEPT_CXX20(integral) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> load(
        T const* ctx, memory_order_type<O>) noexcept {
        auto val = load(ctx, memory_order_relaxed);
        UTL_COMPILER_BARRIER();
        return val;
    }
#endif
public:
    template <memory_order O>
    struct operations {
    protected:
        static_assert(is_load_order<O>(), "Invalid order");
        static constexpr memory_order_type<O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline value_type<T> load(
            T const* ctx) noexcept {
            return load(ctx, this_order);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline value_type<T> load(
            T const* ctx) noexcept {
            using type UTL_NODEBUG = copy_cv_t<T const, underlying_type_t<T>>;
            return (value_type<T>)load((type*)ctx, this_order);
        }

        template <UTL_CONCEPT_CXX20(boolean_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_boolean(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline value_type<T> load(
            T const* ctx) noexcept {
            return load(ctx, this_order);
        }

        template <UTL_CONCEPT_CXX20(interpretable_type) T UTL_CONSTRAINT_CXX11(is_interpretable<T>::value)>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) static inline value_type<T> load(
            T const* ctx) noexcept {
            using type UTL_NODEBUG = copy_cv_t<T const, interpreted_type_t<T>>;
            return adaptor::to_value(load(adaptor::to_interlocked(ctx), this_order));
        }
    };
};

struct store_traits {
private:
    template <typename T>
    using value_type = typename interlocked_adaptor<T>::value_type;
    using relaxed_type = value_constant<memory_order, memory_order::relaxed>;
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store8((__int8 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store16((__int16 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store32((__int32 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        __iso_volatile_store64((__int64 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, seq_cst_typ) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedExchange8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, seq_cst_typ) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        _InterlockedExchange16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, seq_cst_typ) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        _InterlockedExchange32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, seq_cst_typ) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        _InterlockedExchange64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value));
    }

#if UTL_ARCH_ARM
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        UTL_COMPILER_BARRIER();
        __stlr8((unsigned __int8 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        UTL_COMPILER_BARRIER();
        __stlr16((unsigned __int16 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        UTL_COMPILER_BARRIER();
        __stlr32((unsigned __int32 volatile*)ctx, value);
    }

    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        UTL_COMPILER_BARRIER();
        __stlr64((unsigned __int64 volatile*)ctx, value);
    }
#else
    template <UTL_CONCEPT_CXX20(integral) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
        T* ctx, value_type<T> value, value_constant<memory_order, O>) noexcept {
        UTL_COMPILER_BARRIER();
        store(ctx, value, memory_order_relaxed);
    }
#endif
public:
    template <memory_order O>
    struct operations {
    protected:
        static_assert(is_store_order<O>(), "Invalid order");
        static constexpr value_constant<memory_order, O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <UTL_CONCEPT_CXX20(native_atomic_type) T UTL_CONSTRAINT_CXX11(is_native_atomic_type<T>::value)>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
            T* ctx, value_type<T> value) noexcept {
            store(ctx, value, this_order);
        }

        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
            T* ctx, value_type<T> value) noexcept {
            using type UTL_NODEBUG = copy_cv_t<T, underlying_type_t<T>>;
            store((type*)ctx, (underlying_type_t<T>)value, this_order);
        }

        template <UTL_CONCEPT_CXX20(boolean_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_boolean(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
            T* ctx, value_type<T> value) noexcept {
            store(ctx, value, this_order);
        }

        template <UTL_CONCEPT_CXX20(interpretable_type) T UTL_CONSTRAINT_CXX11(is_interpretable<T>::value)>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void store(
            T* ctx, value_type<T> value) noexcept {
            using type UTL_NODEBUG = copy_cv_t<T, interpreted_type_t<T>>;
            alignas(T) unsigned char buffer[sizeof(T)];
            store((type*)ctx, *(interpreted_type_t<T>*)__UTL_MEMCPY(buffer, &value, sizeof(T)),
                this_order);
        }
    };
};

template <memory_order O>
using fence_operations = fence_traits::operations<O>;
template <memory_order O>
using load_operations = load_traits::operations<O>;
template <memory_order O>
using store_operations = store_traits::operations<O>;

struct exchange_traits {
private:
    template <typename T>
    using value_type = typename interlocked_adaptor<T>::value_type;
    using relaxed_type = memory_order_type<memory_order::relaxed>;
    using acquire_type = memory_order_type<memory_order::acquire>;
    using release_type = memory_order_type<memory_order::release>;
    using consume_type = memory_order_type<memory_order::consume>;
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedExchange8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange16_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange32_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange64_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangePointer_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange8_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange16_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange32_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange64_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangePointer_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange8_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange16_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange32_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchange64_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangePointer_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedExchange8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedExchange16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedExchange32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedExchange64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <typename T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangePointer(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd8_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64_nf(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd8_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64_acq(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd8_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64_rel(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedExchangeAdd8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd16(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd32(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(_InterlockedExchangeAdd64(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(integral) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_sub(
        T* ctx, value_type<T> value, memory_order_type<O> order) noexcept {
        return fetch_add(ctx, (T)(0u - (make_unsigned_t<T>)value), order);
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd16_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd32_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd64_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd8_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd16_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd32_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd64_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd8_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd16_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd32_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd64_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedAnd64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr16_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr32_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr64_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr8_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr16_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr32_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr64_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr8_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr16_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr32_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr64_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedOr64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor8_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor16_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor32_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, relaxed_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor64_nf(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor8_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor16_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor32_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, release_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor64_rel(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor8_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor16_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor32_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, acquire_type) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor64_acq(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<1>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(1, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor8(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<2>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(2, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor16(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<4>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(4, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor32(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }
    template <UTL_CONCEPT_CXX20(sized_integral<8>) T, memory_order O UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(8, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
        T* ctx, value_type<T> value, memory_order_type<O>) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        return adaptor::to_value(
            _InterlockedXor64(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value)));
    }

public:
    template <memory_order O>
    struct operations {
    protected:
        static constexpr value_constant<memory_order, O> this_order{};
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <typename T UTL_CONSTRAINT_CXX11(
            UTL_TRAIT_is_integral(T) || UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(integral<T> || is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
            T* ctx, value_type<T> value) noexcept {
            return exchange(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(enum_type) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_enum(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
            T* ctx, value_type<T> value) noexcept {
            using type UTL_NODEBUG = copy_cv_t<T, underlying_type_t<T>>;
            return (value_type<T>)exchange((type*)ctx, (underlying_type_t<T>)value);
        }
        template <UTL_CONCEPT_CXX20(interpretable_type) T UTL_CONSTRAINT_CXX11(is_interpretable<T>::value)>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> exchange(
            T* ctx, value_type<T> const& value) noexcept {
            using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
            return adaptor::to_value(
                exchange(adaptor::to_interlocked(ctx), adaptor::to_interlocked(value), this_order));
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
            T* ctx, value_type<T> value) noexcept {
            return fetch_add(ctx, value, this_order);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_add(
            T* ctx, ptrdiff_t value) noexcept {
            static constexpr intptr_t stride = sizeof(T);
            using type UTL_NODEBUG = copy_cv_t<T, intptr_t>;
            return (value_type<T>)fetch_add((type*)ctx, (intptr_t)value * stride);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_sub(
            T* ctx, value_type<T> value) noexcept {
            return fetch_sub(ctx, value, this_order);
        }
        template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_pointer(T))>
        UTL_CONSTRAINT_CXX20(is_pointer_v<T>)
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_sub(
            T* ctx, ptrdiff_t value) noexcept {
            static constexpr intptr_t stride = sizeof(T);
            using type UTL_NODEBUG = copy_cv_t<T, intptr_t>;
            return (value_type<T>)fetch_sub((type*)ctx, (intptr_t)value * stride);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_and(
            T* ctx, value_type<T> value) noexcept {
            return fetch_and(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_or(
            T* ctx, value_type<T> value) noexcept {
            return fetch_or(ctx, value, this_order);
        }
        template <UTL_CONCEPT_CXX20(integral) T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_integral(T))>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline value_type<T> fetch_xor(
            T* ctx, value_type<T> value) noexcept {
            return fetch_xor(ctx, value, this_order);
        }
    };
};

template <>
struct exchange_traits::operations<memory_order::consume> :
    exchange_traits::operations<memory_order::acquire> {};

template <memory_order O>
using exchange_operations = exchange_traits::operations<O>;

struct compare_exchange_traits {
private:
    template <typename T>
    using value_type UTL_NODEBUG = typename interlocked_adaptor<T>::value_type;
    template <typename T>
    using pointer UTL_NODEBUG = typename interlocked_adaptor<T>::pointer;
    using relaxed_type UTL_NODEBUG = memory_order_type<memory_order::relaxed>;
    using acquire_type UTL_NODEBUG = memory_order_type<memory_order::acquire>;
    using release_type UTL_NODEBUG = memory_order_type<memory_order::release>;
    using consume_type UTL_NODEBUG = memory_order_type<memory_order::consume>;
    using seq_cst_type UTL_NODEBUG = memory_order_type<memory_order::seq_cst>;
    using acq_rel_type UTL_NODEBUG = memory_order_type<memory_order::acq_rel>;
    static constexpr release_type release_v{};
    static constexpr acquire_type acquire_v{};

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline char interlocked(
        char volatile* ctx, char expected, char desired, relaxed_type) noexcept {
        return _InterlockedCompareExchange8_nf(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline char interlocked(
        char volatile* ctx, char expected, char desired, release_type) noexcept {
        return _InterlockedCompareExchange8_rel(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline char interlocked(
        char volatile* ctx, char expected, char desired, acquire_type) noexcept {
        return _InterlockedCompareExchange8_acq(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline char interlocked(
        char volatile* ctx, char expected, char desired, acq_rel) noexcept {
        return _InterlockedCompareExchange8(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline short interlocked(
        short volatile* ctx, short expected, short desired, relaxed_type) noexcept {
        return _InterlockedCompareExchange16_nf(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline short interlocked(
        short volatile* ctx, short expected, short desired, release_type) noexcept {
        return _InterlockedCompareExchange16_rel(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline short interlocked(
        short volatile* ctx, short expected, short desired, acquire_type) noexcept {
        return _InterlockedCompareExchange16_acq(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline short interlocked(
        short volatile* ctx, short expected, short desired, acq_rel) noexcept {
        return _InterlockedCompareExchange16(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline long interlocked(
        long volatile* ctx, long expected, long desired, relaxed_type) noexcept {
        return _InterlockedCompareExchange32_nf(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline long interlocked(
        long volatile* ctx, long expected, long desired, release_type) noexcept {
        return _InterlockedCompareExchange32_rel(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline long interlocked(
        long volatile* ctx, long expected, long desired, acquire_type) noexcept {
        return _InterlockedCompareExchange32_acq(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline long interlocked(
        long volatile* ctx, long expected, long desired, acq_rel) noexcept {
        return _InterlockedCompareExchange32(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline __int64 interlocked(
        __int64 volatile* ctx, __int64 expected, __int64 desired, relaxed_type) noexcept {
        return _InterlockedCompareExchange64_nf(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline __int64 interlocked(
        __int64 volatile* ctx, __int64 expected, __int64 desired, release_type) noexcept {
        return _InterlockedCompareExchange64_rel(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline __int64 interlocked(
        __int64 volatile* ctx, __int64 expected, __int64 desired, acquire_type) noexcept {
        return _InterlockedCompareExchange64_acq(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline __int64 interlocked(
        __int64 volatile* ctx, __int64 expected, __int64 desired, acq_rel) noexcept {
        return _InterlockedCompareExchange64(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void* interlocked(
        void* volatile* ctx, void* expected, void* desired, relaxed_type) noexcept {
        return _InterlockedCompareExchangePointer_nf(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void* interlocked(
        void* volatile* ctx, void* expected, void* desired, release_type) noexcept {
        return _InterlockedCompareExchangePointer_rel(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void* interlocked(
        void* volatile* ctx, void* expected, void* desired, acquire_type) noexcept {
        return _InterlockedCompareExchangePointer_acq(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void* interlocked(
        void* volatile* ctx, void* expected, void* desired, acq_rel_type) noexcept {
        return _InterlockedCompareExchangePointer(ctx, desired, comp);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool interlocked128(
        __int64 volatile* ctx, __int64* expected, __int64 hi, __int64 lo, relaxed_type) noexcept {
        return _InterlockedCompareExchange128_nf(ctx, hi, lo, expected);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void* interlocked128(
        __int64 volatile* ctx, __int64* expected, __int64 hi, __int64 lo, release_type) noexcept {
        return _InterlockedCompareExchange128_rel(ctx, hi, lo, expected);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void* interlocked128(
        __int64 volatile* ctx, __int64* expected, __int64 hi, __int64 lo, acquire_type) noexcept {
        return _InterlockedCompareExchange128_acq(ctx, hi, lo, expected);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline void* interlocked128(
        __int64 volatile* ctx, __int64* expected, __int64 hi, __int64 lo, acq_rel_type) noexcept {
        return _InterlockedCompareExchange128(ctx, hi, lo, expected);
    }

    template <memory_order F>
    static UTL_CONSTEVAL void interlock_order(relaxed_type, memory_order_type<F>) noexcept = delete;
    static UTL_CONSTEVAL void interlock_order(consume_type, acquire_type) noexcept = delete;
    template <memory_order S>
    static UTL_CONSTEVAL void interlock_order(memory_order_type<S>, seq_cst_type) noexcept = delete;

    static UTL_CONSTEVAL acq_rel_type interlock_order(seq_cst_type, seq_cst_type) noexcept {
        return {};
    }
    static UTL_CONSTEVAL relaxed_type interlock_order(relaxed_type, relaxed_type) noexcept {
        return {};
    }
    static UTL_CONSTEVAL release_type interlock_order(release_type, relaxed_type) noexcept {
        return {};
    }
    template <memory_order F>
    static UTL_CONSTEVAL acquire_type interlock_order(acquire_type, memory_order_type<F>) noexcept {
        static_assert(is_load_order<F>(), "Invalid failure order");
        return {};
    }
    template <memory_order F>
    static UTL_CONSTEVAL acquire_type interlock_order(consume_type, memory_order_type<F>) noexcept {
        static_assert(is_load_order<F>(), "Invalid failure order");
        return {};
    }
    template <memory_order F>
    static UTL_CONSTEVAL acq_rel_type interlock_order(release_type, memory_order_type<F>) noexcept {
        static_assert(is_load_order<F>(), "Invalid failure order");
        return {};
    }
    template <memory_order F>
    static UTL_CONSTEVAL acq_rel_type interlock_order(acq_rel_type, memory_order_type<F>) noexcept {
        static_assert(is_load_order<F>(), "Invalid failure order");
        return {};
    }
    template <memory_order F>
    static UTL_CONSTEVAL acq_rel_type interlock_order(seq_cst_type, memory_order_type<F>) noexcept {
        static_assert(is_load_order<F>(), "Invalid failure order");
        return {};
    }

    template <UTL_CONCEPT_CXX20(integral) T, memory_order S, memory_order F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_integral(T) && sizeof(T) <= 8)>
    UTL_CONCEPT_CXX20(sizeof(T) <= 8)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, memory_order_type<S> success,
        memory_order_type<F> failure) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        constexpr auto order = interlock_order(success, failure);
        auto const comp = adaptor::to_interlock(*expected);
        auto const prev = interlocked(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(desired), comp, order);
        if (comp == prev) {
            return true;
        }

        __UTL_MEMCPY(expected, &prev, sizeof(T));
        return false;
    }

    template <UTL_CONCEPT_CXX20(enum_type) T, memory_order S, memory_order F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_enum(T) && sizeof(T) <= 8)>
    UTL_CONCEPT_CXX20(sizeof(T) <= 8)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(
        T* ctx, pointer<T> expected, value_type<T> desired, memory_order_type<S> success,
        memory_order_type<F> failure) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        constexpr auto order = interlock_order(success, failure);
        auto const comp = adaptor::to_interlock(*expected);
        auto const prev = interlocked(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(desired), comp, order);
        if (comp == prev) {
            return true;
        }

        __UTL_MEMCPY(expected, &prev, sizeof(T));
        return false;
    }

    template <typename T, memory_order S, memory_order F UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_pointer(T))>
    UTL_CONSTRAINT_CXX20(UTL_TRAIT_is_pointer(T))
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(T* ctx,
        pointer<T> expected, value_type<T> desired, memory_order_type<S> success,
        memory_order_type<F> failure) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        constexpr auto order = interlock_order(success, failure);
        auto const comp = adaptor::to_interlock(*expected);
        auto const prev = interlocked(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(desired), comp, order);
        if (comp == prev) {
            return true;
        }

        __UTL_MEMCPY(expected, &prev, sizeof(T));
        return false;
    }

    template <UTL_CONCEPT_CXX20(interpretable_type) T, memory_order S, memory_order F UTL_CONSTRAINT_CXX11(is_interpretable<T>::value)>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(T* ctx,
        pointer<T> expected, value_type<T> const& desired, memory_order_type<S> success,
        memory_order_type<F> failure) noexcept {
        using adaptor UTL_NODEBUG = interlocked_adaptor<T>;
        constexpr auto order = interlock_order(success, failure);
        auto const comp = adaptor::to_interlock(*expected);
        auto const prev = interlocked(
            adaptor::to_interlocked(ctx), adaptor::to_interlocked(desired), comp, order);
        if (comp == prev) {
            return true;
        }

        __UTL_MEMCPY(expected, &prev, sizeof(T));
        return false;
    }

    template <UTL_CONCEPT_CXX20(sized_integral<16>) T, memory_order S, memory_order F UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_integral(16, T))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline bool compare_exchange(T* ctx,
        pointer<T> expected, value_type<T> const& desired, memory_order_type<S> success,
        memory_order_type<F> failure) noexcept {
        constexpr auto order = interlock_order(success, failure);
        alignas(16) __int64 buffer[2];
        __UTL_MEMCPY(buffer, &desired, 16);
        return interlocked128(
            (__int64 volatile*)ctx, buffer[1], buffer[0], (__int64*)expected, order);
    }

public:
    template <memory_order S, memory_order F>
    struct operations {
    protected:
        static constexpr memory_order_type<S> success_v{};
        using failure_type UTL_NODEBUG = compare_exchange_failure<F>;
        UTL_CONSTEXPR_CXX20 ~operations() noexcept = default;

    public:
        template <typename T>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto compare_exchange_strong(T* ctx,
            pointer<T> expected, value_type<T> const& desired, failure_type failure) noexcept
            -> decltype(compare_exchange(ctx, expected, desired, success_v, failure)) {
            return compare_exchange(ctx, expected, desired, success_v, failure);
        }

        template <typename T>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline auto compare_exchange_weak(T* ctx,
            pointer<T> expected, value_type<T> const& desired, failure_type failure) noexcept
            -> decltype(compare_exchange(ctx, expected, desired, success_v, failure)) {
            // Only < ARMv8.1-a has no dedicated CAS instruction
            // But it requires exclusive load/store intrinsics to be exposed by MSVC to be supported
            return compare_exchange(ctx, expected, desired, success_v, failure);
        }
    };
};

template <memory_order S, memory_order F>
using compare_exchange_operations = compare_exchange_traits::operations<S, F>;

#undef UTL_TRAIT_is_atomic_interpretable
} // namespace atomics

UTL_NAMESPACE_END
