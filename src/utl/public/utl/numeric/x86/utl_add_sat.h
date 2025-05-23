// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#ifndef UTL_NUMERIC_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

#if !UTL_ARCH_x86
#  error "This header is only available on x86 targets"
#endif // UTL_ARCH_x86

#include "utl/numeric/utl_limits.h"
#include "utl/numeric/utl_sized_integral.h"

#if UTL_SUPPORTS_GNU_ASM

UTL_NAMESPACE_BEGIN
namespace details {
namespace add_sat {
namespace runtime {

#  if UTL_ARCH_x86_64
template <typename T UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<8>) T
        UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(8, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    auto sat = l;
    // Need to move imm64 to a register before it is usable
    auto const max = UTL_NUMERIC_maximum(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    __asm__("sar{q    %[bits], %[sat]   | %[sat], %[bits] }\n\t"
            "xor{q    %[max], %[sat]    | %[sat], %[max] }\n\t"
            "add{q    %[right], %[left] | %[left], %[right] }\n\t"
            "cmovno{q %[left],  %[sat]  | %[sat], %[left] }"
            : [sat] "+r"(sat), [left] "+r"(l)
            : [right] "r"(r), [max] "r"(max), [bits] "Ji"(shift)
            : "cc");
    return sat;
}
#  endif // UTL_ARCH_x86_64

template <typename T UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<4>) T
        UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_maximum(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    auto sat = l;
    __asm__("sar{l    %[bits], %[sat]   | %[sat], %[bits] }\n\t"
            "xor{l    %[max], %[sat]    | %[sat], %[max] }\n\t"
            "add{l    %[right], %[left] | %[left], %[right] }\n\t"
            "cmovno{l  %[left], %[sat]  | %[sat], %[left] }"
            : [left] "+r"(l), [sat] "+r"(sat)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

template <typename T UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<2>) T
        UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr auto max = UTL_NUMERIC_maximum(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    auto sat = l;
    __asm__("sar{w    %[bits], %[sat]   | %[sat], %[bits] }\n\t"
            "xor{w    %[max], %[sat]    | %[sat], %[max] }\n\t"
            "add{w    %[right], %[left] | %[left], %[right] }\n\t"
            "cmovno{w  %[left], %[sat]  | %[sat], %[left] }"
            : [left] "+r"(l), [sat] "+r"(sat)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

template <typename T UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<1>) T
        UTL_CONSTRAINT_CXX11( UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    using x86w = int16_t;
    static constexpr x86w max = UTL_NUMERIC_maximum(T);
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    x86w sat = l;
    __asm__("sar{w    %[bits], %[sat]   | %[sat], %[bits] }\n\t"
            "xor{w    %[max], %[sat]    | %[sat], %[max] }\n\t"
            "add{b    %[right], %[left] | %[left], %[right] }\n\t"
            "cmovno{w %w[left], %[sat]  | %[sat], %w[left] }"
            : [left] "+r"(l), [sat] "+r"(sat)
            : [max] "i"(max), [bits] "Ji"(shift), [right] "r"(r)
            : "cc");
    return sat;
}

} // namespace runtime
} // namespace add_sat
} // namespace details

UTL_NAMESPACE_END

#elif UTL_COMPILER_MSVC // UTL_SUPPORTS_GNU_ASM

#  include "utl/type_traits/utl_make_unsigned.h"
#  include "utl/utility/utl_signs.h"

#  include <intrin.h>

UTL_NAMESPACE_BEGIN
namespace details {
namespace add_sat {
namespace runtime {

#  if UTL_ARCH_x86_64
template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_signed_integral(8, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<8>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_signed_integral(8, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_maximum(T);
    __UTL make_unsigned_t<T> tmp;
    return !_addcarry_u64(0, l, r, &tmp) ? __UTL to_signed(tmp) : sat;
}
#  endif // UTL_ARCH_x86_64

template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<4>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_signed_integral(4, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_maximum(T);
    __UTL make_unsigned_t<T> tmp;
    return !_addcarry_u32(0, l, r, &tmp) ? __UTL to_signed(tmp) : sat;
}

template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<2>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_signed_integral(2, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_maximum(T);
    __UTL make_unsigned_t<T> tmp;
    return !_addcarry_u16(0, l, r, &tmp) ? __UTL to_signed(tmp) : sat;
}

template <typename T UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI auto has_overload_impl(int) noexcept -> __UTL true_type;
template <UTL_CONCEPT_CXX20(__UTL sized_signed_integral<1>) T UTL_CONSTRAINT_CXX11(
    UTL_TRAIT_is_sized_signed_integral(1, T))>
__UTL_HIDE_FROM_ABI T impl(T l, T r) noexcept {
    static constexpr int shift = sizeof(l) * CHAR_BIT - 1;
    T const sat = (l >> shift) ^ UTL_NUMERIC_maximum(T);
    __UTL make_unsigned_t<T> tmp;
    return !_addcarry_u8(0, l, r, &tmp) ? __UTL to_signed(tmp) : sat;
}
} // namespace runtime
} // namespace add_sat
} // namespace details

UTL_NAMESPACE_END

#endif // UTL_SUPPORTS_GNU_ASM
