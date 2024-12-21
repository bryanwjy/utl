// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if (UTL_TARGET_APPLE | UTL_TARGET_LINUX | UTL_TARGET_BSD | UTL_TARGET_MICROSOFT)

using path_char = decltype(u8' ');

#else

#  error Unsupported target

#endif

#define __UFS_NAMESPACE_BEGIN                     \
    UTL_EXTERN_CXX_BEGIN                          \
    namespace __UTL_NAMESPACE_VISIBILITY UTL_NS { \
    namespace filesystem {

#define __UFS_NAMESPACE_END \
    }                       \
    }

#define __UFS __UTL filesystem::
