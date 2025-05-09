// Copyright 2023-2024 Bryan Wong

#pragma once

/**
 * Standard Tuple implementation from C++20 onwards
 */

#ifndef UTL_TUPLE_PRIVATE_HEADER_GUARD
#  error "Private header accessed"
#endif

#if !UTL_CXX20
#  error "Header compiled with invalid standard"
#endif

#include "utl/utl_config.h"

#include "utl/compare/utl_compare_fwd.h"

#include "utl/compare/utl_compare_traits.h"
#include "utl/concepts/utl_allocator_type.h"
#include "utl/concepts/utl_assignable_from.h"
#include "utl/concepts/utl_assignable_to.h"
#include "utl/concepts/utl_common_reference_with.h"
#include "utl/concepts/utl_common_with.h"
#include "utl/concepts/utl_constructible_as.h"
#include "utl/concepts/utl_constructible_from.h"
#include "utl/concepts/utl_not_resolvable_to.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/concepts/utl_swappable.h"
#include "utl/concepts/utl_three_way_comparable.h"
#include "utl/concepts/utl_variadic_match.h"
#include "utl/memory/utl_uses_allocator.h"
#include "utl/ranges/utl_swap.h"
#include "utl/tuple/utl_tuple_compare_traits.h"
#include "utl/tuple/utl_tuple_concepts.h"
#include "utl/tuple/utl_tuple_details.h"
#include "utl/tuple/utl_tuple_get_element.h"
#include "utl/type_traits/utl_common_comparison_category.h"
#include "utl/type_traits/utl_common_reference.h"
#include "utl/type_traits/utl_common_type.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_is_base_of.h"
#include "utl/type_traits/utl_is_explicit_constructible.h"
#include "utl/type_traits/utl_is_implicit_constructible.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_default_constructible.h"
#include "utl/type_traits/utl_is_standard_layout.h"
#include "utl/type_traits/utl_reference_constructs_from_temporary.h"
#include "utl/type_traits/utl_template_list.h"
#include "utl/type_traits/utl_unwrap_reference.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_ignore.h"
#include "utl/utility/utl_move.h"
#include "utl/utility/utl_sequence.h"

// TODO: if std is included or forward declared use std, else use UTL
namespace std {
template <typename... Ts, __UTL common_with<Ts>... Us>
struct __UTL_PUBLIC_TEMPLATE common_type<__UTL tuple<Ts...>, __UTL tuple<Us...>> {
    using type UTL_NODEBUG = __UTL tuple<__UTL common_type_t<Ts, Us>...>;
};
template <typename... Ts, typename... Us, template <typename> class TQual,
    template <typename> class UQual>
requires (... && __UTL common_reference_with<TQual<Ts>, UQual<Us>>)
struct __UTL_PUBLIC_TEMPLATE
    basic_common_reference<__UTL tuple<Ts...>, __UTL tuple<Us...>, TQual, UQual> {
    using type UTL_NODEBUG = __UTL tuple<__UTL common_reference_t<TQual<Ts>, UQual<Us>>...>;
};
} // namespace std

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {
template <typename T>
using add_const_lvalue_reference UTL_NODEBUG = add_lvalue_reference<T const>;
template <typename T>
using add_const_rvalue_reference UTL_NODEBUG = add_rvalue_reference<T const>;

template <typename T>
concept unrecognized = tuple_like<remove_reference_t<T>> && !is_tuple_v<T>;

template <typename TupleLike, size_t... Is>
requires tuple_like<remove_reference_t<TupleLike>>
__UTL_HIDE_FROM_ABI auto nothrow_accessible(index_sequence<Is...>) noexcept
    -> bool_constant<(...&& noexcept(__UTL get_element<Is>(__UTL declval<TupleLike>())))>;

template <typename TupleLike>
requires tuple_like<remove_reference_t<TupleLike>>
inline constexpr bool is_nothrow_accessible_v = decltype(nothrow_accessible<TupleLike>(
    make_index_sequence<tuple_size<TupleLike>::value>{}))::value;

template <unrecognized TupleLike, size_t... Is>
__UTL_HIDE_FROM_ABI constexpr auto forward_unrecognized(
    TupleLike&& t, index_sequence<Is...>) noexcept(is_nothrow_accessible_v<TupleLike>)
    -> __UTL tuple<decltype(__UTL details::tuple::decl_element<Is, TupleLike>())...> {
    return {__UTL get_element<Is>(__UTL forward<TupleLike>(t))...};
}

template <unrecognized TupleLike>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN) constexpr auto forward_unrecognized(TupleLike&& t) noexcept(
    is_nothrow_accessible_v<TupleLike>) {
    return forward_unrecognized(
        __UTL forward<TupleLike>(t), make_index_sequence<tuple_size<TupleLike>::value>{});
}

