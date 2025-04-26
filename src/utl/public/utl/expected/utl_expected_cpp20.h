// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if !defined(UTL_EXPECTED_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif
#if !UTL_CXX20
#  error "Invalid header accessed"
#endif

#include "utl/initializer_list/utl_initializer_list_fwd.h"

#include "utl/concepts/utl_boolean_testable.h"
#include "utl/concepts/utl_constructible_from.h"
#include "utl/concepts/utl_convertible_to.h"
#include "utl/concepts/utl_different_from.h"
#include "utl/concepts/utl_invocable.h"
#include "utl/concepts/utl_same_as.h"
#include "utl/concepts/utl_void_type.h"
#include "utl/expected/utl_bad_expected_access.h"
#include "utl/expected/utl_expected_common.h"
#include "utl/expected/utl_expected_details_cpp20.h"
#include "utl/expected/utl_unexpected.h"
#include "utl/functional/utl_invoke.h"
#include "utl/type_traits/utl_constants.h"
#include "utl/type_traits/utl_decay.h"
#include "utl/type_traits/utl_declval.h"
#include "utl/type_traits/utl_enable_if.h"
#include "utl/type_traits/utl_invoke.h"
#include "utl/type_traits/utl_is_assignable.h"
#include "utl/type_traits/utl_is_boolean.h"
#include "utl/type_traits/utl_is_complete.h"
#include "utl/type_traits/utl_is_constructible.h"
#include "utl/type_traits/utl_is_convertible.h"
#include "utl/type_traits/utl_is_copy_assignable.h"
#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_destructible.h"
#include "utl/type_traits/utl_is_equality_comparable.h"
#include "utl/type_traits/utl_is_function.h"
#include "utl/type_traits/utl_is_move_assignable.h"
#include "utl/type_traits/utl_is_move_constructible.h"
#include "utl/type_traits/utl_is_nothrow_assignable.h"
#include "utl/type_traits/utl_is_nothrow_constructible.h"
#include "utl/type_traits/utl_is_nothrow_convertible.h"
#include "utl/type_traits/utl_is_nothrow_copy_assignable.h"
#include "utl/type_traits/utl_is_nothrow_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_assignable.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_is_same.h"
#include "utl/type_traits/utl_is_void.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_cvref.h"
#include "utl/utility/utl_forward.h"
#include "utl/utility/utl_in_place.h"
#include "utl/utility/utl_move.h"

#define __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE \
    (ALWAYS_INLINE)(PURE)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#define __UTL_ATTRIBUTE_EXPECTED_INLINE_CONST \
    (ALWAYS_INLINE)(CONST)(NODISCARD)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_CONST
#define __UTL_ATTRIBUTE_GETTER (ALWAYS_INLINE)(NODISCARD) __UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER

UTL_NAMESPACE_BEGIN

template <typename T, typename E>
class __UTL_PUBLIC_TEMPLATE expected : private details::expected::storage_base<T, E> {
    static_assert(is_destructible_v<T>, "Invalid value type");
    static_assert(!is_reference_v<T>, "Invalid value type");
    static_assert(!is_function_v<T>, "Invalid value type");
    static_assert(!__UTL_TRAIT_unexpect_tag(remove_cvref_t<T>), "Invalid value type");
    static_assert(!__UTL_TRAIT_in_place_tag(remove_cvref_t<T>), "Invalid value type");
    static_assert(!__UTL_TRAIT_is_unexpected(remove_cvref_t<T>), "Invalid value type");
    static_assert(UTL_TRAIT_is_complete(unexpected<E>), "Invalid error type");
    using base_type = details::expected::storage_base<T, E>;

    template <typename T1, typename E1, typename T1Qual, typename E1Qual>
    using can_convert = conjunction<is_constructible<T, T1Qual>, is_constructible<E, E1Qual>,
        conditional_t<!UTL_TRAIT_is_boolean(T),
            negation<
                disjunction<bool_constant<UTL_TRAIT_is_same(T, T1) && UTL_TRAIT_is_same(E, E1)>,
                    is_constructible<T, expected<T1, E1>&>, is_convertible<expected<T1, E1>&, T>,
                    is_constructible<T, expected<T1, E1>>, is_convertible<expected<T1, E1>, T>,
                    is_constructible<T, expected<T1, E1> const&>,
                    is_convertible<expected<T1, E1> const&, T>,
                    is_constructible<T, expected<T1, E1> const>,
                    is_convertible<expected<T1, E1> const, T>>>,
            true_type>,
        negation<disjunction<is_constructible<unexpected<E>, expected<T1, E1>&>,
            is_constructible<unexpected<E>, expected<T1, E1>>,
            is_constructible<unexpected<E>, expected<T1, E1> const&>,
            is_constructible<unexpected<E>, expected<T1, E1> const>>>>;

