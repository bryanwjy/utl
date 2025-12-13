/* Copyright 2023-2025 Bryan Wong */
#ifdef UTL_MODULES
module;
#  include "utl/config.h"

module utl.support.exceptions:nested_exception;
export import :basic;
export import :exception_ptr;
import utl.utility.basic;
#else
#  include "utl/support/exceptions/nested_exception.h"
#endif