template <typename T, typename... Tail>
struct storage<T, Tail...> {
private:
    static constexpr bool move_assignable =
        (is_move_assignable_v<T> && ... && is_move_assignable_v<Tail>);
    static constexpr bool move_constructible =
        (is_move_constructible_v<T> && ... && is_move_constructible_v<Tail>);

public:
    static constexpr size_t element_count = sizeof...(Tail) + 1;
    using head_type UTL_NODEBUG = T;
    using tail_type UTL_NODEBUG = storage<Tail...>;
    using move_assign_t UTL_NODEBUG = conditional_t<move_assignable, invalid_t, storage>;
    using move_construct_t UTL_NODEBUG = conditional_t<move_constructible, invalid_t, storage>;

    __UTL_HIDE_FROM_ABI constexpr storage() noexcept((is_nothrow_default_constructible_v<T> &&
        ... && is_nothrow_default_constructible_v<Tail>)) = default;
    __UTL_HIDE_FROM_ABI constexpr storage(storage const&) noexcept(
        (is_nothrow_copy_constructible_v<T> && ... &&
            is_nothrow_copy_constructible_v<Tail>)) = default;
    __UTL_HIDE_FROM_ABI constexpr storage& operator=(storage const&) noexcept(
        (is_nothrow_copy_assignable_v<T> && ... && is_nothrow_copy_assignable_v<Tail>)) = default;

    __UTL_HIDE_FROM_ABI constexpr storage(storage&&) noexcept((is_nothrow_move_constructible_v<T> &&
        ... && is_nothrow_move_constructible_v<Tail>)) = default;
    __UTL_HIDE_FROM_ABI constexpr storage& operator=(storage&&) noexcept(
        (is_nothrow_move_assignable_v<T> && ... && is_nothrow_move_assignable_v<Tail>)) = default;

    template <constructible_as<T> UHead, constructible_as<Tail>... UTail>
    __UTL_HIDE_FROM_ABI constexpr storage(UHead&& other_head, UTail&&... other_tail) noexcept(
        is_nothrow_constructible_v<head_type, UHead> &&
        is_nothrow_constructible_v<tail_type, UTail...>)
        : head(__UTL forward<UHead>(other_head))
        , tail(__UTL forward<UTail>(other_tail)...) {}

    template <allocator_usable_with<T> Alloc>
    requires constructible_from<T, allocator_arg_t, Alloc const&>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_constructible_v<head_type, allocator_arg_t, Alloc const&> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&>)
        : head(allocator_arg, alloc)
        , tail(allocator_arg, alloc) {}

    template <allocator_usable_with<T> Alloc>
    requires (!constructible_from<T, allocator_arg_t, Alloc const&> &&
                 constructible_from<T, Alloc const&>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_constructible_v<head_type, Alloc const&> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&>)
        : head(alloc)
        , tail(allocator_arg, alloc) {}

    template <allocator_type Alloc>
    requires constructible_from<T>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc) noexcept(
        is_nothrow_default_constructible_v<head_type> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&>)
        : head()
        , tail(allocator_arg, alloc) {}

    template <allocator_usable_with<T> Alloc, typename UHead, typename... UTail>
    requires constructible_from<T, allocator_arg_t, Alloc const&, UHead>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(is_nothrow_constructible_v<head_type, allocator_arg_t,
                                            Alloc const&, UHead> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&, UTail...>)
        : head(allocator_arg, alloc, __UTL forward<UHead>(other_head))
        , tail(allocator_arg, alloc, __UTL forward<UTail>(other_tail)...) {}

    template <allocator_usable_with<T> Alloc, typename UHead, typename... UTail>
    requires (!constructible_from<T, allocator_arg_t, Alloc const&, UHead> &&
                 constructible_from<T, UHead, Alloc const&>)
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(is_nothrow_constructible_v<head_type, UHead,
                                            Alloc const&> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&, UTail...>)
        : head(__UTL forward<UHead>(other_head), alloc)
        , tail(allocator_arg, alloc, __UTL forward<UTail>(other_tail)...) {}

    template <allocator_type Alloc, constructible_as<T> UHead, typename... UTail>
    __UTL_HIDE_FROM_ABI constexpr storage(allocator_arg_t, Alloc const& alloc, UHead&& other_head,
        UTail&&... other_tail) noexcept(is_nothrow_constructible_v<head_type, UHead> &&
        is_nothrow_constructible_v<tail_type, allocator_arg_t, Alloc const&, UTail...>)
        : head(__UTL forward<UHead>(other_head))
        , tail(allocator_arg, alloc, __UTL forward<UTail>(other_tail)...) {}

    template <typename U, typename... Us>
    __UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...>& other) noexcept(
        (is_nothrow_swappable_with_v<T&, U&> && ... && is_nothrow_swappable_with_v<Tail&, Us&>)) {
        __UTL ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename U, typename... Us>
    __UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...>& other) const
        noexcept((is_nothrow_swappable_with_v<T const&, U&> && ... &&
            is_nothrow_swappable_with_v<Tail const&, Us&>)) {
        __UTL ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename U, typename... Us>
    __UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...> const& other) noexcept(
        (is_nothrow_swappable_with_v<T&, U const&> && ... &&
            is_nothrow_swappable_with_v<Tail&, Us const&>)) {
        __UTL ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename U, typename... Us>
    __UTL_HIDE_FROM_ABI constexpr void swap(storage<U, Us...> const& other) const
        noexcept((is_nothrow_swappable_with_v<T const&, U const&> && ... &&
            is_nothrow_swappable_with_v<Tail const&, Us const&>)) {
        __UTL ranges::swap(head, other.head);
        tail.swap(other.tail);
    }

    template <typename UHead, typename... UTail>
    __UTL_HIDE_FROM_ABI constexpr storage&
    assign(UHead&& other_head, UTail&&... other_tail) noexcept(
        is_nothrow_assignable_v<T&, UHead>&& noexcept(tail.assign(__UTL declval<UTail>()...))) {
        head = __UTL forward<UHead>(other_head);
        tail.assign(__UTL forward<UTail>(other_tail)...);
        return *this;
    }

    template <typename UHead, typename... UTail>
    __UTL_HIDE_FROM_ABI constexpr storage const& assign(
        UHead&& other_head, UTail&&... other_tail) const
        noexcept(is_nothrow_assignable_v<T const&, UHead>&& noexcept(
            tail.assign(__UTL declval<UTail>()...))) {
        head = __UTL forward<UHead>(other_head);
        tail.assign(__UTL forward<UTail>(other_tail)...);
        return *this;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() && noexcept UTL_LIFETIMEBOUND
    -> T&& {
        return __UTL move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() & noexcept UTL_LIFETIMEBOUND
    -> T& {
        return head;
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() const&& noexcept UTL_LIFETIMEBOUND -> T const&& {
        return __UTL move(head);
    }

    template <size_t I>
    requires (I == 0)
    UTL_ATTRIBUTES(NODISCARD, CONST, _HIDE_FROM_ABI) constexpr auto get() const& noexcept UTL_LIFETIMEBOUND -> T const& {
        return head;
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) constexpr auto get() && noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) -> template_element_t<I, storage>&& {
        return __UTL move(tail).template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) constexpr auto get() & noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND)
    -> template_element_t<I, storage>& {
        return tail.template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) constexpr auto get() const&& noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) -> template_element_t<I, storage> const&& {
        return __UTL move(tail).template get<I - 1>();
    }

    template <size_t I>
    requires (I > 0) && (I < element_count)
    UTL_ATTRIBUTES(NODISCARD, CONST, FLATTEN, _HIDE_FROM_ABI) constexpr auto get() const& noexcept UTL_ATTRIBUTE(
        LIFETIMEBOUND) -> template_element_t<I, storage> const& {
        return tail.template get<I - 1>();
    }

    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) head_type head;
    UTL_ATTRIBUTE(NO_UNIQUE_ADDRESS) tail_type tail;
};

} // namespace tuple
} // namespace details

