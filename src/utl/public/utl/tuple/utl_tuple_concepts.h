// Copyright 2023-2024 Bryan Wong

#pragma once

#if UTL_CXX20

#  include "utl/concepts/utl_convertible_to.h"
#  include "utl/concepts/utl_reference.h"
#  include "utl/tuple/utl_tuple_get_element.h"
#  include "utl/tuple/utl_tuple_traits.h"

UTL_NAMESPACE_BEGIN

namespace details {
namespace tuple {
template <typename T>
concept has_size = requires { tuple_size<T>::value; };
template <typename T, size_t I>
concept has_element = has_size<T> && requires(T t) {
    requires (I < tuple_size<T>::value);
    typename tuple_element<I, T>::type;
    __UTL get_element<I>(__UTL forward<T>(t));
};
} // namespace tuple
} // namespace details

template <typename T>
concept tuple_like =
    (!reference<T> && details::tuple::has_size<T> && []<size_t... Is>(index_sequence<Is...>) {
        return (... && details::tuple::has_element<T, Is>);
    }(make_index_sequence<tuple_size_v<T>>{}));

UTL_NAMESPACE_END

#endif // if UTL_CXX20
