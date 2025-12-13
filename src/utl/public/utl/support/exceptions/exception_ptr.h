/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/support/exceptions/basic.h"
#  include "utl/support/exceptions/runtime.h"
#  include "utl/support/new/placement_new.h"
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

#if UTL_ABI_ITANIUM

#  if UTL_WITH_EXCEPTIONS
class __UTL_ABI_PUBLIC exception_ptr {

public:
    __UTL_HIDE_FROM_ABI exception_ptr() noexcept : ptr_() {}
    __UTL_HIDE_FROM_ABI exception_ptr(decltype(nullptr)) noexcept : ptr_() {}

    __UTL_HIDE_FROM_ABI exception_ptr(exception_ptr const& other) noexcept
        : ptr_(other.ptr_) {
        __cxa_increment_exception_refcount(ptr_);
    }

    __UTL_HIDE_FROM_ABI exception_ptr& operator=(
        exception_ptr const& other) noexcept {
        if (ptr_ != other.ptr_) {
            __cxa_increment_exception_refcount(other.ptr_);
            __cxa_decrement_exception_refcount(ptr_);
            ptr_ = other.ptr_;
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI exception_ptr(exception_ptr&& other) noexcept
        : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    __UTL_HIDE_FROM_ABI exception_ptr& operator=(
        exception_ptr&& other) noexcept {
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    }
    __UTL_HIDE_FROM_ABI ~exception_ptr() noexcept {
        __cxa_decrement_exception_refcount(ptr_);
    }

    __UTL_HIDE_FROM_ABI explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    __UTL_HIDE_FROM_ABI friend bool operator==(
        exception_ptr const& lhs, exception_ptr const& rhs) noexcept {
        return lhs.ptr_ == rhs.ptr_;
    }

    __UTL_HIDE_FROM_ABI friend bool operator!=(
        exception_ptr const& lhs, exception_ptr const& rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    __UTL_HIDE_FROM_ABI static exception_ptr from_native_exception_pointer(
        void* ptr) noexcept {
        exception_ptr ptr;
        if (ptr) {
            ptr.ptr_ = ptr;
            __cxa_increment_exception_refcount(ptr.ptr_);
        }

        return ptr;
    }

    template <typename E>
    __UTL_HIDE_FROM_ABI friend exception_ptr make_exception_ptr(E) noexcept;

    __UTL_ABI_PUBLIC friend exception_ptr current_exception() noexcept;

    __UTL_ABI_PUBLIC friend void rethrow_exception(exception_ptr);

    void* ptr_;
};

UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) exception_ptr current_exception() noexcept {
    exception_ptr ptr;
    ptr.ptr_ = __cxa_current_primary_exception();
    return ptr;
}

UTL_ATTRIBUTES( _ABI_PUBLIC, NORETURN) void rethrow_exception(exception_ptr ptr) {
    __cxa_rethrow_primary_exception(p.ptr_);
    terminate();
}

template <typename E>
__UTL_HIDE_FROM_ABI exception_ptr make_exception_ptr(E exc) noexcept {
    if constexpr (is_pointer_v<E>) {
        try {
            throw exc;
        } catch (...) {
            return current_exception();
        }
    } else {
        // Easiest way to emulate decay_t without pulling in type_traits
        using exception_type =
            decltype([](E& exc) -> auto { return exc; }(exc));
        void* ptr =
            __cxxabiv1::__cxa_allocate_exception(sizeof(exception_type));
        (void)__cxxabiv1::__cxa_init_primary_exception(ptr,
            const_cast<type_info*>(&typeid(E)),
            [](void* ptr) { static_cast<exception_type*>(ptr)->~E(); });
        try {
            ::new (ptr) exception_type(exc);
            return exception_ptr::from_native_exception_pointer(ptr);
        } catch (...) {
            __cxxabiv1::__cxa_free_exception(ptr);
            return current_exception();
        }
    }
}

#  else  // !UTL_WITH_EXCEPTIONS
class __UTL_ABI_PUBLIC exception_ptr {

public:
    __UTL_HIDE_FROM_ABI exception_ptr() noexcept : ptr_() {}
    __UTL_HIDE_FROM_ABI exception_ptr(decltype(nullptr)) noexcept : ptr_() {}

    __UTL_HIDE_FROM_ABI exception_ptr(
        exception_ptr const& other) noexcept = default;
    __UTL_HIDE_FROM_ABI exception_ptr& operator=(
        exception_ptr const& other) noexcept = default;
    __UTL_HIDE_FROM_ABI ~exception_ptr() noexcept = default;

    __UTL_HIDE_FROM_ABI explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    __UTL_HIDE_FROM_ABI friend bool operator==(
        exception_ptr const& lhs, exception_ptr const& rhs) noexcept {
        return lhs.ptr_ == rhs.ptr_;
    }

    __UTL_HIDE_FROM_ABI friend bool operator!=(
        exception_ptr const& lhs, exception_ptr const& rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    void* ptr_;
};

UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) exception_ptr current_exception() noexcept {
    return exception_ptr();
}

UTL_ATTRIBUTES( _ABI_PUBLIC, NORETURN) void rethrow_exception(exception_ptr ptr) {
    assert(false);
    UTL_BUILTIN_unreachable();
}

template <typename E>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NORETURN) exception_ptr make_exception_ptr(E exc) noexcept {
    assert(false);
    UTL_BUILTIN_unreachable();
}
#  endif // UTL_WITH_EXCEPTIONS

#else // !UTL_ABI_ITANIUM

class __UTL_ABI_PUBLIC exception_ptr {
public:
    exception_ptr() noexcept { __ExceptionPtrCreate(this); }
    exception_ptr(exception_ptr const& other) noexcept {
        __ExceptionPtrCopy(this, &other);
    }