template <typename... Types>
class UTL_ATTRIBUTES(
    _PUBLIC_TEMPLATE, EMPTY_BASES) tuple : private details::tuple::storage<Types...> {
private:
    template <size_t I, typename T>
    friend struct tuple_element_offset;

    using base_type UTL_NODEBUG = details::tuple::storage<Types...>;
    using swap_type UTL_NODEBUG = conditional_t<conjunction<is_swappable<Types>...>::value, tuple,
        details::tuple::invalid_swap_t<Types...>>;
    using const_swap_type UTL_NODEBUG =
        conditional_t<conjunction<is_swappable<Types const>...>::value, tuple,
            details::tuple::invalid_swap_t<Types...>> const;
    using move_assign_t UTL_NODEBUG =
        conditional_t<conjunction<is_move_assignable<Types>...>::value, details::tuple::invalid_t,
            tuple>;
    using move_construct_t UTL_NODEBUG =
        conditional_t<conjunction<is_move_constructible<Types>...>::value,
            details::tuple::invalid_t, tuple>;

    using base_type::get;

    template <typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    __UTL_HIDE_FROM_ABI constexpr tuple(TupleLike&& other, index_sequence<Is...>) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<base_type,
            decltype(__UTL details::tuple::decl_element<Is, TupleLike>())...>)
        : base_type(__UTL get_element<Is>(__UTL forward<TupleLike>(other))...) {}

    template <allocator_type Alloc, typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    __UTL_HIDE_FROM_ABI constexpr tuple(allocator_arg_t, Alloc const& alloc, TupleLike&& other,
        index_sequence<Is...>) noexcept(details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<base_type, allocator_arg_t, Alloc const&,
            decltype(__UTL details::tuple::decl_element<Is, TupleLike>())...>)
        : base_type(
              allocator_arg, alloc, __UTL get_element<Is>(__UTL forward<TupleLike>(other))...) {}

    template <typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    __UTL_HIDE_FROM_ABI constexpr tuple& assign(TupleLike&& other, index_sequence<Is...>) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike>&& noexcept(
            __UTL declval<base_type&>().assign(
                __UTL get_element<Is>(__UTL declval<TupleLike>())...))) {
        return (tuple&)base_type::assign(__UTL get_element<Is>(__UTL forward<TupleLike>(other))...);
    }

    template <typename TupleLike, size_t... Is>
    requires tuple_like<remove_reference_t<TupleLike>>
    __UTL_HIDE_FROM_ABI constexpr tuple const& assign(
        TupleLike&& other, index_sequence<Is...>) const
        noexcept(details::tuple::is_nothrow_accessible_v<TupleLike>&& noexcept(
            __UTL declval<base_type const&>().assign(
                __UTL get_element<Is>(__UTL declval<TupleLike>())...))) {
        return (tuple const&)base_type::assign(
            __UTL get_element<Is>(__UTL forward<TupleLike>(other))...);
    }

