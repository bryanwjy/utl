/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/compiler.h" // IWYU pragma: keep
#include "utl/configuration/pragma.h"
#include "utl/configuration/standard.h" // IWYU pragma: keep

#if UTL_SUPPORTS_GNU_ASM
#  define UTL_COMPILER_BARRIER() __asm__ __volatile__("" ::: "memory")
#elif UTL_COMPILER_MSVC

#  ifdef UTL_CXX
extern "C" void _ReadWriteBarrier();
#  else
void _ReadWriteBarrier();
#  endif
#  pragma intrinsic(_ReadWriteBarrier)
#  define UTL_COMPILER_BARRIER() _ReadWriteBarrier()
#else

#  include "utl/configuration/pragma.h"

UTL_PRAGMA_WARN("Unrecognize compiler")
#  define UTL_COMPILER_BARRIER()
#endif
