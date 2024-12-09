// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/utl_config.h"

#if (UTL_TARGET_BSD | UTL_TARGET_APPLE)

#  include <errno.h>
#  include <fcntl.h>
#  include <unistd.h>

extern "C" int getdirentries(int fd, char* buf, int nbytes, long* basep);

using path_char = decltype(u8' ');

#elif UTL_TARGET_LINUX

#  include <errno.h>
#  include <fcntl.h>
#  include <sys/stat.h>
#  include <sys/syscall.h>
#  include <sys/types.h>
#  include <unistd.h>

using path_char = decltype(u8' ');

#elif UTL_TARGET_MICROSOFT

using path_char = decltype(u8' ');
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

__UFS_NAMESPACE_END