public:
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple const&) noexcept(
        (... && is_nothrow_copy_constructible_v<Types>)) = default;
    __UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple const&) noexcept(
        (... && is_nothrow_copy_assignable_v<Types>)) = default;
    __UTL_HIDE_FROM_ABI constexpr tuple(tuple&&) noexcept(
        (... && is_nothrow_move_constructible_v<Types>)) = default;
    __UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple&&) noexcept(
        (... && is_nothrow_move_assignable_v<Types>)) = default;

    __UTL_HIDE_FROM_ABI UTL_CONSTEXPR_CXX20 ~tuple() = default;

    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple const& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, Types const&>))
    requires (... && is_assignable_v<Types const&, Types const&>)
    {
        return assign(other, index_sequence_for<Types...>{});
    }

    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple&& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, Types&&>))
    requires (... && is_assignable_v<Types const&, Types &&>)
    {
        return assign(__UTL move(other), index_sequence_for<Types...>{});
    }

public:
    template <same_as<Types>... Us>
    requires (... && swappable_with<Types&, Us&>)
    __UTL_HIDE_FROM_ABI constexpr void swap(tuple<Us...>& other) noexcept(
        (... && is_nothrow_swappable_with_v<Types&, Us&>)) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...>&)other);
    }

    template <same_as<Types>... Us>
    requires (... && swappable_with<Types const&, Us const&>)
    __UTL_HIDE_FROM_ABI constexpr void swap(tuple<Us...> const& other) const
        noexcept((... && is_nothrow_swappable_with_v<Types const&, Us const&>)) {
        static_assert(is_base_of<details::tuple::storage<Us...>, tuple<Us...>>::value,
            "tuple must inherit from storage");
        base_type::swap((details::tuple::storage<Us...> const&)other);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN) friend inline constexpr void swap(
        swap_type& l, swap_type& r) noexcept((... && is_nothrow_swappable_v<Types>)) {
        l.swap(r);
    }

    UTL_ATTRIBUTES(_HIDE_FROM_ABI, FLATTEN) friend inline constexpr void swap(const_swap_type& l,
        const_swap_type& r) noexcept((... && is_nothrow_swappable_v<Types const>)) {
        l.swap(r);
    }

public:
    __UTL_HIDE_FROM_ABI explicit(
        (... || is_explicit_constructible_v<Types>)) constexpr tuple() noexcept((... &&
        is_nothrow_default_constructible_v<Types>)) = default;

public:
    __UTL_HIDE_FROM_ABI explicit((... ||
        is_explicit_constructible_v<Types,
            Types const&>)) constexpr tuple(Types const&... args) noexcept((... &&
        is_nothrow_copy_constructible_v<Types>))
        : base_type(args...) {}

private:
    template <typename... Us>
    static consteval bool explicit_constructible() {
        return false;
    }

    template <typename... Us>
    requires (sizeof...(Us) == sizeof...(Types))
    static consteval bool explicit_constructible() {
        return (... || is_explicit_constructible_v<Types, Us>);
    }

    template <typename... Us>
    static constexpr bool explicit_constructible_v = explicit_constructible<Us...>();

    template <typename Tuple>
    static consteval bool use_other_overload() {
        if constexpr (sizeof...(Types) != 1)
            return false;
        else if constexpr (is_same_v<remove_cvref_t<Tuple>, tuple>)
            return true; // Should use a copy/move constructor instead.
        else {
            using First = tuple_element_t<0, tuple>;
            if constexpr (is_convertible_v<Tuple, First>)
                return true;
            else if constexpr (is_constructible_v<First, Tuple>)
                return true;
        }
        return false;
    }

public:
    template <typename... Us>
    requires (!requires {
        requires (sizeof...(Us) == sizeof...(Types));
        requires (... && is_constructible_v<Types, Us>);
        requires (... && !reference_constructs_from_temporary_v<Types, Us &&>);
    })
    __UTL_HIDE_FROM_ABI tuple(Us&&... u) = delete;
    template <typename... Us>
    requires requires {
        requires (sizeof...(Us) == sizeof...(Types));
        requires (... && is_constructible_v<Types, Us>);
        requires (... && !reference_constructs_from_temporary_v<Types, Us &&>);
    }
    __UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<Us...>) constexpr tuple(
        Us&&... u) noexcept(is_nothrow_constructible_v<base_type, Us...>)
        : base_type(__UTL forward<Us>(u)...) {}

