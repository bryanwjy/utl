// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_enable_if.h"

#define __UTL_ATTRIBUTE_ADDRESSOF (CONST)(NODISCARD)(ALWAYS_INLINE)__UTL_ATTRIBUTE__HIDE_FROM_ABI
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE_ADDRESSOF

UTL_NAMESPACE_BEGIN

template <typename T>
constexpr T const* addressof(T const&&) = delete;

UTL_NAMESPACE_END

#ifdef UTL_BUILTIN_addressof

#  include "utl/concepts/utl_object_type.h"
#  include "utl/type_traits/utl_is_object.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(object_type) T>
UTL_ATTRIBUTE(ADDRESSOF) inline constexpr UTL_ENABLE_IF_CXX11(T*, is_object<T>::value) addressof(T& arg UTL_LIFETIMEBOUND) noexcept {
    return UTL_BUILTIN_addressof(arg);
}

template <typename T>
UTL_ATTRIBUTE(ADDRESSOF) inline constexpr UTL_ENABLE_IF_CXX11(T*, !is_object<T>::value) addressof(T& arg UTL_LIFETIMEBOUND) noexcept {
    return &arg;
}

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_addressof_constexpr : true_type {};

UTL_NAMESPACE_END
#else // ifdef UTL_BUILTIN_addressof

#  include "utl/type_traits/utl_constants.h"
#  include "utl/type_traits/utl_copy_cvref.h"
#  include "utl/type_traits/utl_declval.h"
#  include "utl/type_traits/utl_logical_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace addressof {

template <typename T>
__UTL_HIDE_FROM_ABI auto has_global_overload(int) -> __UTL false_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto has_global_overload(float)
    -> __UTL always_true_type<decltype(operator&(__UTL declval<T&>()))>;

template <typename T>
__UTL_HIDE_FROM_ABI auto has_member_overload(int) -> __UTL false_type;
template <typename T>
__UTL_HIDE_FROM_ABI auto has_member_overload(float)
    -> __UTL always_true_type<decltype(__UTL declval<T&>().operator&())>;
template <typename T>
__UTL_HIDE_FROM_ABI auto not_addressible(int) noexcept
    -> __UTL always_false_type<decltype(&__UTL declval<T&>())>;
template <typename T>
__UTL_HIDE_FROM_ABI auto not_addressible(float) noexcept -> __UTL true_type;

template <typename T>
using has_overload UTL_NODEBUG = __UTL disjunction<decltype(not_addressible<T>(0)),
    decltype(has_member_overload<T>(0)), decltype(has_global_overload<T>(0))>;
} // namespace addressof
} // namespace details

template <typename T>
UTL_ATTRIBUTE(ADDRESSOF) inline enable_if_t<details::addressof::has_overload<T>::value, T*> addressof(
    T& arg UTL_LIFETIMEBOUND) noexcept {
    return reinterpret_cast<T*>(&reinterpret_cast<copy_cvref_t<T&, char>>(arg));
}

template <typename T>
UTL_ATTRIBUTE(ADDRESSOF) inline constexpr enable_if_t<!details::addressof::has_overload<T>::value, T*>
addressof(T& arg UTL_LIFETIMEBOUND) noexcept {
    return &arg;
}

template <typename T>
struct __UTL_PUBLIC_TEMPLATE is_addressof_constexpr :
    negation<details::addressof::has_overload<T>> {};

UTL_NAMESPACE_END
#endif // ifndef UTL_BUILTIN_addressof

#undef __UTL_ATTRIBUTE_ADDRESSOF
#undef __UTL_ATTRIBUTE_TYPE_AGGREGATE_ADDRESSOF
