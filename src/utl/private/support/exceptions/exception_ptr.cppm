/* Copyright 2023-2025 Bryan Wong */
export module utl.support.exceptions:exception_ptr;
export import :basic;
import utl.support.type_info;
import "utl/config.h";

#if UTL_ABI_ITANIUM
import <vcruntime_exception.h>;
#else
import <cxxabi.h>;
#endif // UTL_ABI_MICROSOFT

#include "utl/support/exceptions/exception_ptr.h"