public:
    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...>&>());
        requires (... && is_constructible_v<Types, UTypes&>);
        requires (... || reference_constructs_from_temporary_v<Types, UTypes&>);
    }
    __UTL_HIDE_FROM_ABI tuple(tuple<UTypes...>& other) noexcept = delete;

    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...>&>());
        requires (... && is_constructible_v<Types, UTypes&>);
        requires (... && !reference_constructs_from_temporary_v<Types, UTypes&>);
    }
    __UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes&...>) constexpr tuple(
        tuple<UTypes...>& other) noexcept((... && is_nothrow_constructible_v<Types, UTypes&>))
        : tuple(other, index_sequence_for<UTypes...>{}) {}

public:
    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...> const&>());
        requires (... && is_constructible_v<Types, UTypes const&>);
        requires (... || reference_constructs_from_temporary_v<Types, UTypes const&>);
    }
    __UTL_HIDE_FROM_ABI tuple(tuple<UTypes...> const& other) noexcept = delete;

    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...> const&>());
        requires (... && is_constructible_v<Types, UTypes const&>);
        requires (... && !reference_constructs_from_temporary_v<Types, UTypes const&>);
    }
    __UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes const&...>) constexpr tuple(
        tuple<UTypes...> const& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes const&>))
        : tuple(other, index_sequence_for<UTypes...>{}) {}

public:
    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...> &&>());
        requires (... && is_constructible_v<Types, UTypes>);
        requires (... || reference_constructs_from_temporary_v<Types, UTypes &&>);
    }
    __UTL_HIDE_FROM_ABI tuple(tuple<UTypes...>&& other) noexcept = delete;

    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...> &&>());
        requires (... && is_constructible_v<Types, UTypes>);
        requires (... && !reference_constructs_from_temporary_v<Types, UTypes &&>);
    }
    __UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes...>) constexpr tuple(
        tuple<UTypes...>&& other) noexcept((... && is_nothrow_constructible_v<Types, UTypes>))
        : tuple(__UTL move(other), index_sequence_for<UTypes...>{}) {}

public:
    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...> &&>());
        requires (... && is_constructible_v<Types, UTypes>);
        requires (... || reference_constructs_from_temporary_v<Types, UTypes const &&>);
    }
    __UTL_HIDE_FROM_ABI tuple(tuple<UTypes...> const&& other) noexcept = delete;

    template <typename... UTypes>
    requires requires {
        requires (sizeof...(UTypes) == sizeof...(Types));
        requires (!use_other_overload<tuple<UTypes...> const &&>());
        requires (!same_as<tuple<UTypes...>, tuple>);
        requires (... && is_constructible_v<Types, UTypes const>);
        requires (... && !reference_constructs_from_temporary_v<Types, UTypes const &&>);
    }
    __UTL_HIDE_FROM_ABI explicit(explicit_constructible_v<UTypes const...>) constexpr tuple(
        tuple<UTypes...> const&& other) noexcept((... &&
        is_nothrow_constructible_v<Types, UTypes const>))
        : tuple(__UTL move(other), index_sequence_for<UTypes...>{}) {}

private:
    template <details::tuple::unrecognized TupleLike>
    static constexpr bool is_explicit_v = is_explicit_constructible_v<tuple,
        decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>;

public:
    template <details::tuple::unrecognized TupleLike>
    requires requires {
        requires (!use_other_overload<TupleLike &&>());
        requires is_constructible_v<tuple,
            decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>;
    }
    __UTL_HIDE_FROM_ABI explicit(is_explicit_v<TupleLike>) constexpr tuple(TupleLike&& t) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<tuple,
            decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>)
        : tuple(details::tuple::forward_unrecognized(__UTL forward<TupleLike>(t))) {}

private:
    template <allocator_type Alloc, typename... Args>
    static consteval bool is_nothrow_constructible_with_allocator() {
        if constexpr (sizeof...(Types) != sizeof...(Args)) {
            return false;
        } else {
            return (
                ... && is_nothrow_constructible_v<base_type, allocator_arg_t, Alloc const&, Args>);
        }
    }

    template <allocator_type Alloc, typename... Args>
    static consteval bool is_constructible_with_allocator() {
        if constexpr (sizeof...(Types) != sizeof...(Args)) {
            return false;
        } else {
            return (... || is_constructible_with_allocator_v<Types, Alloc, Args>);
        }
    }

    template <allocator_type Alloc, details::tuple::unrecognized TupleLike>
    static consteval bool is_explicit_tuple_with_alloc() {
        return is_explicit_constructible_v<tuple, Alloc const&,
            decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>;
    }

    template <allocator_type Alloc, typename... Args>
    static consteval bool is_explicit_with_allocator() {
        if constexpr (sizeof...(Types) != sizeof...(Args)) {
            return false;
        } else {
            return (... ||
                (is_explicit_constructible_with_allocator_v<Types, Alloc, Args> ||
                    is_explicit_constructible_v<Types, Args>));
        }
    }

    template <allocator_type Alloc, typename... T>
    static consteval bool dangling_construction() {
        return (... ||
            (!uses_allocator_v<Types, Alloc> && reference_constructs_from_temporary_v<Types, T&&>));
    }

