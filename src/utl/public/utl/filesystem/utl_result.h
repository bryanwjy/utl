// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/expected/utl_expected.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/system_error/utl_error_category.h"
#include "utl/system_error/utl_error_code.h"

UTL_NAMESPACE_BEGIN

enum class fs_errc : int {
    file_type_mismatch
};

template <>
struct is_error_code_enum<fs_errc> : true_type {};

__UTL_ABI_PUBLIC error_category const& filesystem_category() noexcept;

template <typename T>
using result = __UTL expected<T, __UTL error_code>;

namespace details {
template <fs_errc E, typename T>
__UTL_HIDE_FROM_ABI inline constexpr result<T> make_error() noexcept {
    return result<T>{__UTL unexpect, E};
}
} // namespace details

UTL_NAMESPACE_END
