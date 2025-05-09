// Copyright 2023-2024 Bryan Wong

#pragma once

/**
 * Standard pair implementation up to and including C++17
 */

#if !defined(UTL_PAIR_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

#include "utl/utl_config.h"

#include "utl/utility/utl_pair_fwd.h"

#include "utl/compare/utl_compare_traits.h"
#include "utl/concepts/utl_assignable_from.h"
#include "utl/concepts/utl_common_with.h"
#include "utl/concepts/utl_equality_comparable.h"
#include "utl/concepts/utl_swappable.h"
#include "utl/tuple/utl_tuple_concepts.h"
#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/tuple/utl_tuple_traits.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_pair_details.h"
#include "utl/utility/utl_sequence.h"

UTL_NAMESPACE_BEGIN

template <typename T>
struct is_pair_like : bool_constant<tuple_like<T> && tuple_size_v<T> == 2> {};
template <typename T>
concept pair_like = tuple_like<T> && tuple_size_v<T> == 2;

template <typename T0, typename T1>
struct pair {
private:
    template <typename U0, typename U1>
    using construct_explicitly UTL_NODEBUG =
        disjunction<is_explicit_constructible<T0, U0>, is_explicit_constructible<T1, U1>>;
    template <typename U0, typename U1>
    using nothrow_construct UTL_NODEBUG =
        conjunction<is_nothrow_constructible<T0, U0>, is_nothrow_constructible<T1, U1>>;
    template <typename U0, typename U1>
    using reference_dangles UTL_NODEBUG = disjunction<reference_constructs_from_temporary<T0, U0>,
        reference_constructs_from_temporary<T1, U1>>;
    template <typename U0, typename U1>
    using reference_valid UTL_NODEBUG = negation<reference_dangles<U0, U1>>;
    template <typename U0, typename U1>
    using nothrow_assign_from_pair UTL_NODEBUG =
        conjunction<is_nothrow_assignable<T0&, U0>, is_nothrow_assignable<T1&, U1>>;
    template <typename U0, typename U1>
    using nothrow_const_assign_from_pair UTL_NODEBUG =
        conjunction<is_nothrow_assignable<T0 const&, U0>, is_nothrow_assignable<T1 const&, U1>>;

    template <typename U0, typename U1>
    using nothrow_swappable UTL_NODEBUG =
        conjunction<is_nothrow_swappable_with<T0&, U0&>, is_nothrow_swappable_with<T1&, U1&>>;
    template <typename U0, typename U1>
    using nothrow_const_swappable UTL_NODEBUG =
        conjunction<is_nothrow_swappable_with<T0 const&, U0&>,
            is_nothrow_swappable_with<T1 const&, U1&>>;

    using swap_type UTL_NODEBUG =
        conditional_t<swappable<T0> && swappable<T1>, pair, details::pair::invalid_swap_t<T0, T1>>;
    using const_swap_type UTL_NODEBUG = conditional_t<swappable<T0 const> && swappable<T1 const>,
        pair, details::pair::invalid_swap_t<T0, T1>> const;

    template <typename L0, typename L1, size_t... Is, size_t... Js>
    requires tuple_like<remove_reference_t<L0>> && tuple_like<remove_reference_t<L1>>
    __UTL_HIDE_FROM_ABI constexpr pair(L0& l0, L1& l1, index_sequence<Is...>,
        index_sequence<Js...>) noexcept(conjunction_v<details::tuple::is_all_nothrow_gettable<L0&>,
        details::tuple::is_all_nothrow_gettable<L1&>,
        __UTL is_nothrow_constructible<T0, tuple_element_t<Is, L0>...>,
        __UTL is_nothrow_constructible<T1, tuple_element_t<Js, L1>...>>)
        : first(__UTL forward<tuple_element_t<Is, L0>>(__UTL get_element<Is>(l0))...)
        , second(__UTL forward<tuple_element_t<Js, L1>>(__UTL get_element<Js>(l1))...) {}

    template <typename U0, typename U1>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) constexpr pair& assign(U0&& u0, U1&& u1) noexcept(
        nothrow_assign_from_pair<U0&&, U1&&>::value) {
        first = __UTL forward<U0>(u0);
        second = __UTL forward<U1>(u1);
        return *this;
    }

    template <typename U0, typename U1>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, ALWAYS_INLINE) constexpr pair const& assign(U0&& u0, U1&& u1) const
        noexcept(nothrow_const_assign_from_pair<U0&&, U1&&>::value) {
        first = __UTL forward<U0>(u0);
        second = __UTL forward<U1>(u1);
        return *this;
    }