public:
    template <allocator_type Alloc>
    requires (... || is_constructible_with_allocator_v<Types, Alloc>)
    __UTL_HIDE_FROM_ABI explicit(is_explicit_with_allocator<Alloc>()) constexpr tuple(
        allocator_arg_t, Alloc const& alloc) noexcept(is_nothrow_constructible_v<base_type,
        allocator_arg_t, Alloc const&>)
        : base_type(allocator_arg, alloc) {}

public:
    template <allocator_type Alloc>
    requires requires {
        requires is_constructible_with_allocator<Alloc, Types const&...>();
        requires !dangling_construction<Alloc, Types const&...>();
    }
    __UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_allocator<Alloc, Types const&...>()) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        Types const&... args) noexcept(is_nothrow_constructible_with_allocator<Alloc,
        Types const&...>())
        : base_type(allocator_arg, alloc, args...) {}

public:
    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes...>();
        requires !dangling_construction<Alloc, UTypes...>();
    }
    __UTL_HIDE_FROM_ABI explicit(is_explicit_with_allocator<Alloc, UTypes...>()) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        UTypes&&... args) noexcept(is_nothrow_constructible_with_allocator<Alloc, UTypes...>())
        : base_type(allocator_arg, alloc, forward<UTypes>(args)...) {}

    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes...>();
        requires dangling_construction<Alloc, UTypes...>();
    }
    __UTL_HIDE_FROM_ABI tuple(allocator_arg_t, Alloc const& alloc, UTypes&&... args) = delete;

public:
    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes&...>();
        requires !dangling_construction<Alloc, UTypes&...>();
    }
    __UTL_HIDE_FROM_ABI explicit(is_explicit_with_allocator<Alloc, UTypes&...>()) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&
            other) noexcept(is_nothrow_constructible_with_allocator<Alloc, UTypes&...>())
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes&...>();
        requires dangling_construction<Alloc, UTypes&...>();
    }
    __UTL_HIDE_FROM_ABI tuple(
        allocator_arg_t, Alloc const& alloc, tuple<UTypes...>& other) = delete;

public:
    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes const&...>();
        requires !dangling_construction<Alloc, UTypes const&...>();
    }
    __UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_allocator<Alloc, UTypes const&...>()) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple<UTypes...> const&
            other) noexcept(is_nothrow_constructible_with_allocator<Alloc, UTypes const&...>())
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes const&...>();
        requires dangling_construction<Alloc, UTypes const&...>();
    }
    __UTL_HIDE_FROM_ABI tuple(
        allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const& other) = delete;

public:
    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes...>();
        requires !dangling_construction<Alloc, UTypes...>();
    }
    __UTL_HIDE_FROM_ABI explicit(is_explicit_with_allocator<Alloc, UTypes...>()) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple<UTypes...>&&
            other) noexcept(is_nothrow_constructible_with_allocator<Alloc, UTypes...>())
        : tuple(allocator_arg, alloc, __UTL move(other), index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes...>();
        requires dangling_construction<Alloc, UTypes...>();
    }
    __UTL_HIDE_FROM_ABI tuple(
        allocator_arg_t, Alloc const& alloc, tuple<UTypes...>&& other) = delete;

public:
    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes const...>();
        requires !dangling_construction<Alloc, UTypes const...>();
    }
    __UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_allocator<Alloc, UTypes const...>()) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple<UTypes...> const&&
            other) noexcept(is_nothrow_constructible_with_allocator<Alloc, UTypes const...>())
        : tuple(allocator_arg, alloc, other, index_sequence_for<UTypes...>{}) {}

    template <allocator_type Alloc, typename... UTypes>
    requires requires {
        requires is_constructible_with_allocator<Alloc, UTypes const...>();
        requires dangling_construction<Alloc, UTypes const...>();
    }
    __UTL_HIDE_FROM_ABI tuple(
        allocator_arg_t, Alloc const& alloc, tuple<UTypes...> const&& other) = delete;

public:
    template <allocator_type Alloc, details::tuple::unrecognized TupleLike>
    requires (is_constructible_v<tuple, Alloc const&,
        decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>)
    __UTL_HIDE_FROM_ABI explicit(is_explicit_tuple_with_alloc<Alloc, TupleLike>()) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        TupleLike&& other) noexcept(details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_constructible_v<tuple, Alloc const&,
            decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>)
        : tuple(allocator_arg, alloc, __UTL forward<TupleLike>(other),
              index_sequence_for<Types...>{}) {}

public:
    template <allocator_type Alloc>
    requires requires {
        requires is_constructible_with_allocator<Alloc, Types const&...>();
        requires !dangling_construction<Alloc, Types const&...>();
    }
    __UTL_HIDE_FROM_ABI explicit(
        is_explicit_with_allocator<Alloc, Types const&...>()) constexpr tuple(allocator_arg_t,
        Alloc const& alloc,
        tuple const&
            other) noexcept(is_nothrow_constructible_with_allocator<Alloc, Types const&...>())
        : tuple(allocator_arg, alloc, other, index_sequence_for<Types...>{}) {}

    template <allocator_type Alloc>
    requires requires {
        requires is_constructible_with_allocator<Alloc, Types...>();
        requires !dangling_construction<Alloc, Types...>();
    }
    __UTL_HIDE_FROM_ABI explicit(is_explicit_with_allocator<Alloc, Types&&...>()) constexpr tuple(
        allocator_arg_t, Alloc const& alloc,
        tuple&& other) noexcept(is_nothrow_constructible_with_allocator<Alloc, Types...>())
        : tuple(allocator_arg, alloc, __UTL move(other), index_sequence_for<Types...>{}) {}

