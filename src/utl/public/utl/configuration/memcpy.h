/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/builtin_check.h"
#include "utl/configuration/compiler.h"
#include "utl/configuration/keywords.h"
#include "utl/configuration/standard.h"

#if UTL_HAS_BUILTIN(__builtin_memcpy)

#  define __UTL_MEMCPY __builtin_memcpy

#else

#  if UTL_CXX
extern "C" void* memcpy(
    void* UTL_RESTRICT, void const* UTL_RESTRICT, decltype(sizeof(0)));
#  else
#    include <stddef.h>
void* memcpy(void* UTL_RESTRICT, void const* UTL_RESTRICT, size_t);
#  endif

#  if UTL_COMPILER_MSVC
#    pragma intrinsic(memcpy)
#  endif
#  define __UTL_MEMCPY ::memcpy

#endif