public:
    using first_type = T0;
    using second_type = T1;
    T0 first;
    T1 second;

    __UTL_HIDE_FROM_ABI constexpr pair(pair const&) noexcept(
        nothrow_construct<T0 const&, T1 const&>::value) = default;
    __UTL_HIDE_FROM_ABI constexpr pair(pair&&) noexcept(
        nothrow_construct<T0&&, T1&&>::value) = default;
    __UTL_HIDE_FROM_ABI constexpr pair& operator=(pair const&) noexcept(
        nothrow_assign_from_pair<T0 const&, T1 const&>::value) = default;
    __UTL_HIDE_FROM_ABI constexpr pair& operator=(pair&&) noexcept(
        nothrow_assign_from_pair<T0&&, T1&&>::value) = default;

    __UTL_HIDE_FROM_ABI constexpr pair const& operator=(pair const& other) const
        noexcept(nothrow_const_assign_from_pair<T0 const&, T1 const&>::value)
    requires requires(T0 const& t0, T1 const& t1) {
        t0 = t0;
        t1 = t1;
    }
    {
        return assign(other.first, other.second);
    }

    __UTL_HIDE_FROM_ABI constexpr pair const& operator=(pair&& other) const
        noexcept(nothrow_const_assign_from_pair<T0&&, T1&&>::value)
    requires requires(T0 t0, T1 t1) {
        t0 = __UTL move(t0);
        t1 = __UTL move(t1);
    }
    {
        return assign(__UTL move(other).first, __UTL move(other).second);
    }

public:
    template <typename U0 = T0, typename U1 = T1>
    requires swappable_with<T0&, U0&> && swappable_with<T1&, U1&>
    __UTL_HIDE_FROM_ABI constexpr void swap(pair<U0, U1>& other) noexcept(
        nothrow_swappable<U0&, U1&>::value) {
        __UTL ranges::swap(first, other.first);
        __UTL ranges::swap(second, other.second);
    }
    template <typename U0 = T0, typename U1 = T1>
    requires swappable_with<T0&, U0 const&> && swappable_with<T1&, U1 const&>
    __UTL_HIDE_FROM_ABI constexpr void swap(pair<U0, U1> const& other) noexcept(
        nothrow_swappable<U0 const&, U1 const&>::value) {
        __UTL ranges::swap(first, other.first);
        __UTL ranges::swap(second, other.second);
    }

    template <typename U0 = T0, typename U1 = T1>
    requires swappable_with<T0 const&, U0 const&> && swappable_with<T1 const&, U1 const&>
    __UTL_HIDE_FROM_ABI constexpr void swap(pair<U0, U1> const& other) const
        noexcept(nothrow_const_swappable<U0 const&, U1 const&>::value) {
        __UTL ranges::swap(first, other.first);
        __UTL ranges::swap(second, other.second);
    }

    template <typename U0 = T0, typename U1 = T1>
    requires swappable_with<T0 const&, U0&> && swappable_with<T1 const&, U1&>
    __UTL_HIDE_FROM_ABI constexpr void swap(pair<U0, U1>& other) const
        noexcept(nothrow_const_swappable<U0&, U1&>::value) {
        __UTL ranges::swap(first, other.first);
        __UTL ranges::swap(second, other.second);
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr void swap(swap_type& l, swap_type& r) noexcept(
        nothrow_swappable<T0&, T1&>::value) {
        l.swap(r);
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr void swap(const_swap_type& l,
        const_swap_type& r) noexcept(nothrow_const_swappable<T0 const&, T1 const&>::value) {
        l.swap(r);
    }

public:
    explicit(UTL_TRAIT_is_explicit_constructible(T0) ||
        UTL_TRAIT_is_explicit_constructible(T1)) constexpr pair() noexcept = default;

public:
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<T0 const&, T1 const&>::value) constexpr pair(
        T0 const& t0, T1 const& t1) noexcept(nothrow_construct<T0, T1>::value)
        : first(t0)
        , second(t1) {}

public:
    template <typename U0 = T0, typename U1 = T1>
    requires reference_valid<U0&&, U1&&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0&&, U1&&>::value) constexpr pair(
        U0&& u0, U1&& u1) noexcept(nothrow_construct<U0&&, U1&&>::value)
        : first(__UTL forward<U0>(u0))
        , second(__UTL forward<U1>(u1)) {}

    template <typename U0 = T0, typename U1 = T1>
    requires reference_dangles<U0&&, U1&&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0&&, U1&&>::value) constexpr pair(
        U0&& u0, U1&& u1) noexcept(nothrow_construct<U0&&, U1&&>::value) = delete;

