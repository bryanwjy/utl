/* Copyright 2023-2025 Bryan Wong */
#ifdef UTL_MODULES
module utl.support.exceptions:exception_ptr;
#else
#  include "utl/support/exceptions/exception_ptr.h"
#endif

namespace std {
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) exception_ptr current_exception() noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) void rethrow_exception(exception_ptr);
} // namespace std

UTL_DEFAULT_NAMESPACE_BEGIN

#if UTL_LIBUTLCXXRT
exception_ptr current_exception() noexcept {
    exception_ptr ptr;
    ptr.ptr_ = __cxa_current_primary_exception();
    return ptr;
}
#elif UTL_LIBCXXRT | UTL_LIBCXX | UTL_LIBSTDCXX
exception_ptr current_exception() noexcept {
    exception_ptr ptr;
    ptr.ptr_ = __cxa_current_primary_exception();
    return ptr;
}

void rethrow_exception(exception_ptr ptr) {
    return std::rethrow_exception(ptr);
}
#elif UTL_ABI_MICROSOFT

#else
#  error "Unsupported"
#endif

UTL_DEFAULT_NAMESPACE_END
