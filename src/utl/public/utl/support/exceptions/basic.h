/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/support/exceptions/runtime.h"
#endif // !UTL_MODULES

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

class __UTL_ABI_PUBLIC exception_ptr;

using ::std::bad_exception;
using ::std::exception;

UTL_INLINE_CXX17 constexpr bool with_exceptions = UTL_WITH_EXCEPTIONS;

UTL_PUBLIC_DEFAULT_NAMESPACE_END