public:
    template <typename U0, typename U1>
    requires reference_valid<U0&, U1&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0&, U1&>::value) constexpr pair(
        pair<U0, U1>& p) noexcept(nothrow_construct<U0&, U1&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1>
    requires reference_dangles<U0&, U1&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0&, U1&>::value) constexpr pair(
        pair<U0, U1>& p) noexcept(nothrow_construct<U0&, U1&>::value) = delete;

public:
    template <typename U0, typename U1>
    requires reference_valid<U0 const&, U1 const&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0 const&, U1 const&>::value) constexpr pair(
        pair<U0, U1> const& p) noexcept(nothrow_construct<U0 const&, U1 const&>::value)
        : first(p.first)
        , second(p.second) {}

    template <typename U0, typename U1>
    requires reference_dangles<U0 const&, U1 const&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0 const&, U1 const&>::value) constexpr pair(
        pair<U0, U1> const& p) noexcept(nothrow_construct<U0 const&, U1 const&>::value) = delete;

public:
    template <typename U0, typename U1>
    requires reference_valid<U0&&, U1&&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0&&, U1&&>::value) constexpr pair(
        pair<U0, U1>&& p) noexcept(nothrow_construct<U0&&, U1&&>::value)
        : first(__UTL move(p).first)
        , second(__UTL move(p).second) {}

    template <typename U0, typename U1>
    requires reference_dangles<U0&&, U1&&>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<U0&&, U1&&>::value) constexpr pair(
        pair<U0, U1>&& p) noexcept(nothrow_construct<U0&&, U1&&>::value) = delete;

public:
    template <typename U0, typename U1>
    requires reference_valid<U0 const&&, U1 const&&>::value
    __UTL_HIDE_FROM_ABI explicit(
        construct_explicitly<U0 const&&, U1 const&&>::value) constexpr pair(pair<U0, U1> const&&
            p) noexcept(nothrow_construct<U0 const&&, U1 const&&>::value)
        : first(__UTL move(p).first)
        , second(__UTL move(p).second) {}

    template <typename U0, typename U1>
    requires reference_dangles<U0 const&&, U1 const&&>::value
    __UTL_HIDE_FROM_ABI explicit(
        construct_explicitly<U0 const&&, U1 const&&>::value) constexpr pair(pair<U0, U1> const&&
            p) noexcept(nothrow_construct<U0 const&&, U1 const&&>::value) = delete;

public:
    template <typename P>
    requires pair_like<remove_reference_t<P>> &&
                 reference_valid<details::tuple::get_type_t<0, P>,
                     details::tuple::get_type_t<1, P>>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<details::tuple::get_type_t<0, P>,
        details::tuple::get_type_t<1, P>>::value) constexpr pair(P&&
            p) noexcept(details::tuple::is_all_nothrow_gettable<P>::value &&
        nothrow_construct<details::tuple::get_type_t<0, P>,
            details::tuple::get_type_t<1, P>>::value)
        : first(__UTL get_key(__UTL forward<P>(p)))
        , second(__UTL get_value(__UTL forward<P>(p))) {}

    template <typename P>
    requires pair_like<remove_reference_t<P>> &&
        reference_dangles<details::tuple::get_type_t<0, P>, details::tuple::get_type_t<1, P>>::value
    __UTL_HIDE_FROM_ABI explicit(construct_explicitly<details::tuple::get_type_t<0, P>,
        details::tuple::get_type_t<1, P>>::value) constexpr pair(P&&
            p) noexcept(details::tuple::is_all_nothrow_gettable<P>::value &&
        nothrow_construct<details::tuple::get_type_t<0, P>,
            details::tuple::get_type_t<1, P>>::value) = delete;

