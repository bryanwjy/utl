
module utl.support.typeinfo;

import "utl/config.h";
#if UTL_ABI_MICROSOFT
import <vcruntime_typeinfo.h>;
#else
import <cxxabi.h>;
#endif // UTL_ABI_MICROSOFT

#include "utl/support/typeinfo.h"
