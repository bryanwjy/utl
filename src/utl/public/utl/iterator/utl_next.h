// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#include "utl/iterator/utl_advance.h"
#include "utl/iterator/utl_iterator_traits.h"
#include "utl/iterator/utl_legacy_input_iterator.h"
#include "utl/type_traits/utl_enable_if.h"

UTL_NAMESPACE_BEGIN

template <UTL_CONCEPT_CXX20(__UTL input_iterator) It>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD) constexpr auto next(It it,
    typename __UTL iterator_traits<It>::difference_type n = 1) noexcept(noexcept(__UTL advance(it)))
    -> UTL_ENABLE_IF_CXX11(It, UTL_TRAIT_is_legacy_input_iterator(It)) {
    return __UTL advance(it, n), it;
}

UTL_NAMESPACE_END