public:
    template <typename L0, typename L1>
    requires (
        tuple_like<remove_reference_t<L0>> && tuple_like<remove_reference_t<L1>> &&
        []<size_t... Is>(index_sequence<Is...>) {
            return constructible_from<T0, tuple_element_t<Is, L0>...>;
        }(tuple_index_sequence<L0>{}) &&
        []<size_t... Is>(index_sequence<Is...>) {
            return constructible_from<T1, tuple_element_t<Is, L1>...>;
        }(tuple_index_sequence<L1>{}))
    __UTL_HIDE_FROM_ABI constexpr pair(piecewise_construct_t, L0 l0, L1 l1) noexcept(
        __UTL is_nothrow_constructible<pair, L0&, L1&, tuple_index_sequence<L0>,
            tuple_index_sequence<L1>>::value)
        : pair(l0, l1, tuple_index_sequence<L0>{}, tuple_index_sequence<L1>{}) {}

    template <typename... Args0, typename... Args1>
    requires constructible_from<T0, Args0...> && constructible_from<T1, Args1...>
    __UTL_HIDE_FROM_ABI constexpr pair(piecewise_construct_t, tuple<Args0...> l0,
        tuple<Args1...> l1) noexcept(__UTL is_nothrow_constructible<pair, tuple<Args0...>&,
        tuple<Args1...>&, index_sequence_for<Args0...>, index_sequence_for<Args1...>>::value)
        : pair(l0, l1, index_sequence_for<Args0...>{}, index_sequence_for<Args1...>{}) {}

public:
    template <typename U0, typename U1>
    requires requires(T0& t0, T1& t1, U0 const& u0, U1 const& u1) {
        t0 = u0;
        t1 = u1;
    }
    __UTL_HIDE_FROM_ABI constexpr pair& operator=(pair<U0, U1> const& p) noexcept(
        nothrow_assign_from_pair<U0 const&, U1 const&>::value) {
        return assign(p.first, p.second);
    }

    template <typename U0, typename U1>
    requires requires(T0 const& t0, T1 const& t1, U0 const& u0, U1 const& u1) {
        t0 = u0;
        t1 = u1;
    }
    __UTL_HIDE_FROM_ABI constexpr pair const& operator=(pair<U0, U1> const& p) const
        noexcept(nothrow_const_assign_from_pair<U0 const&, U1 const&>::value) {
        return assign(p.first, p.second);
    }

public:
    template <typename U0, typename U1>
    requires requires(T0& t0, T1& t1, U0&& u0, U1&& u1) {
        t0 = __UTL move(u0);
        t1 = __UTL move(u1);
    }
    __UTL_HIDE_FROM_ABI constexpr pair& operator=(pair<U0, U1>&& p) noexcept(
        nothrow_assign_from_pair<U0&&, U1&&>::value) {
        return assign(__UTL move(p).first, __UTL move(p).second);
    }

    template <typename U0, typename U1>
    requires requires(T0 const& t0, T1 const& t1, U0&& u0, U1&& u1) {
        t0 = __UTL move(u0);
        t1 = __UTL move(u1);
    }
    __UTL_HIDE_FROM_ABI constexpr pair const& operator=(pair<U0, U1>&& p) const
        noexcept(nothrow_const_assign_from_pair<U0&&, U1&&>::value) {
        return assign(__UTL move(p).first, __UTL move(p).second);
    }

public:
    template <typename P>
    requires pair_like<remove_reference_t<P>> &&
        assignable_from<T0&, details::tuple::get_type_t<0, P>> &&
        assignable_from<T1&, details::tuple::get_type_t<1, P>>
    __UTL_HIDE_FROM_ABI constexpr pair& operator=(P&& p) noexcept(
        details::tuple::is_all_nothrow_gettable<P>::value &&
        nothrow_assign_from_pair<details::tuple::get_type_t<0, P>,
            details::tuple::get_type_t<1, P>>::value) {
        return assign(__UTL get_key(__UTL forward<P>(p)), __UTL get_value(__UTL forward<P>(p)));
    }

    template <typename P>
    requires pair_like<remove_reference_t<P>> &&
        assignable_from<T0 const&, details::tuple::get_type_t<0, P>> &&
        assignable_from<T1 const&, details::tuple::get_type_t<1, P>>
    __UTL_HIDE_FROM_ABI constexpr pair const& operator=(P&& p) const
        noexcept(details::tuple::is_all_nothrow_gettable<P>::value &&
            nothrow_const_assign_from_pair<details::tuple::get_type_t<0, P>,
                details::tuple::get_type_t<1, P>>::value) {
        return assign(__UTL get_key(__UTL forward<P>(p)), __UTL get_value(__UTL forward<P>(p)));
    }

    __UTL_HIDE_FROM_ABI constexpr ~pair() = default;
}; // pair

