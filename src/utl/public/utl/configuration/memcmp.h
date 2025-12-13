/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/builtin_check.h"

#if UTL_HAS_BUILTIN(__builtin_memcmp)

#  define __UTL_MEMCMP __builtin_memcmp

#else

#  include "utl/configuration/compiler.h"
#  include "utl/configuration/standard.h"

#  if UTL_CXX
extern "C" int memcmp(void const*, void const*, decltype(sizeof(0)));
#  else
#    include <stddef.h>
int memcmp(void const*, void const*, size_t);
#  endif

#  if UTL_COMPILER_MSVC
#    pragma intrinsic(memcmp)
#  endif
#  define __UTL_MEMCMP ::memcmp

#endif
