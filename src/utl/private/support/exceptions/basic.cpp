/* Copyright 2023-2025 Bryan Wong */
#ifdef UTL_MODULES
module utl.support.exceptions:basic;
#else
#  include "utl/support/exceptions/basic.h"
#endif

#if UTL_LIBCXXRT | UTL_LIBCXX | UTL_LIBSTDCXX
namespace std {
__UTL_ABI_PUBLIC terminate_handler set_terminate(terminate_handler) noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) terminate_handler get_terminate() noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) void terminate() noexcept;
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) int uncaught_exceptions() noexcept;

} // namespace std
#endif

UTL_DEFAULT_NAMESPACE_BEGIN
#if UTL_UTL_LIBCXXRT
/** Already Defined **/
#elif UTL_LIBCXXRT | UTL_LIBCXX | UTL_LIBSTDCXX

terminate_handler set_terminate(terminate_handler handler) noexcept {
    return std::set_terminate(handler);
}

terminate_handler get_terminate() noexcept {
    return std::get_terminate();
}

void terminate() noexcept {
    return std::terminate();
}

int uncaught_exceptions() noexcept {
    return std::uncaught_exceptions();
}

#elif UTL_ABI_MICROSOFT
terminate_handler set_terminate(terminate_handler handler) noexcept {
    return ::set_terminate(handler);
}

terminate_handler get_terminate() noexcept {
    return ::_get_terminate();
}

void terminate() noexcept {
    return ::terminate();
}

int uncaught_exceptions() noexcept {
    return ::__uncaught_exceptions();
}
#else
#  error "Unsupported"
#endif

UTL_DEFAULT_NAMESPACE_END
