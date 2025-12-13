/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/support/exceptions/basic.h"
#  include "utl/support/exceptions/exception_ptr.h"
#  include "utl/utility/core.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN
class __UTL_ABI_PUBLIC nested_exception {
public:
    __UTL_HIDE_FROM_ABI nested_exception() noexcept
        : exception_(current_exception()) {}
    __UTL_HIDE_FROM_ABI nested_exception(
        nested_exception const&) noexcept = default;
    __UTL_HIDE_FROM_ABI nested_exception& operator=(
        nested_exception const&) noexcept = default;

    virtual ~nested_exception() noexcept;
    UTL_ATTRIBUTE(_HIDE_FROM_ABI, NORETURN) void rethrow_nested() const {
        if (!exception_) {
            terminate();
        }

        rethrow_exception(exception_);
    }

    __UTL_HIDE_FROM_ABI exception_ptr nested_ptr() const noexcept {
        return exception_;
    }

private:
    exception_ptr exception_;
};
UTL_PUBLIC_DEFAULT_NAMESPACE_END

UTL_DEFAULT_NAMESPACE_BEGIN
namespace details {
template <typename T>
struct nested : public T, public nested_exception {
    __UTL_HIDE_FROM_ABI explicit nested(T const& val) : T(val) {}
};
} // namespace details
UTL_DEFAULT_NAMESPACE_END

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if UTL_WITH_EXCEPTIONS
template <typename T>
UTL_ATTRIBUTES(NORETURN, _HIDE_FROM_ABI)
void throw_with_nested(T&& val) {
    using decayed_type =
        decltype([](auto&& val) { return val; }(__UTL forward<T>(val)));
    static_assert(requires(decayed_type val) {
        decayed_type(val);
        val.~decayed_type();
    });

    if constexpr (UTL_BUILTIN_is_class(decayed_type) &&
        !UTL_BUILTIN_is_base_of(nested_exception, decayed_type) &&
        !UTL_BUILTIN_final(decayed_type)) {
        throw details::nested<decayed_type>(__UTL forward<T>(val));
    } else {
        throw __UTL forward<T>(val);
    }
}

#else

template <typename T>
UTL_ATTRIBUTES(NORETURN, _HIDE_FROM_ABI)
void throw_with_nested(T&&) noexcept {
    assert(false);
    UTL_BUILTIN_unreachable();
}

template <typename E>
__UTL_HIDE_FROM_ABI void rethrow_if_nested(E const& exc) {
    if constexpr (UTL_BUILTIN_is_polymorphic(E) &&
        (!UTL_BUILTIN_is_base_of(nested_exception, E) ||
            UTL_BUILTIN_is_convertible(E const*, nested_exception const*))) {
        auto const* nested =
            dynamic_cast<nested_exception const*>(UTL_BUILTIN_addressof(exc));
        if (nested) {
            nested->rethrow_nested();
        }
    } else {
        (void)exc;
    }
}

#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_END
