// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_platform.h"

__UFS_NAMESPACE_BEGIN

class __UTL_ABI_PUBLIC file_error {
#if UTL_TARGET_MICROSOFT
    using value_type = unsigned long;
#else
    using value_type = int;
#endif
public:
    __UTL_HIDE_FROM_ABI inline constexpr explicit file_error(int value) noexcept : value_{value} {}

    __UTL_HIDE_FROM_ABI inline std::string_view message() const noexcept;

    __UTL_HIDE_FROM_ABI inline constexpr int value() const noexcept { return value_; }

private:
    int value_;
};

__UFS_NAMESPACE_END