namespace details {
namespace pair {

template <pair_like T, pair_like U>
using three_way_result_t UTL_NODEBUG =
    common_comparison_category_t<decltype(__UTL details::tuple::decl_element<0, T>() <=>
                                     __UTL details::tuple::decl_element<0, U>()),
        decltype(__UTL details::tuple::decl_element<1, T>() <=>
            __UTL details::tuple::decl_element<1, U>())>;

template <pair_like T, pair_like U>
__UTL_HIDE_FROM_ABI constexpr three_way_result_t<T, U> three_way(T const& l, U const& r) noexcept(
    noexcept(__UTL get_key(l) <=> __UTL get_key(r)) && noexcept(
        __UTL get_value(l) <=> __UTL get_value(r))) {
    auto c = __UTL get_key(l) <=> __UTL get_key(r);
    return c != 0 ? c : __UTL get_value(l) <=> __UTL get_value(r);
}

} // namespace pair
} // namespace details

template <typename T0, typename T1, equality_comparable_with<T0> U0,
    equality_comparable_with<T1> U1>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator==(pair<T0, T1> const& l,
    pair<U0, U1> const& r) noexcept(noexcept(l.first == r.first && l.second == r.second)) {
    return l.first == r.first && l.second == r.second;
}

template <typename T0, typename T1, equality_comparable_with<T0> U0,
    equality_comparable_with<T1> U1>
requires (
    requires(T0 const& l, U0 const& r) { l < r; } && requires(T1 const& l, U1 const& r) { l < r; })
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator<(pair<T0, T1> const& l,
    pair<U0, U1> const& r) noexcept(noexcept(l.first == r.first && l.second == r.second)) {
    return l.first < r.first || (l.first == r.first && l.second < r.second);
}

template <typename T0, typename T1, equality_comparable_with<T0> U0,
    equality_comparable_with<T1> U1>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator!=(
    pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(noexcept(!(l == r))) {
    return !(l == r);
}

template <typename T0, typename T1, typename U0, typename U1>
requires (requires(pair<T0, T1> const& l, pair<U0, U1> const& r) { r < l; })
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator<=(
    pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(noexcept(!(r < l))) {
    return !(r < l);
}

template <typename T0, typename T1, typename U0, typename U1>
requires (requires(pair<T0, T1> const& l, pair<U0, U1> const& r) { r < l; })
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator>(
    pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(noexcept(r < l)) {
    return r < l;
}

template <typename T0, typename T1, typename U0, typename U1>
requires (requires(pair<T0, T1> const& l, pair<U0, U1> const& r) { l < r; })
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator>=(
    pair<T0, T1> const& l, pair<U0, U1> const& r) noexcept(noexcept(!(l < r))) {
    return !(l < r);
}

template <typename T0, typename T1, three_way_comparable_with<T0> U0,
    three_way_comparable_with<T1> U1>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr auto operator<=>(pair<T0, T1> const& l,
    pair<U0, U1> const& r) noexcept(noexcept(details::pair::three_way(l, r))) {
    return details::pair::three_way(l, r);
}

UTL_NAMESPACE_END

namespace std {

template <typename T0, typename T1, __UTL common_with<T0> U0, __UTL common_with<T1> U1>
struct __UTL_PUBLIC_TEMPLATE common_type<__UTL pair<T0, T1>, __UTL pair<U0, U1>> {
    using type UTL_NODEBUG = __UTL pair<__UTL common_type_t<T0, U0>, __UTL common_type_t<T1, U1>>;
};

template <typename T0, typename T1, typename U0, typename U1, template <typename> class TQual,
    template <typename> class UQual>
requires requires {
    typename __UTL common_reference<TQual<T0>, UQual<U0>>::type;
    typename __UTL common_reference<TQual<T1>, UQual<U1>>::type;
}
struct __UTL_PUBLIC_TEMPLATE
    basic_common_reference<__UTL pair<T0, T1>, __UTL pair<U0, U1>, TQual, UQual> {
    using type UTL_NODEBUG = __UTL pair<__UTL common_reference_t<TQual<T0>, UQual<U0>>,
        __UTL common_reference_t<TQual<T1>, UQual<U1>>>;
};
} // namespace std