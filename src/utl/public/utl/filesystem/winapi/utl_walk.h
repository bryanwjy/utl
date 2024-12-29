#pragma once

#include "utl/utl_config.h"

#if !UTL_TARGET_MICROSOFT
#  error "Invalid header access"
#endif

#if !defined(UTL_FILESYSTEM_WALK_PRIVATE_HEADER_GUARD)
#  error "Private header accessed"
#endif

struct _WIN32_FIND_DATAA;
using WIN32_FIND_DATAA = _WIN32_FIND_DATAA;
using PWIN32_FIND_DATAA = WIN32_FIND_DATAA*;
using LPWIN32_FIND_DATAA = WIN32_FIND_DATAA*;
using PVOID = void*;
using HANDLE = PVOID;
using LPCSTR = char const*;

extern "C" HANDLE FindFirstFileA(LPCSTR, LPWIN32_FIND_DATAA);

__UFS_NAMESPACE_BEGIN

namespace detail {

__UTL_ABI_PUBLIC file_view_snapshot translate_directory_entry(WIN32_FIND_DATAA const*) noexcept;

class walk_argument {
public:
    __UTL_HIDE_FROM_ABI walk_argument(WIN32_FIND_DATAA const* entry) noexcept
        : entry_{details::translate_directory_entry(entry)} {}

    template <UTL_CONCEPT_CXX20(
        walk_invocable) F UTL_CONSTAINT_CXX11(UTL_TRAIT_is_walk_invocable(F))>
    bool apply(F&& func) const {
        // TODO
    }

private:
    file_view_snapshot entry_;
};

} // namespace detail
__UFS_NAMESPACE_END
