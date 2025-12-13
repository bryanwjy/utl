/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  if UTL_ABI_MICROSOFT
#    include "utl/support/exceptions.h" // For bad_alloc and bad_new_array_length
#    define __PLACEMENT_NEW_INLINE
#    define __PLACEMENT_VEC_NEW_INLINE
#    include <vcruntime_new.h>
#  endif
#endif

#if UTL_ABI_ITANIUM
UTL_NAMESPACE_BEGIN(std)
using new_handler = void (*)();
__UTL_ABI_PUBLIC new_handler set_new_handler(new_handler handler);

__UTL_ABI_PUBLIC new_handler get_new_handler();
UTL_NAMESPACE_END(std)
#endif
