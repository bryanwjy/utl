/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/compiler.h" // IWYU pragma: keep
#ifndef UTL_WITH_EXCEPTIONS
#  ifndef _LIBCPP_HAS_NO_EXCEPTIONS
#    if defined(__cpp_exceptions)
#      define UTL_WITH_EXCEPTIONS 1
#    elif UTL_COMPILER_MSVC && defined(_CPPUNWIND)
#      if _CPPUNWIND
#        define UTL_WITH_EXCEPTIONS 1
#      endif
#    elif defined(__EXCEPTIONS)
#      define UTL_WITH_EXCEPTIONS 1
#    elif defined(_HAS_EXCEPTIONS)
#      define UTL_WITH_EXCEPTIONS _HAS_EXCEPTIONS
#    endif
#  endif
#endif
