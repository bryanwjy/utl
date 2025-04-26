// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/concepts/utl_same_as.h"
#include "utl/concepts/utl_swappable.h"
#include "utl/type_traits/utl_is_array.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_reference.h"
#include "utl/type_traits/utl_is_swappable.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_in_place.h"
#include "utl/utility/utl_move.h"

UTL_STD_NAMESPACE_BEGIN
struct unexpect_t;
template <typename>
class unexpected;
UTL_STD_NAMESPACE_END

#undef __UTL_ATTRIBUTE_GETTER
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER

UTL_NAMESPACE_BEGIN

struct __UTL_ABI_PUBLIC unexpect_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr unexpect_t() noexcept = default;
#ifdef UTL_CXX20
    template <same_as<::std::unexpect_t> U = ::std::unexpect_t>
    __UTL_HIDE_FROM_ABI inline constexpr unexpect_t(U) noexcept {}

    template <same_as<::std::unexpect_t> U = ::std::unexpect_t>
    __UTL_HIDE_FROM_ABI inline constexpr operator U() const noexcept {
        return U{};
    }
#endif
};

#if UTL_CXX14
UTL_INLINE_CXX17 constexpr unexpect_t unexpect{};
#endif

namespace details {
namespace unexpect {

#if UTL_CXX20
template <typename U>
concept tag_type = (same_as<U, ::std::unexpect_t> || same_as<U, unexpect_t>);

#  define __UTL_TRAIT_unexpect_tag(...) __UTL details::unexpect::tag_type<__VA_ARGS__>

#elif UTL_CXX14
template <typename U>
UTL_INLINE_CXX17 constexpr bool tag_type =
    UTL_TRAIT_is_same(U, ::std::unexpect_t) || UTL_TRAIT_is_same(U, unexpect_t);

#  define __UTL_TRAIT_unexpect_tag(...) __UTL details::unexpect::tag_type<__VA_ARGS__>

#else

template <typename U>
using tag_type =
    bool_constant<UTL_TRAIT_is_same(U, ::std::unexpect_t) || UTL_TRAIT_is_same(U, unexpect_t)>;

#  define __UTL_TRAIT_unexpect_tag(...) __UTL details::unexpect::tag_type<__VA_ARGS__>::value

#endif
} // namespace unexpect
} // namespace details

template <typename E>
class __UTL_PUBLIC_TEMPLATE unexpected {
    static_assert(UTL_TRAIT_is_destructible(E), "Invalid type");
    static_assert(!UTL_TRAIT_is_array(E), "Invalid type");
    static_assert(!UTL_TRAIT_is_reference(E), "Invalid type");

public:
    __UTL_HIDE_FROM_ABI inline constexpr unexpected(unexpected const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_constructible(E)) = default;
    __UTL_HIDE_FROM_ABI inline constexpr unexpected(unexpected&&) noexcept(
        UTL_TRAIT_is_nothrow_move_constructible(E)) = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 unexpected& operator=(unexpected const&) noexcept(
        UTL_TRAIT_is_nothrow_copy_assignable(E)) = default;
    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 unexpected& operator=(unexpected&&) noexcept(
        UTL_TRAIT_is_nothrow_move_assignable(E)) = default;

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX20 ~unexpected() noexcept = default;

