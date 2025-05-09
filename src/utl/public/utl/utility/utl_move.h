// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/type_traits/utl_is_copy_constructible.h"
#include "utl/type_traits/utl_is_nothrow_move_constructible.h"
#include "utl/type_traits/utl_logical_traits.h"
#include "utl/type_traits/utl_remove_reference.h"

#if UTL_CXX14 && UTL_USE_STD_move
#  include <utility>

UTL_NAMESPACE_BEGIN

using std::move;
using std::move_if_noexcept;

UTL_NAMESPACE_END

#else // UTL_CXX14 && UTL_USE_STD_move

#  if UTL_USE_STD_move
UTL_PRAGMA_WARN(
    "The current standard does not implement a constexpr move, `UTL_USE_STD_move` ignored")
#  endif

UTL_NAMESPACE_BEGIN

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC, _HIDE_FROM_ABI) constexpr remove_reference_t<T>&& move(
    T&& t UTL_LIFETIMEBOUND) noexcept {
    return static_cast<remove_reference_t<T>&&>(t);
}

namespace details {
namespace utility {
template <typename T>
using move_if_noexcept_result_t UTL_NODEBUG =
    conditional_t<!UTL_TRAIT_is_nothrow_move_constructible(T) && UTL_TRAIT_is_copy_constructible(T),
        T const&, T&&>;

} // namespace utility
} // namespace details

template <typename T>
UTL_ATTRIBUTES(NODISCARD, CONST, INTRINSIC, _HIDE_FROM_ABI) constexpr details::utility::move_if_noexcept_result_t<T>
move_if_noexcept(T& t UTL_LIFETIMEBOUND) noexcept {
    return move(t);
}

UTL_NAMESPACE_END

#endif // UTL_CXX14 && UTL_USE_STD_move