    template <typename U> /* __UTL details::is_unexpected_type_v<U> */
    using unexpected_error_type = decltype(__UTL declval<U>().error());

    template <typename T1, typename E1>
    friend class expected;

    using base_type::base_type;

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;
    template <typename U>
    using rebind = expected<U, error_type>;

    __UTL_HIDE_FROM_ABI explicit(is_explicit_constructible_v<
        T>) inline constexpr expected() noexcept(is_nothrow_default_constructible_v<T>)
    requires is_default_constructible_v<T>
        : base_type{__UTL in_place} {}
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected const& other) noexcept(
        is_nothrow_copy_constructible_v<base_type>) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected&& other) noexcept(
        is_nothrow_move_constructible_v<base_type>) = default;

    template <typename T1, typename E1>
    requires can_convert<T1, E1, T1 const&, E1 const&>::value
    __UTL_HIDE_FROM_ABI explicit(!is_convertible_v<T1 const&, T> ||
        !is_convertible_v<E1 const&, E>) inline constexpr expected(expected<T1, E1> const&
            other) noexcept(is_nothrow_constructible_v<T, T1 const&> &&
        is_nothrow_constructible_v<E, E1 const&>)
        : base_type(__UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1>
    requires can_convert<T1, E1, T1, E1>::value
    __UTL_HIDE_FROM_ABI explicit(!is_convertible_v<T1, T> ||
        !is_convertible_v<E1, E>) inline constexpr expected(expected<T1, E1>&&
            other) noexcept(is_nothrow_constructible_v<T, T1> && is_nothrow_constructible_v<E, E1>)
        : base_type(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

    template <typename U = T>
    requires (constructible_from<T, U> && !__UTL_TRAIT_in_place_tag(remove_cvref_t<U>) &&
        different_from<expected, remove_cvref_t<U>> &&
        !details::is_unexpected_type_v<remove_cvref_t<U>> &&
        (!is_boolean_v<T> || !__UTL_TRAIT_is_expected(remove_cvref_t<U>)))
    __UTL_HIDE_FROM_ABI explicit(!is_convertible_v<U, T>) inline constexpr expected(
        U&& value) noexcept(is_nothrow_constructible_v<T, U>)
        : base_type(__UTL in_place, __UTL forward<U>(value)) {}

    template <typename U>
    requires details::is_unexpected_type_v<remove_cvref_t<U>> &&
        constructible_from<E, unexpected_error_type<U>>
    __UTL_HIDE_FROM_ABI explicit(
        !is_convertible_v<unexpected_error_type<U>, E>) inline constexpr expected(U&&
            e) noexcept(is_nothrow_constructible_v<E, unexpected_error_type<U>>)
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    template <typename... Args>
    requires constructible_from<T, Args...>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(in_place_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<T, Args...>)
        : base_type(__UTL in_place, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args>
    requires constructible_from<T, ::std::initializer_list<U>&, Args...>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(in_place_t,
        ::std::initializer_list<U> il, Args&&... args) noexcept(is_nothrow_constructible_v<T,
        ::std::initializer_list<U>&, Args...>)
        : base_type(__UTL in_place, il, __UTL forward<Args>(args)...) {}

    template <typename... Args>
    requires constructible_from<E, Args...>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t, Args&&... args) noexcept(
        UTL_TRAIT_is_nothrow_constructible(E, Args...))
        : base_type(__UTL unexpect, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args>
    requires constructible_from<E, ::std::initializer_list<U>&, Args...>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t,
        ::std::initializer_list<U> il, Args&&... args) noexcept(is_nothrow_constructible_v<E,
        ::std::initializer_list<U>&, Args...>)
        : base_type(__UTL unexpect, il, __UTL forward<Args>(args)...) {}

    __UTL_HIDE_FROM_ABI inline ~expected() noexcept = default;

    __UTL_HIDE_FROM_ABI inline constexpr expected& operator=(expected const&) noexcept(
        is_nothrow_copy_assignable_v<T> && is_nothrow_copy_assignable_v<E>) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected& operator=(expected&&) noexcept(
        is_nothrow_move_assignable_v<T> && is_nothrow_move_assignable_v<E>) = default;

    template <typename U = T>
    requires (!same_as<expected, remove_cvref_t<U>> &&
        !details::is_unexpected_type_v<remove_cvref_t<U>> && constructible_from<T, U> &&
        assignable_from<T&, U> &&
        (is_nothrow_constructible_v<T, U> || is_nothrow_move_constructible_v<T> ||
            is_nothrow_move_constructible_v<E>))
    __UTL_HIDE_FROM_ABI inline constexpr expected& operator=(U&& u) noexcept(
        is_nothrow_assignable_v<T&, U> && is_nothrow_constructible_v<T, U>) {
        if (has_value()) {
            this->value_ref() = __UTL forward<U>(u);
        } else {
            this->reinitialize_as_value(__UTL forward<U>(u));
        }

        return *this;
    }

    template <typename U>
    requires details::is_unexpected_type<remove_cvref_t<U>>::value &&
        constructible_from<E, unexpected_error_type<U>> &&
        assignable_from<E&, unexpected_error_type<U>> &&
        (is_nothrow_constructible_v<E, unexpected_error_type<U>> ||
            is_nothrow_move_constructible_v<T> || is_nothrow_move_constructible_v<E>)
    __UTL_HIDE_FROM_ABI inline constexpr expected& operator=(U&& u) noexcept(
        is_nothrow_assignable_v<E&, unexpected_error_type<U>> &&
        is_nothrow_constructible_v<E, unexpected_error_type<U>>) {
        if (!has_value()) {
            this->error_ref() = __UTL forward_like<U>(u.error());
        } else {
            this->reinitialize_as_error(__UTL forward_like<U>(u.error()));
        }

        return *this;
    }

    using base_type::has_value;

    UTL_ATTRIBUTE(GETTER) inline constexpr T const& value() const& UTL_THROWS {
        static_assert(is_copy_constructible_v<E>, "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return this->value_ref();
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T& value() & UTL_THROWS {
        static_assert(is_copy_constructible_v<E>, "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return this->value_ref();
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T const&& value() const&& UTL_THROWS {
        static_assert(is_copy_constructible_v<E> ||
                is_constructible_v<E, decltype(__UTL move(this->error_ref()))>,
            "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return __UTL move(this->value_ref());
    }
    UTL_ATTRIBUTE(GETTER) inline constexpr T&& value() && UTL_THROWS {
        static_assert(is_copy_constructible_v<E> ||
                is_constructible_v<E, decltype(__UTL move(this->error_ref()))>,
            "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
        return __UTL move(this->value_ref());
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const& error() const& noexcept {
        UTL_ASSERT(!has_value());
        return this->error_ref();
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E& error() & noexcept {
        UTL_ASSERT(!has_value());
        return this->error_ref();
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const&& error() const&& noexcept {
        UTL_ASSERT(!has_value());
        return __UTL move(this->error_ref());
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E&& error() && noexcept {
        UTL_ASSERT(!has_value());
        return __UTL move(this->error_ref());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T* operator->() noexcept {
        return this->value_ptr();
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const* operator->() const noexcept {
        return this->value_ptr();
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T& operator*() & noexcept {
        return this->value_ref();
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T&& operator*() && noexcept {
        return __UTL move(this->value_ref());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const& operator*() const& noexcept {
        return this->value_ref();
    }
    UTL_ATTRIBUTE(EXPECTED_INLINE_CONST) inline constexpr T const&& operator*() const&& noexcept {
        return __UTL move(this->value_ref());
    }

    UTL_ATTRIBUTE(EXPECTED_INLINE_PURE) inline constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    template <convertible_to<T> U>
    requires is_copy_constructible_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr T value_or(U&& default_val) const& noexcept(
        is_nothrow_convertible_v<U, T>) {
        return has_value() ? **this : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <convertible_to<T> U>
    requires is_move_constructible_v<T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr T value_or(U&& default_val) && noexcept(
        is_nothrow_convertible_v<U, T>) {
        return has_value() ? __UTL move(**this) : static_cast<T>(__UTL forward<U>(default_val));
    }

    template <convertible_to<E> U>
    requires is_copy_constructible_v<E>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) const& noexcept(
        is_nothrow_convertible_v<U, E>) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : this->error_ref();
    }

    template <convertible_to<E> U>
    requires is_move_constructible_v<E>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) && noexcept(
        is_nothrow_convertible_v<U, E>) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val))
                           : __UTL move(this->error_ref());
    }

    template <invocable<T&> F>
    requires constructible_from<E, E&>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) & noexcept(
        is_nothrow_invocable_v<F, T&> && is_nothrow_constructible_v<E, E&>)
        -> invoke_result_t<F, T&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T&>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");
        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f), this->value_ref());
        }

        return return_type{__UTL unexpect, this->error_ref()};
    }

    template <invocable<T const&> F>
    requires constructible_from<E, E const&>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const& noexcept(
        is_nothrow_invocable_v<F, T const&> && is_nothrow_constructible_v<E, E const&>)
        -> invoke_result_t<F, T const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const&>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f), this->value_ref())
                           : return_type{__UTL unexpect, this->error_ref()};
    }

    template <invocable<T> F>
    requires constructible_from<E, E>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) && noexcept(
        is_nothrow_invocable_v<F, T> && is_nothrow_constructible_v<E, E>) -> invoke_result_t<F, T> {
        using return_type = remove_cvref_t<invoke_result_t<F, T>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");

        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
        }

        return return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <invocable<T const> F>
    requires constructible_from<E, E const>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const&& noexcept(
        is_nothrow_invocable_v<F, T const> && is_nothrow_constructible_v<E, E const>)
        -> invoke_result_t<F, T const> {
        using return_type = remove_cvref_t<invoke_result_t<F, T const>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()))
                           : return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <invocable<T&> F>
    requires constructible_from<E, E&>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) &
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T&>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> || is_constructible_v<return_type, invoke_result_t<F, T&>>),
            "Invalid return type");
        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, this->error_ref()};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f), this->value_ref());
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f), this->value_ref()};
        }
    }

    template <invocable<T const&> F>
    requires constructible_from<E, E const&>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) const&
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T const&>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const&>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> ||
                    is_constructible_v<return_type, invoke_result_t<F, T const&>>),
            "Invalid return type");

        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, this->error_ref()};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f), this->value_ref());
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f), this->value_ref()};
        }
    }

    template <invocable<T> F>
    requires constructible_from<E, E>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) &&
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> || is_constructible_v<return_type, invoke_result_t<F, T>>),
            "Invalid return type");

        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{__UTL details::expected::transforming,
                __UTL forward<F>(f), __UTL move(this->value_ref())};
        }
    }

    template <invocable<T const> F>
    requires constructible_from<E, E const>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) const&&
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F, T const>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F, T const>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> ||
                    is_constructible_v<return_type, invoke_result_t<F, T const>>),
            "Invalid return type");

        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f), __UTL move(this->value_ref()));
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{__UTL details::expected::transforming,
                __UTL forward<F>(f), __UTL move(this->value_ref())};
        }
    }

    template <invocable<E&> F>
    requires constructible_from<T, T&>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) & noexcept(
        is_nothrow_invocable_v<F, E&> && is_nothrow_constructible_v<T, T&>)
        -> invoke_result_t<F, E&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E&>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::value_type, T>,
            "Invalid return type");
        if (has_value()) {
            return return_type{__UTL in_place, this->value_ref()};
        }

        return __UTL invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <invocable<E const&> F>
    requires constructible_from<T, T const&>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const& noexcept(
        is_nothrow_invocable_v<F, E const&> && is_nothrow_constructible_v<T, T const&>)
        -> invoke_result_t<F, E const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const&>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::value_type, T>,
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place, this->value_ref()};
        }

        return __UTL invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <invocable<E> F>
    requires constructible_from<T, T>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) && noexcept(
        is_nothrow_invocable_v<F, E> && is_nothrow_constructible_v<T, T>) -> invoke_result_t<F, E> {
        using return_type = remove_cvref_t<invoke_result_t<F, E>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::value_type, T>,
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place, __UTL move(this->value_ref())};
        }

        return __UTL invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <invocable<E const> F>
    requires constructible_from<T, T const>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const&& noexcept(
        is_nothrow_invocable_v<F, E const> && is_nothrow_constructible_v<T, T const>)
        -> invoke_result_t<F, E const> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::value_type, T>,
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place, __UTL move(this->value_ref())};
        }

        return __UTL invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <invocable<E&> F>
    requires constructible_from<T, T&>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) &
        UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E&>>;
        static_assert(is_complete_v<expected<T, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E&>>,
            "Invalid return type");
        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, this->value_ref()};
        }

        return expected<return_type, E>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), this->error_ref()};
    }

    template <invocable<E const&> F>
    requires constructible_from<T, T const&>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const&
        UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E const&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const&>>;
        static_assert(is_complete_v<expected<T, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E const&>>,
            "Invalid return type");

        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, this->value_ref()};
        }

        return expected<return_type, E>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), this->error_ref()};
    }

    template <invocable<E> F>
    requires constructible_from<T, T>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) &&
        UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E>>;
        static_assert(is_complete_v<expected<T, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E>>,
            "Invalid return type");

        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, __UTL move(this->value_ref())};
        }

        return expected<T, return_type>{__UTL details::expected::transforming_error,
            __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    template <invocable<E const> F>
    requires constructible_from<T, T const>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const&&
        UTL_THROWS
        -> expected<T, remove_cv_t<invoke_result_t<F, E const>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const>>;
        static_assert(is_complete_v<expected<T, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E const>>,
            "Invalid return type");

        if (has_value()) {
            return expected<T, return_type>{__UTL in_place, __UTL move(this->value_ref())};
        }

        return expected<T, return_type>{__UTL details::expected::transforming_error,
            __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    template <typename... Args>
    requires is_nothrow_constructible_v<T, Args...>
    __UTL_HIDE_FROM_ABI inline constexpr T& emplace(Args&&... args) noexcept {
        return this->emplace_value(__UTL forward<Args>(args)...);
    }

    template <typename U, typename... Args>
    requires is_nothrow_constructible_v<T, ::std::initializer_list<U>&, Args...>
    __UTL_HIDE_FROM_ABI inline constexpr T& emplace(
        ::std::initializer_list<U> il, Args&&... args) noexcept {
        return this->emplace_value(il, __UTL forward<Args>(args)...);
    }

    __UTL_HIDE_FROM_ABI inline constexpr void swap(expected& other) noexcept(
        is_nothrow_swappable_v<T> && is_nothrow_swappable_v<E> &&
        is_nothrow_move_constructible_v<T> && is_nothrow_move_constructible_v<E>)
    requires (swappable<T> && swappable<E> && is_move_constructible_v<T> &&
        is_move_constructible_v<E> &&
        (is_nothrow_move_constructible_v<T> || is_nothrow_move_constructible_v<E>))
    {
        if (this->has_value() == other.has_value()) {
            if (this->has_value()) {
                __UTL ranges::swap(this->value_ref(), other.value_ref());
            } else {
                __UTL ranges::swap(this->error_ref(), other.error_ref());
            }
        } else if (this->has_value()) {
            this->cross_swap(other);
        } else {
            other.cross_swap(*this);
        }
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr void swap(expected& left, expected& right) noexcept(
        noexcept(left.swap(right)))
    requires requires { left.swap(right); }
    {
        left.swap(right);
    }

    template <typename T2, typename E2>
    requires (!void_type<T2>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        expected<T2, E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<T, T2> &&
        is_nothrow_equality_comparable_with_v<E, E2>) {
        static_assert(is_equality_comparable_with_v<T, T2>, "Program ill-formed");
        static_assert(is_equality_comparable_with_v<E, E2>, "Program ill-formed");
        return left.has_value() == right.has_value() &&
            (left.has_value() ? left.value_ref() == right.value()
                              : left.error_ref() == right.error());
    }

    template <typename T2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        T2 const& right) noexcept(is_nothrow_equality_comparable_with_v<T, T2>) {
        static_assert(is_equality_comparable_with_v<T, T2>, "Program ill-formed");
        static_assert(boolean_testable<decltype(left.value_ref() == right)>, "Program ill-formed");
        return left.has_value() && left.value_ref() == right;
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        unexpected<E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<E, E2>) {
        static_assert(is_equality_comparable_with_v<E, E2>, "Program ill-formed");
        static_assert(
            boolean_testable<decltype(left.error_ref() == right.error())>, "Program ill-formed");
        return !left.has_value() && left.error_ref() == right.error();
    }

    template <typename T2, typename E2>
    requires (!void_type<T2>)
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        expected<T2, E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<T, T2> &&
        is_nothrow_equality_comparable_with_v<E, E2>) {
        return !(left == right);
    }

    template <typename T2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        T2 const& right) noexcept(is_nothrow_equality_comparable_with_v<T, T2>) {
        return !(left == right);
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        unexpected<E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<E, E2>) {
        return !(left == right);
    }
};

