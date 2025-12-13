// Copyright 2023-2025 Bryan Wong

#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

template <typename T, T N>
struct integral_constant {
    static constexpr T value = N;
    using value_type = T;
    using type = integral_constant;
    __UTL_HIDE_FROM_ABI constexpr operator value_type() const noexcept {
        return N;
    }
    __UTL_HIDE_FROM_ABI constexpr value_type operator()() const noexcept {
        return N;
    }
};

template <typename T, T N>
constexpr T integral_constant<T, N>::value;

template <typename T, T N>
using value_constant = integral_constant<T, N>;

template <size_t N>
using size_constant = integral_constant<size_t, N>;
using npos_type = size_constant<(size_t)-1>;

template <bool B>
using bool_constant = integral_constant<bool, B>;
using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename...>
using always_true_type = true_type;
template <typename...>
using always_false_type = false_type;

template <typename T, T N>
inline constexpr T integral_constant_v = N;
template <typename T, T N>
inline constexpr T value_constant_v = N;
template <typename...>
inline constexpr bool always_true_v = true;
template <typename...>
inline constexpr bool always_false_v = false;

template <typename T, typename... Ts>
struct unsupported_trait {
private:
    static_assert(always_false_v<T, Ts...>, "Unsupported type trait");

public:
    using type = void;
    static constexpr bool value = false;
};

template <typename T, typename... Ts>
inline constexpr bool unsupported_trait_v = unsupported_trait<T, Ts...>::value;
template <typename T, typename... Ts>
using unsupported_trait_t UTL_NODEBUG =
    typename unsupported_trait<T, Ts...>::type;

UTL_PUBLIC_DEFAULT_NAMESPACE_END
