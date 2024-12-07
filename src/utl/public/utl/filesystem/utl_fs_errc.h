// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/system_error/utl_error_category.h"
#include "utl/system_error/utl_error_code.h"

UTL_NAMESPACE_BEGIN

enum class fs_errc : int {
    mismatch_filetype
};

template <>
struct is_error_code_enum<fs_errc> : true_type {};

__UTL_ABI_PUBLIC error_category const& filesystem_category() noexcept;

namespace details {
template <typename T>
__UTL_HIDE_FROM_ABI inline constexpr result<T> make_type_error() noexcept {
    return result<T>{__UTL unexpect, fs_errc::mismatch_filetype};
}
} // namespace details

UTL_NAMESPACE_END
