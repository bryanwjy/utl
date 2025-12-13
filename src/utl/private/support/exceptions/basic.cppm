/* Copyright 2023-2025 Bryan Wong */
export module utl.support.exceptions:basic;
import "utl/config.h";
#if UTL_ABI_MICROSOFT
import <vcruntime_exception.h>;
#else
import <cxxabi.h>;
#endif // UTL_ABI_MICROSOFT

#include "utl/support/exceptions/basic.h"