public:
    template <assignable_to<Types&, details::tuple::add_const_lvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...> const& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes const&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <assignable_to<Types const&, details::tuple::add_const_lvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...> const& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes const&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <assignable_to<Types&, add_lvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...>& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

    template <assignable_to<Types const&, add_lvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...>& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes&>)) {
        return assign(other, index_sequence_for<Types...>{});
    }

public:
    template <assignable_to<Types&, add_rvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...>&& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes&&>)) {
        return assign(__UTL move(other), index_sequence_for<Types...>{});
    }

public:
    template <assignable_to<Types const&, add_rvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...>&& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes&&>)) {
        return assign(__UTL move(other), index_sequence_for<Types...>{});
    }

public:
    template <assignable_to<Types&, details::tuple::add_const_rvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple& operator=(tuple<UTypes...> const&& other) noexcept(
        (... && is_nothrow_assignable_v<Types&, UTypes const&&>)) {
        return assign(__UTL move(other), index_sequence_for<Types...>{});
    }

public:
    template <assignable_to<Types const&, details::tuple::add_const_rvalue_reference>... UTypes>
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(tuple<UTypes...> const&& other) const
        noexcept((... && is_nothrow_assignable_v<Types const&, UTypes const&&>)) {
        return assign(__UTL move(other), index_sequence_for<Types...>{});
    }

public:
    template <details::tuple::unrecognized TupleLike>
    requires (is_assignable_v<tuple&,
        decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>)
    __UTL_HIDE_FROM_ABI constexpr tuple& operator=(TupleLike&& t) noexcept(
        details::tuple::is_nothrow_accessible_v<TupleLike> &&
        is_nothrow_assignable_v<tuple&,
            decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>) {
        return *this = details::tuple::forward_unrecognized(__UTL forward<TupleLike>(t));
    }

    template <details::tuple::unrecognized TupleLike>
    requires (is_assignable_v<tuple const&,
        decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>)
    __UTL_HIDE_FROM_ABI constexpr tuple const& operator=(TupleLike&& t) const
        noexcept(details::tuple::is_nothrow_accessible_v<TupleLike> &&
            is_nothrow_assignable_v<tuple const&,
                decltype(details::tuple::forward_unrecognized(__UTL declval<TupleLike>()))>) {
        return *this = details::tuple::forward_unrecognized(__UTL forward<TupleLike>(t));
    }
};

