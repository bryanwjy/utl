// Copyright 2023-2025 Bryan Wong

#include "utl/filesystem/utl_filesystem_fwd.h"

// POSIX
#if (UTL_TARGET_APPLE | UTL_TARGET_LINUX | UTL_TARGET_BSD) && !UTL_TARGET_SONY && \
    !UTL_TARGET_NINTENDO

#  include "utl/filesystem/posix/utl_open.h"
#  include "utl/system_error/utl_system_error.h"

#  include <fcntl.h>

#  include <cerrno>
#  include <cstdlib>

__UFS_NAMESPACE_BEGIN

namespace details {
namespace open {

int get_flag(create_t) noexcept {
    return O_CREATE;
}
int get_flag(temporary_t) noexcept {
#  ifdef O_TMPFILE
    return O_TMPFILE;
#  else
    return O_CREATE;
#  endif
}
int get_flag(read_only_t) noexcept {
    return O_RDONLY;
}
int get_flag(write_only_t) noexcept {
    return O_WRONLY;
}
int get_flag(read_write_t) noexcept {
    return O_RDWR;
}
int get_flag(append_t<true>) noexcept {
    return O_APPEND;
}
int get_flag(exclusive_t<true>) noexcept {
    return O_EXCL;
}
int get_flag(close_on_exec_t<true>) noexcept {
    return O_CLOEXEC;
}
int get_flag(truncate_t<true>) noexcept {
    return O_TRUNC;
}
int get_flag(set_control_terminal_t<false>) noexcept {
#  ifdef O_NOCTTY
    return O_NOCTTY;
#  else
    return 0;
#  endif
}
int get_flag(path_only_t<true>) noexcept {
#  ifdef O_PATH
    return O_PATH;
#  else
    return 0;
#  endif
}
int get_flag(buffered_t<false>) noexcept {
#  ifdef O_DIRECT
    return O_DIRECT;
#  else
    return 0;
#  endif
}
int get_flag(follow_symlink_t<false>) noexcept {
    return O_NOFOLLOW;
}
int get_flag(blocking_t<false>) noexcept {
    return O_NONBLOCK;
}
int get_flag(auto_file_sync_t) noexcept {
#  ifdef O_SYNC
    return O_SYNC;
#  else
    return 0;
#  endif
}
int get_flag(auto_data_sync_t) noexcept {
#  ifdef O_DSYNC
    return O_DSYNC;
#  else
    return 0;
#  endif
}

int file_type_flag(file_type_constant_t<file_type::directory>) noexcept {
#  ifdef O_DIRECTORY
    return O_DIRECTORY;
#  else
    return 0;
#  endif
}

int file_type_flag(file_type_constant_t<file_type::symlink>) noexcept {
#  ifdef O_SYMLINK
    return O_SYMLINK;
#  elif defined(O_PATH) & defined(O_NOFOLLOW)
    return O_NOFOLLOW | O_PATH;
#  else
    return 0;
#  endif
}

bool supports_symlink() noexcept {
#  if UTL_TARGET_BSD
    return false;
#  else
    return true;
#  endif
}

file_descriptor_t impl(
    read_file_handle const& handle, zpath_view path, arguments_t const args) noexcept {
    auto const fd = ::openat(handle.native_handle(), path.data(), args.flags, args.mode);
    return static_cast<file_descriptor_t>(fd);
}

file_descriptor_t impl(zpath_view, arguments_t) noexcept {
    auto const fd = ::open(path.data(), args.flags, args.mode);
    return static_cast<file_descriptor_t>(fd);
}

} // namespace open
} // namespace details

__UFS_NAMESPACE_END
#endif // (UTL_TARGET_APPLE | UTL_TARGET_LINUX | UTL_TARGET_BSD) && !UTL_TARGET_SONY &&
       // !UTL_TARGET_NINTENDO