    exception_ptr& operator=(exception_ptr const& other) noexcept {
        if (this != &other) {
            __ExceptionPtrCopy(this, &other);
        }

        return *this;
    }

    __UTL_HIDE_FROM_ABI exception_ptr(decltype(nullptr)) noexcept
        : exception_ptr() {}
    __UTL_HIDE_FROM_ABI exception_ptr& operator=(decltype(nullptr)) noexcept {
        exception_ptr default_val;
        *this = default_val;
        return *this;
    }

    ~exception_ptr() noexcept { __ExceptionPtrDestroy(this); }

    explicit operator bool() const noexcept {
        return __ExceptionPtrToBool(this);
    }

private:
    UTL_DISABLE_WARNING_PUSH()
    UTL_DISABLE_WARNING("-Wunused-private-field")
    void* ptr1_ = nullptr;
    void* ptr2_ = nullptr;
    UTL_DISABLE_WARNING_POP()
};

__UTL_ABI_PUBLIC bool operator==(
    exception_ptr const& lhs, exception_ptr const& rhs) noexcept {
    return __ExceptionPtrCompare(&lhs, &rhs);
}

__UTL_ABI_PUBLIC void swap(exception_ptr& lhs, exception_ptr& rhs) noexcept {
    __ExceptionPtrSwap(&rhs, &lhs);
}

__UTL_HIDE_FROM_ABI inline bool operator!=(
    exception_ptr const& lhs, exception_ptr const& rhs) noexcept {
    return !(lhs == rhs);
}

#  if UTL_WITH_EXCEPTIONS

UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) exception_ptr current_exception() noexcept {
    exception_ptr result;
    __ExceptionPtrCurrentException(&result);
    return result;
}

UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) void rethrow_exception(exception_ptr ptr) {
    __ExceptionPtrRethrow(&ptr);
}

template <typename E>
__UTL_HIDE_FROM_ABI exception_ptr make_exception_ptr(E exc) noexcept {
    exception_ptr result;
    auto const* src = __GetExceptionInfo(exc);
    if (src) {
        __ExceptionPtrCopyException(&result, UTL_BUILTIN_addressof(exc), src);
    }
    return result;
}

#  else  // !UTL_WITH_EXCEPTIONS
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) exception_ptr current_exception() noexcept {
    return exception_ptr();
}

UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) void rethrow_exception(exception_ptr) {
    assert(false);
    UTL_BUILTIN_unreachable();
}

template <typename E>
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NORETURN) exception_ptr make_exception_ptr(E exc) noexcept {
    assert(false);
    UTL_BUILTIN_unreachable();
}
#  endif // UTL_WITH_EXCEPTIONS

#endif // UTL_ABI_ITANIUM

UTL_PUBLIC_DEFAULT_NAMESPACE_END