template <void_type Void, typename E>
class __UTL_PUBLIC_TEMPLATE expected<Void, E> : __UTL details::expected::void_storage_base<E> {
    static_assert(UTL_TRAIT_is_complete(unexpected<E>), "Invalid error type");
    using base_type = details::expected::void_storage_base<E>;

    template <typename T1, typename E1, typename E1Qual>
    using can_convert = conjunction<is_void<T1>, is_constructible<E, E1Qual>,
        negation<disjunction<is_constructible<unexpected<E>, expected<T1, E1>&>,
            is_constructible<unexpected<E>, expected<T1, E1>>,
            is_constructible<unexpected<E>, expected<T1, E1> const&>,
            is_constructible<unexpected<E>, expected<T1, E1> const>>>>;

    template <typename U> /* __UTL details::is_unexpected_type_v<U> */
    using unexpected_error_type = decltype(__UTL declval<U>().error());

    template <typename T1, typename E1>
    friend class expected;

public:
    using value_type = Void;
    using error_type = E;
    using unexpected_type = unexpected<E>;
    template <typename U>
    using rebind = expected<U, error_type>;

    __UTL_HIDE_FROM_ABI inline constexpr expected() noexcept = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected const& other) noexcept(
        is_nothrow_copy_constructible_v<base_type>) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected(expected&& other) noexcept(
        is_nothrow_move_constructible_v<base_type>) = default;

    template <typename T1, typename E1>
    requires can_convert<T1, E1, E1 const&>::value
    __UTL_HIDE_FROM_ABI explicit(!is_convertible_v<E1 const&, E>) inline constexpr expected(
        expected<T1, E1> const& other) noexcept(is_nothrow_constructible_v<E, E1 const&>)
        : base_type(__UTL details::expected::converting, other.has_value(), other.data_ref()) {}

    template <typename T1, typename E1>
    requires can_convert<T1, E1, E1>::value
    __UTL_HIDE_FROM_ABI explicit(!is_convertible_v<E1, E>) inline constexpr expected(
        expected<T1, E1>&& other) noexcept(is_nothrow_constructible_v<E, E1>)
        : base_type(__UTL details::expected::converting, other.has_value(),
              __UTL move(other.data_ref())) {}

    template <typename U>
    requires details::is_unexpected_type_v<remove_cvref_t<U>> &&
        constructible_from<E, unexpected_error_type<U>>
    __UTL_HIDE_FROM_ABI explicit(
        !is_convertible_v<unexpected_error_type<U>, E>) inline constexpr expected(U&&
            e) noexcept(is_nothrow_constructible_v<E, unexpected_error_type<U>>)
        : base_type(__UTL unexpect, __UTL forward_like<U>(e.error())) {}

    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(in_place_t) noexcept
        : base_type(__UTL in_place) {}

    template <typename... Args>
    requires constructible_from<E, Args...>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t, Args&&... args) noexcept(
        is_nothrow_constructible_v<E, Args...>)
        : base_type(__UTL unexpect, __UTL forward<Args>(args)...) {}

    template <typename U, typename... Args>
    requires constructible_from<E, ::std::initializer_list<U>&, Args...>
    __UTL_HIDE_FROM_ABI explicit inline constexpr expected(unexpect_t,
        ::std::initializer_list<U> il, Args&&... args) noexcept(is_nothrow_constructible_v<E,
        ::std::initializer_list<U>&, Args...>)
        : base_type(__UTL unexpect, il, __UTL forward<Args>(args)...) {}

    __UTL_HIDE_FROM_ABI inline ~expected() noexcept = default;

    __UTL_HIDE_FROM_ABI inline constexpr expected& operator=(expected const&) noexcept(
        is_nothrow_copy_assignable_v<E>) = default;
    __UTL_HIDE_FROM_ABI inline constexpr expected& operator=(expected&&) noexcept(
        is_nothrow_move_assignable_v<E>) = default;

    template <typename U>
    requires details::is_unexpected_type_v<remove_cvref_t<U>> &&
        constructible_from<E, unexpected_error_type<U>> &&
        assignable_from<E&, unexpected_error_type<U>> &&
        (is_nothrow_constructible_v<E, unexpected_error_type<U>> ||
            is_nothrow_move_constructible_v<E>)
    __UTL_HIDE_FROM_ABI inline constexpr expected& operator=(U&& u) noexcept(
        is_nothrow_assignable_v<E&, unexpected_error_type<U>> &&
        is_nothrow_constructible_v<E, unexpected_error_type<U>>) {
        if (!has_value()) {
            this->error_ref() = __UTL forward_like<U>(u.error());
        } else {
            this->reinitialize_as_error(__UTL forward_like<U>(u.error()));
        }

        return *this;
    }

    using base_type::has_value;

    __UTL_HIDE_FROM_ABI inline constexpr void value() const& UTL_THROWS {
        static_assert(is_copy_constructible_v<E>, "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
    }

    __UTL_HIDE_FROM_ABI inline constexpr void value() && UTL_THROWS {
        static_assert(is_copy_constructible_v<E> ||
                is_constructible_v<E, decltype(__UTL move(this->error_ref()))>,
            "Invalid error type");
        UTL_THROW_IF(!has_value(),
            bad_expected_access<decay_t<E>>(this->error_ref(), UTL_SOURCE_LOCATION()));
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const& error() const& noexcept {
        UTL_ASSERT(!has_value());
        return this->error_ref();
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E& error() & noexcept {
        UTL_ASSERT(!has_value());
        return this->error_ref();
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E const&& error() const&& noexcept {
        UTL_ASSERT(!has_value());
        return __UTL move(this->error_ref());
    }
    UTL_ATTRIBUTES(EXPECTED_INLINE_CONST) inline constexpr E&& error() && noexcept {
        UTL_ASSERT(!has_value());
        return __UTL move(this->error_ref());
    }

    __UTL_HIDE_FROM_ABI inline constexpr void operator*() const noexcept {}

    UTL_ATTRIBUTE(EXPECTED_INLINE_PURE) inline constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    template <convertible_to<E> U>
    requires is_copy_constructible_v<E>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) const& noexcept(
        is_nothrow_convertible_v<U, E>) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val)) : this->error_ref();
    }

    template <convertible_to<E> U>
    requires is_move_constructible_v<E>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr E error_or(U&& default_val) && noexcept(
        is_nothrow_convertible_v<U, E>) {
        return has_value() ? static_cast<E>(__UTL forward<U>(default_val))
                           : __UTL move(this->error_ref());
    }

    template <invocable F>
    requires constructible_from<E, E&>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) & noexcept(
        is_nothrow_invocable_v<F> && is_nothrow_constructible_v<E, E&>) -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");
        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f));
        }

        return return_type{__UTL unexpect, this->error_ref()};
    }

    template <invocable F>
    requires constructible_from<E, E const&>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const& noexcept(
        is_nothrow_invocable_v<F> && is_nothrow_constructible_v<E, E const&>)
        -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f))
                           : return_type{__UTL unexpect, this->error_ref()};
    }

    template <invocable F>
    requires constructible_from<E, E>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) && noexcept(
        is_nothrow_invocable_v<F> && is_nothrow_constructible_v<E, E>) -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");

        if (has_value()) {
            return __UTL invoke(__UTL forward<F>(f));
        }

        return return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <invocable F>
    requires constructible_from<E, E const>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto and_then(F&& f) const&& noexcept(
        is_nothrow_invocable_v<F> && is_nothrow_constructible_v<E, E const>) -> invoke_result_t<F> {
        using return_type = remove_cvref_t<invoke_result_t<F>>;
        static_assert(details::is_expected_type_v<return_type> &&
                same_as<typename return_type::error_type, E>,
            "Invalid return type");

        return has_value() ? __UTL invoke(__UTL forward<F>(f))
                           : return_type{__UTL unexpect, __UTL move(this->error_ref())};
    }

    template <invocable F>
    requires constructible_from<E, E&>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) &
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> || is_constructible_v<return_type, invoke_result_t<F>>),
            "Invalid return type");
        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, this->error_ref()};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }

    template <invocable F>
    requires constructible_from<E, E const&>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) const&
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> || is_constructible_v<return_type, invoke_result_t<F>>),
            "Invalid return type");

        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, this->error_ref()};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }

    template <invocable F>
    requires constructible_from<E, E>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) &&
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> || is_constructible_v<return_type, invoke_result_t<F>>),
            "Invalid return type");

        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }

    template <invocable F>
    requires constructible_from<E, E const>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform(F&& f) const&&
        UTL_THROWS
        -> expected<remove_cv_t<invoke_result_t<F>>, E> {
        using return_type = remove_cv_t<invoke_result_t<F>>;
        static_assert(is_complete_v<expected<return_type, E>> &&
                (is_void_v<return_type> || is_constructible_v<return_type, invoke_result_t<F>>),
            "Invalid return type");

        if (!has_value()) {
            return expected<return_type, E>{__UTL unexpect, __UTL move(this->error_ref())};
        }

        if constexpr (is_void_v<return_type>) {
            __UTL invoke(__UTL forward<F>(f));
            return expected<return_type, E>{};
        } else {
            return expected<return_type, E>{
                __UTL details::expected::transforming, __UTL forward<F>(f)};
        }
    }

    template <invocable<E&> F>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) & noexcept(
        is_nothrow_invocable_v<F, E&>) -> invoke_result_t<F, E&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E&>>;
        static_assert(
            details::is_expected_type_v<return_type> && is_void_v<typename return_type::value_type>,
            "Invalid return type");
        if (has_value()) {
            return return_type{__UTL in_place};
        }

        return __UTL invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <invocable<E const&> F>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const& noexcept(
        is_nothrow_invocable_v<F, E const&>) -> invoke_result_t<F, E const&> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const&>>;
        static_assert(
            details::is_expected_type_v<return_type> && is_void_v<typename return_type::value_type>,
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place};
        }

        return __UTL invoke(__UTL forward<F>(f), this->error_ref());
    }

    template <invocable<E> F>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) && noexcept(
        is_nothrow_invocable_v<F, E>) -> invoke_result_t<F, E> {
        using return_type = remove_cvref_t<invoke_result_t<F, E>>;
        static_assert(
            details::is_expected_type_v<return_type> && is_void_v<typename return_type::value_type>,
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place};
        }

        return __UTL invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <invocable<E const> F>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto or_else(F&& f) const&& noexcept(
        is_nothrow_invocable_v<F, E const>) -> invoke_result_t<F, E const> {
        using return_type = remove_cvref_t<invoke_result_t<F, E const>>;
        static_assert(
            details::is_expected_type_v<return_type> && is_void_v<typename return_type::value_type>,
            "Invalid return type");

        if (has_value()) {
            return return_type{__UTL in_place};
        }

        return __UTL invoke(__UTL forward<F>(f), __UTL move(this->error_ref()));
    }

    template <invocable<E&> F>
        UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) &
        UTL_THROWS
        -> expected<Void, remove_cv_t<invoke_result_t<F, E&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E&>>;
        static_assert(is_complete_v<expected<Void, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E&>>,
            "Invalid return type");
        if (has_value()) {
            return expected<Void, return_type>{};
        }

        return expected<return_type, E>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), this->error_ref()};
    }

    template <invocable<E const&> F>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const&
        UTL_THROWS
        -> expected<Void, remove_cv_t<invoke_result_t<F, E const&>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const&>>;
        static_assert(is_complete_v<expected<Void, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E const&>>,
            "Invalid return type");

        if (has_value()) {
            return expected<Void, return_type>{};
        }

        return expected<return_type, E>{
            __UTL details::expected::transforming_error, __UTL forward<F>(f), this->error_ref()};
    }

    template <invocable<E> F>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) &&
        UTL_THROWS
        -> expected<Void, remove_cv_t<invoke_result_t<F, E>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E>>;
        static_assert(is_complete_v<expected<Void, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E>>,
            "Invalid return type");

        if (has_value()) {
            return expected<Void, return_type>{};
        }

        return expected<Void, return_type>{__UTL details::expected::transforming_error,
            __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    template <invocable<E const> F>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) inline constexpr auto transform_error(F&& f) const&&
        UTL_THROWS
        -> expected<Void, remove_cv_t<invoke_result_t<F, E const>>> {
        using return_type = remove_cv_t<invoke_result_t<F, E const>>;
        static_assert(is_complete_v<expected<Void, return_type>> &&
                is_constructible_v<return_type, invoke_result_t<F, E const>>,
            "Invalid return type");

        if (has_value()) {
            return expected<Void, return_type>{};
        }

        return expected<Void, return_type>{__UTL details::expected::transforming_error,
            __UTL forward<F>(f), __UTL move(this->error_ref())};
    }

    __UTL_HIDE_FROM_ABI inline constexpr void emplace() noexcept { this->emplace_value(); }

    __UTL_HIDE_FROM_ABI inline constexpr void swap(expected& other) noexcept(
        is_nothrow_swappable_v<E> && is_nothrow_move_constructible_v<E>)
    requires (swappable<E> && is_move_constructible_v<E>)
    {
        if (this->has_value() == other.has_value()) {
            if (!this->has_value()) {
                __UTL ranges::swap(this->value_ref(), other.value_ref());
            } else {
                __UTL ranges::swap(this->error_ref(), other.error_ref());
            }
        } else if (this->has_value()) {
            this->cross_swap(other);
        } else {
            other.cross_swap(*this);
        }
    }

    __UTL_HIDE_FROM_ABI friend inline constexpr void swap(expected& left, expected& right) noexcept(
        noexcept(left.swap(right)))
    requires requires { left.swap(right); }
    {
        left.swap(right);
    }

    template <void_type T2, typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        expected<T2, E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<E, E2>) {
        static_assert(is_equality_comparable_with_v<E, E2>, "Program ill-formed");
        return left.has_value() == right.has_value() &&
            (left.has_value() || left.error_ref() == right.error());
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator==(expected const& left,
        unexpected<E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<E, E2>) {
        static_assert(is_equality_comparable_with_v<E, E2>, "Program ill-formed");
        static_assert(
            boolean_testable<decltype(left.error_ref() == right.error())>, "Program ill-formed");
        return !left.has_value() && left.error_ref() == right.error();
    }

    template <void_type T2, typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        expected<T2, E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<E, E2>) {
        return !(left == right);
    }

    template <typename E2>
    UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) friend constexpr bool operator!=(expected const& left,
        unexpected<E2> const& right) noexcept(is_nothrow_equality_comparable_with_v<E, E2>) {
        return !(left == right);
    }
};

UTL_NAMESPACE_END

#undef __UTL_ATTRIBUTE_GETTER
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_GETTER
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_PURE
#undef __UTL_ATTRIBUTE_EXPECTED_INLINE_PURE
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_EXPECTED_INLINE_CONST
#undef __UTL_ATTRIBUTE_EXPECTED_INLINE_CONST