namespace details {
namespace tuple {

template <tuple_like T, tuple_like U, size_t... Is>
__UTL_HIDE_FROM_ABI auto three_way_result(T const& l, U const& r, index_sequence<Is...>) noexcept
    -> common_comparison_category_t<decltype(__UTL get_element<Is>(l) <=>
        __UTL get_element<Is>(r))...>;

template <tuple_like T, tuple_like U, size_t... Is>
__UTL_HIDE_FROM_ABI auto is_nothrow_three_way(
    T const& l, U const& r, index_sequence<Is...>) noexcept
    -> bool_constant<(...&& noexcept(__UTL get_element<Is>(l) <=> __UTL get_element<Is>(r)))>;

template <typename T, typename U>
requires (tuple_like<remove_reference_t<T>> && tuple_like<remove_reference_t<U>>)
using three_way_result_t UTL_NODEBUG =
    decltype(three_way_result(__UTL declval<T>(), __UTL declval<U>(), tuple_index_sequence<T>{}));

template <typename T, typename U>
requires (tuple_like<remove_reference_t<T>> && tuple_like<remove_reference_t<U>>)
UTL_INLINE_CXX17 constexpr bool is_nothrow_three_way_v = decltype(is_nothrow_three_way(
    __UTL declval<T>(), __UTL declval<U>(), tuple_index_sequence<T>{}))::value;

template <size_t I, tuple_like T, tuple_like U>
requires (I == tuple_size_v<T>)
UTL_ATTRIBUTES(CONST, _HIDE_FROM_ABI) constexpr three_way_result_t<T, U> three_way(
    T const&, U const&) noexcept {
    return three_way_result_t<T, U>::equal;
}

template <size_t I, tuple_like T, tuple_like U>
requires (I < tuple_size_v<T>)
__UTL_HIDE_FROM_ABI constexpr three_way_result_t<T, U> three_way(T const& l, U const& r) noexcept(
    is_nothrow_three_way_v<T, U>) {
    three_way_result_t<T, U> const c = __UTL get_element<I>(l) <=> __UTL get_element<I>(r);
    return c != 0 ? c : three_way<I + 1>(l, r);
}

template <tuple_like T, tuple_like U>
__UTL_HIDE_FROM_ABI constexpr three_way_result_t<T, U> three_way(T const& l, U const& r) noexcept(
    noexcept(three_way<0>(l, r))) {
    return three_way<0>(l, r);
}

template <size_t I, tuple_like T, tuple_like U>
requires (I == tuple_size_v<T>)
UTL_ATTRIBUTES(CONST, _HIDE_FROM_ABI) constexpr bool equals(T const& l, U const& r) noexcept {
    return true;
}

template <size_t I, tuple_like T, tuple_like U>
requires (I < tuple_size_v<T>)
__UTL_HIDE_FROM_ABI constexpr bool equals(
    T const& l, U const& r) noexcept(conjunction<details::tuple::is_all_nothrow_gettable<T>,
    details::tuple::is_all_nothrow_gettable<U>, compare_ops::all_have_nothrow_eq<T, U>>::value) {
    return (__UTL get_element<I>(l) == __UTL get_element<I>(r)) && equals<I + 1>(l, r);
}

template <tuple_like T, tuple_like U>
UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) inline constexpr bool equals(T const& l, U const& r) noexcept(
    noexcept(equals<0>(l, r))) {
    static_assert(compare_ops::all_have_eq<T, U>::value, "All elements must be comparable");
    return equals<0>(l, r);
}

template <size_t I, tuple_like T, tuple_like U>
requires (I == tuple_size_v<T>)
UTL_ATTRIBUTES(CONST, _HIDE_FROM_ABI) constexpr bool less(T const& l, U const& r) noexcept {
    return false;
}

template <size_t I, tuple_like T, tuple_like U>
requires (I < tuple_size_v<T>)
__UTL_HIDE_FROM_ABI constexpr bool less(T const& l, U const& r) noexcept(
    conjunction_v<compare_ops::all_have_nothrow_eq<T, U>, compare_ops::all_have_nothrow_lt<T, U>>) {
    return (__UTL get_element<I - 1>(l) == __UTL get_element<I - 1>(r)) &&
        ((__UTL get_element<I>(l) < __UTL get_element<I>(r)) || less<I + 1>(l, r));
}

template <tuple_like T, tuple_like U>
UTL_ATTRIBUTES(FLATTEN, _HIDE_FROM_ABI) constexpr bool less(T const& l, U const& r) noexcept(
    conjunction_v<compare_ops::all_have_nothrow_eq<T, U>, compare_ops::all_have_nothrow_lt<T, U>>) {
    static_assert(compare_ops::all_have_eq<T, U>::value, "All elements must be comparable");
    static_assert(compare_ops::all_have_lt<T, U>::value, "All elements must be comparable");
    return (__UTL get_element<0>(l) < __UTL get_element<0>(r)) || less<1>(l, r);
}

} // namespace tuple
} // namespace details

template <typename... Ts, equality_comparable_with<Ts>... Us>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator==(
    tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(details::tuple::equals(l, r))) {
    return details::tuple::equals(l, r);
}

UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator==(tuple<> const&, tuple<> const&) noexcept {
    return true;
}

template <typename... Ts, equality_comparable_with<Ts>... Us>
requires requires { (..., (__UTL declval<Ts const&>() < __UTL declval<Us const&>())); }
UTL_ATTRIBUTES(NODISCARD, FLATTEN, _HIDE_FROM_ABI) constexpr bool operator<(
    tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(details::tuple::less(l, r)) {
    return details::tuple::less(l, r);
}

template <typename... Ts, equality_comparable_with<Ts>... Us>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator!=(
    tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(l == r))) {
    return !(l == r);
}

template <typename... Ts, typename... Us>
requires (requires(tuple<Ts...> const& l, tuple<Us...> const& r) { r < l; })
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator<=(
    tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(r < l))) {
    return !(r < l) && true;
}

template <typename... Ts, typename... Us>
requires (requires(tuple<Ts...> const& l, tuple<Us...> const& r) { r < l; })
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator>(
    tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(r < l)) {
    return r < l;
}

template <typename... Ts, typename... Us>
requires (requires(tuple<Ts...> const& l, tuple<Us...> const& r) { l < r; })
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator>=(
    tuple<Ts...> const& l, tuple<Us...> const& r) noexcept(noexcept(!(l < r))) {
    return !(l < r);
}

UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator!=(tuple<> const&, tuple<> const&) noexcept {
    return false;
}

UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator<(tuple<> const&, tuple<> const&) noexcept {
    return false;
}

UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator>(tuple<> const&, tuple<> const&) noexcept {
    return false;
}

UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator<=(tuple<> const&, tuple<> const&) noexcept {
    return true;
}

UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr bool operator>=(tuple<> const&, tuple<> const&) noexcept {
    return true;
}

template <typename... Ts, three_way_comparable_with<Ts>... Us>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr auto operator<=>(tuple<Ts...> const& l,
    tuple<Us...> const& r) noexcept(noexcept(details::tuple::three_way(l, r))) {
    return details::tuple::three_way(l, r);
}

template <same_as<tuple<>> U, same_as<::std::strong_ordering> R = ::std::strong_ordering>
UTL_ATTRIBUTES(NODISCARD, _HIDE_FROM_ABI) constexpr R operator<=>(U, U) noexcept {
    return R::equal;
}

UTL_NAMESPACE_END
