// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/expected/utl_expected.h"
#include "utl/filesystem/utl_platform.h"
#include "utl/filesystem/utl_result.h"
#include "utl/system_error/utl_error_code.h"

__UFS_NAMESPACE_BEGIN

template <typename T>
using result = __UTL expected<T, __UTL error_code>;

__UFS_NAMESPACE_END