    template <typename E1 = E UTL_CONSTRAINT_CXX11(!__UTL_TRAIT_unexpect_tag(remove_cvref_t<E1>) &&
        !__UTL_TRAIT_in_place_tag(remove_cvref_t<E1>) && UTL_TRAIT_is_constructible(E, E1))>
    UTL_CONSTRAINT_CXX20(!__UTL_TRAIT_unexpect_tag(remove_cvref_t<E1>) &&
        !__UTL_TRAIT_in_place_tag(remove_cvref_t<E1>) && is_constructible_v<E, E1>)
    __UTL_HIDE_FROM_ABI inline constexpr unexpected(E1&& arg) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, E1))
        : error_{__UTL forward<E1>(arg)} {}

    template <typename... Args UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_constructible(E, Args...))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, Args...>)
    __UTL_HIDE_FROM_ABI inline constexpr unexpected(in_place_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : error_{__UTL forward<Args>(args)...} {}

    template <typename U, typename... Args UTL_CONSTRAINT_CXX11(
        UTL_TRAIT_is_constructible(E, ::std::initializer_list<U>&, Args...))>
    UTL_CONSTRAINT_CXX20(is_constructible_v<E, ::std::initializer_list<U>&, Args...>)
    __UTL_HIDE_FROM_ABI inline constexpr unexpected(in_place_t, ::std::initializer_list<U> il,
        Args&&... args) noexcept(UTL_TRAIT_is_nothrow_constructible(E, ::std::initializer_list<U>&,
        Args...))
        : error_{il, __UTL forward<Args>(args)...} {}

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr E const& error() const& noexcept {
        return error_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline UTL_CONSTEXPR_CXX14 E& error() & noexcept {
        return error_;
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline constexpr E const&&
    error() const&& noexcept {
        return __UTL move(error_);
    }
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE, NODISCARD) inline UTL_CONSTEXPR_CXX14 E&& error() && noexcept {
        return __UTL move(error_);
    }

    __UTL_HIDE_FROM_ABI inline UTL_CONSTEXPR_CXX14 void swap(unexpected& other) noexcept(
        UTL_TRAIT_is_nothrow_swappable(E)) UTL_CONSTRAINT_CXX20(swappable<E>)
    {
#if UTL_CXX20 || UTL_COMPILER_CLANG_BASED
        static_assert(UTL_TRAIT_is_swappable(E), "Underlying type not swappable!");
        __UTL ranges::swap(error_, other.error_);
#else
        swap_impl(error_, other.error_);
#endif
    }

    __UTL_HIDE_FROM_ABI friend inline UTL_CONSTEXPR_CXX14 void swap(
        unexpected& left, unexpected& right) noexcept(noexcept(left.swap(right)))
        UTL_CONSTRAINT_CXX20(requires { left.swap(right); })
    {
        left.swap(right);
    }

    template <typename E1>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend inline constexpr bool operator==(unexpected const& left,
        unexpected<E1> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E1)) {
        static_assert(UTL_TRAIT_is_equality_comparable_with(E, E1), "Program ill-formed");
        static_assert(UTL_TRAIT_is_boolean_testable(decltype(left.error() == right.error())),
            "Program ill-formed");
        return left.error() == right.error();
    }

    template <typename E1>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend inline constexpr bool operator!=(unexpected const& left,
        unexpected<E1> const& right) noexcept(UTL_TRAIT_is_nothrow_equality_comparable_with(E,
        E1)) {
        return !(left.error() == right.error());
    }

private:
#if !UTL_CXX20 && !UTL_COMPILER_CLANG_BASED
    template <typename Other, UTL_CONSTRAINT_CXX11(UTL_TRAIT_is_swappable(E))>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) static inline UTL_CONSTEXPR_CXX14 void swap_impl(
        E& left, Other& right) {
        __UTL ranges::swap(error_, other.error_);
    }

    template <typename Other, UTL_CONSTRAINT_CXX11(!UTL_TRAIT_is_swappable(E))>
    __UTL_HIDE_FROM_ABI inline void swap_impl(E& left, Other& right);
#endif
    E error_;
};

#if UTL_CXX17
template <class E>
unexpected(E) -> unexpected<E>;
#endif

namespace details {
template <typename T>
struct is_unexpected_type : false_type {};
template <typename T>
struct is_unexpected_type<T const> : is_unexpected_type<T> {};
template <typename T>
struct is_unexpected_type<T volatile> : is_unexpected_type<T> {};
template <typename T>
struct is_unexpected_type<unexpected<T>> : true_type {};

#if UTL_CXX14

template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unexpected_type_v = false;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unexpected_type_v<T const> = is_unexpected_type_v<T>;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unexpected_type_v<T volatile> = is_unexpected_type_v<T>;
template <typename T>
UTL_INLINE_CXX17 constexpr bool is_unexpected_type_v<unexpected<T>> = true;

#  define __UTL_TRAIT_is_unexpected(...) __UTL details::is_unexpected_type_v<__VA_ARGS__>
#else
#  define __UTL_TRAIT_is_unexpected(...) __UTL details::is_unexpected_type<__VA_ARGS__>::value
#endif
} // namespace details

UTL_NAMESPACE_END
