/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  if UTL_WITH_EXCEPTIONS
#    if UTL_ABI_MICROSOFT
#      include <vcruntime_exception.h>
#    else // !UTL_ABI_MICROSOFT
#      include <cxxabi.h>
#    endif // UTL_ABI_MICROSOFT
#  endif   // UTL_WITH_EXCEPTIONS
#endif

UTL_NAMESPACE_BEGIN(std)

UTL_EXPORT using terminate_handler = void (*)();

UTL_EXPORT extern UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) void terminate() noexcept;

UTL_EXPORT extern UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) terminate_handler
get_terminate() noexcept;

UTL_EXPORT extern __UTL_ABI_PUBLIC terminate_handler set_terminate(
    terminate_handler f);

UTL_EXPORT extern UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) int uncaught_exceptions() noexcept;

#if UTL_ABI_ITANIUM

UTL_EXPORT
class __UTL_ABI_PUBLIC exception {
public:
    __UTL_HIDE_FROM_ABI exception() noexcept = default;
    __UTL_HIDE_FROM_ABI exception(exception const&) noexcept = default;
    __UTL_HIDE_FROM_ABI exception& operator=(
        exception const&) noexcept = default;
    virtual ~exception();
    virtual char const* what() const noexcept;
};

UTL_EXPORT
class __UTL_ABI_PUBLIC bad_exception {
public:
    __UTL_HIDE_FROM_ABI bad_exception() noexcept = default;
    __UTL_HIDE_FROM_ABI bad_exception(bad_exception const&) noexcept = default;
    __UTL_HIDE_FROM_ABI bad_exception& operator=(
        bad_exception const&) noexcept = default;
    virtual ~bad_exception();
    virtual char const* what() const noexcept;
};

#else // !UTL_ABI_ITANIUM

#  if UTL_COMPILER_CLANG | UTL_COMPILER_MSVC
/**
 * Magic function for vcruntime
 *
 * TODO: Investigate support on Intel compilers
 */
template <typename E>
void* __GetExceptionInfo(E);
#  else
#    error "Unsupported"
#  endif

#  if !UTL_WITH_EXCEPTIONS // && (!UTL_ABI_ITANIUM)

namespace details {
struct exception_data {
    char const* what;
    bool do_free;
};

__UTL_ABI_PUBLIC EXTERN_CXX void __cdecl __ExceptionPtrCreate(void*);
__UTL_ABI_PUBLIC EXTERN_CXX void __cdecl __ExceptionPtrDestroy(void*);
__UTL_ABI_PUBLIC EXTERN_CXX void __cdecl __ExceptionPtrCopy(void*, void const*);
__UTL_ABI_PUBLIC EXTERN_CXX void __cdecl __ExceptionPtrAssign(
    void*, void const*);
__UTL_ABI_PUBLIC EXTERN_CXX bool __cdecl __ExceptionPtrCompare(
    void const*, void const*);
__UTL_ABI_PUBLIC EXTERN_CXX bool __cdecl __ExceptionPtrToBool(void const*);
__UTL_ABI_PUBLIC EXTERN_CXX void __cdecl __ExceptionPtrSwap(void*, void*);
__UTL_ABI_PUBLIC EXTERN_CXX void __cdecl __ExceptionPtrCurrentException(void*);
UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) EXTERN_CXX void __cdecl __ExceptionPtrRethrow(
    void const*);
__UTL_ABI_PUBLIC EXTERN_CXX void __cdecl __ExceptionPtrCopyException(
    void*, void const*, void const*);

} // namespace details

UTL_EXPORT
class exception {
public:
    constexpr exception() noexcept : data_() {}

    explicit exception(char const* message) noexcept
        : data_{
              .what = message,
              .do_free = true,
          } {}

    exception(exception const&) noexcept : data_() {}

    exception& operator=(exception const&) noexcept { return *this; }

    virtual ~exception() noexcept {}

    virtual char const* what() const noexcept {
        return data_.what ? data_.what : "Unknown exception";
    }

private:
    details::exception_data data_;
};

UTL_EXPORT
class bad_exception : public exception {
public:
    bad_exception() noexcept : exception("bad exception") {}
};
#  endif
#endif

UTL_NAMESPACE_END(std)
