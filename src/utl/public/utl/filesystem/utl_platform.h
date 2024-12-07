// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if (UTL_TARGET_BSD | UTL_TARGET_APPLE)

#  include <errno.h>
#  include <fcntl.h>
#  include <unistd.h>

extern "C" int getdirentries(int fd, char* buf, int nbytes, long* basep);

#elif UTL_TARGET_LINUX

#  include <errno.h>
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <sys/syscall.h>
#  include <sys/types.h>
#  include <unistd.h>

#elif UTL_TARGET_MICROSOFT

// TODO

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

__UFS_NAMESPACE_BEGIN

enum class file_type;
using path_char = decltype(u8' ');

struct __UTL_ABI_PUBLIC file_clock_t {
    __UTL_HIDE_FROM_ABI explicit inline constexpr file_clock_t() noexcept = default;
    __UTL_HIDE_FROM_ABI friend __UTL tempus::time_point<file_clock_t> get_time(
        file_clock_t) noexcept;
};

UTL_INLINE_CXX17 constexpr file_clock_t file_clock{};

__UFS_NAMESPACE_END
