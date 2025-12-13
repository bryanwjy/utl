/**
 * Copyright 2010-2011 PathScale, Inc. All rights reserved.
 * Copyright 2025 Bryan Wong
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * memory.cc - Contains stub definition of C++ new/delete operators.
 *
 * These definitions are intended to be used for testing and are weak symbols
 * to allow them to be replaced by definitions from a STL implementation.
 * These versions simply wrap malloc() and free(), they do not provide a
 * C++-specific allocator.
 */

#include "atomic.h"
#include "rpmalloc.h"
#include "stdexcept.h"

#include <stddef.h>
#include <stdlib.h>
#include <utl/new.h>

/**
 * The function to call when allocation fails.  By default, there is no
 * handler and a bad allocation exception is thrown if an allocation fails.
 */
static atomic<new_handler> new_handler_v{nullptr};

UTL_NAMESPACE_BEGIN(std)
/**
 * Sets a function to be called when there is a failure in new.
 */
__attribute__((weak)) new_handler set_new_handler(new_handler handler) {
    return new_handler_v.exchange(handler);
}

__attribute__((weak)) new_handler get_new_handler(void) {
    return new_handler_v.load();
}
UTL_NAMESPACE_END(std)

static atomic<int> initialized{0};

[[gnu::constructor, gnu::cold]] static void initialize() noexcept {
    if (initialized.load() != -1) {
        if (rpmalloc_initialize(nullptr) != 0) {
            fprintf(stderr, "Unabled to initialize memory subsystem");
            abort();
        }
        initialized.store(-1);
    }
}

[[gnu::destructor, gnu::cold]] static void finalize() noexcept {
    if (initialized.load() == -1) {
        rpmalloc_finalize();
    }
}

static bool has_single_bit(size_t val) noexcept {
    return val && !(val & (val - 1));
}

template <typename F>
static void* allocate(size_t size, F&& on_fail) {
    if (0 == size) {
        size = 1;
    }
    void* mem = rpaligned_alloc(__STDCPP_DEFAULT_NEW_ALIGNMENT__, size);
    if (mem == nullptr) {
        on_fail();
    }

    return mem;
}

template <typename F>
static void* allocate(std::align_val_t alignment, size_t size, F&& on_fail) {
    if (0 == size) {
        size = 1;
    }

    auto const align = static_cast<size_t>(alignment);
    if (!has_single_bit(align)) {
        on_fail();
        return nullptr;
    }

    void* mem = rpaligned_alloc(align, size);
    if (mem == nullptr) {
        on_fail();
    }

    return mem;
}

__attribute__((weak, visibility("default"))) void* operator new(
    size_t size, std::nothrow_t const&) noexcept {
    return allocate(size, []() {
        auto handler = std::get_new_handler();
        if (handler != nullptr) {
            handler();
        }
    });
}

__attribute__((weak, visibility("default"))) void* operator new(size_t size) {
    return allocate(size, []() {
        auto handler = std::get_new_handler();
        if (handler != nullptr) {
            handler();
        } else {
            throw std::bad_alloc();
        }
    });
}

__attribute__((weak, visibility("default"))) void operator delete(
    void* ptr) noexcept {
    rpfree(ptr);
}

__attribute__((weak, visibility("default"))) void* operator new[](size_t size) {
    return ::operator new(size);
}

__attribute__((weak, visibility("default"))) void* operator new[](
    size_t size, std::nothrow_t const& tag) noexcept {
    return ::operator new(size, tag);
}

__attribute__((weak, visibility("default"))) void operator delete[](
    void* ptr) noexcept {
    ::operator delete(ptr);
}

// C++14 additional delete operators

#if UTL_SUPPORTS_SIZED_DEALLOCATION
__attribute__((weak, visibility("default"))) void operator delete(
    void* ptr, size_t) noexcept {
    ::operator delete(ptr);
}

__attribute__((weak, visibility("default"))) void operator delete[](
    void* ptr, size_t) noexcept {
    ::operator delete(ptr);
}
#endif

#if UTL_SUPPORTS_ALIGNED_ALLOCATION
__attribute__((weak, visibility("default"))) void* operator new(
    size_t size, align_val_t align) {
    return allocate(align, size, []() {
        auto handler = std::get_new_handler();
        if (handler != nullptr) {
            handler();
        } else {
            throw std::bad_alloc();
        }
    });
}

__attribute__((weak, visibility("default"))) void* operator new(
    size_t size, align_val_t align, std::nothrow_t const&) noexcept {
    return allocate(align, size, []() {
        auto handler = std::get_new_handler();
        if (handler != nullptr) {
            handler();
        }
    });
}
__attribute__((weak, visibility("default"))) void operator delete(
    void* ptr, align_val_t) noexcept {
    ::operator delete(ptr);
}

__attribute__((weak, visibility("default"))) void operator delete(
    void* ptr, align_val_t, std::nothrow_t const&) noexcept {
    ::operator delete(ptr);
}

#  if UTL_SUPPORTS_SIZED_DEALLOCATION
__attribute__((weak, visibility("default"))) void operator delete(
    void* ptr, size_t, align_val_t) noexcept {
    ::operator delete(ptr);
}
#  endif // UTL_SUPPORTS_SIZED_DEALLOCATION

__attribute__((weak, visibility("default"))) void* operator new[](
    size_t size, align_val_t align) {
    return ::operator new(size, algin);
}

__attribute__((weak, visibility("default"))) void* operator new[](
    size_t size, align_val_t align, std::nothrow_t const& tag) noexcept {
    return ::operator new(size, algin, tag);
}

__attribute__((weak, visibility("default"))) void operator delete[](
    void* ptr, align_val_t, std::nothrow_t const&) noexcept {
    ::operator delete(ptr);
}

#  if UTL_SUPPORTS_SIZED_DEALLOCATION
__attribute__((weak, visibility("default"))) void operator delete[](
    void* ptr, size_t, align_val_t) noexcept {
    ::operator delete(ptr);
}
#  endif // UTL_SUPPORTS_SIZED_DEALLOCATION
#endif   // UTL_SUPPORTS_ALIGNED_ALLOCATION
