// Copyright 2025 Bryan Wong. All rights reserved.

#include "atomic.h"
#include "stdlib.h"
#include "utl/support/new.h"
#include "utl/utility/basic.h"
#include "utlrt/config.h"

#include <stdio.h>

#if UTLRT_LIBC_SUPPORTS_THREAD_ATEXIT
#  define __UTLRT_WEAK_THREAD_ATEXIT
#else
#  include <pthread.h>
#  define __UTLRT_WEAK_THREAD_ATEXIT __attribute__((weak))
#endif

extern "C" {
using destructor_t = void (*)(void*);
__UTLRT_WEAK_THREAD_ATEXIT int __cxa_thread_atexit_impl(
    destructor_t, void*, void*) noexcept;

__UTL_ABI_PUBLIC int __cxa_thread_atexit(
    destructor_t dtor, void* obj, void* dso_symbol) noexcept;
}

#if UTLRT_LIBC_SUPPORTS_THREAD_ATEXIT

int __cxa_thread_atexit(
    destructor_t dtor, void* obj, void* dso_symbol) noexcept {
    return __cxa_thread_atexit_impl(dtor, obj, dso_symbol);
}

#else
namespace {
struct destructor_node_t {
    destructor_t destructor;
    void* object;
    destructor_node_t* next;
};

void execute_destructors(void* ptr) noexcept {
    auto* next_node = static_cast<destructor_node_t*>(ptr);
    while (next_node != nullptr) {
        auto* current_node = std::exchange(next_node, next_node->next);
        current_node->destructor(current_node->next);
        rpfree(current_node);
    }
}

pthread_key_t destructor_key;

[[gnu::constructor]] void initialize() noexcept {
    if (pthread_key_create(&destructor_key, execute_destructors) != 0) {
        fprintf(stderr, "Unable to initialize TLS destructor storage");
        abort();
    }
}

[[gnu::destructor]] void finalize() noexcept {
    if (auto* head = pthread_getspecific(destructor_key)) {
        execute_destructors(head);
        pthread_setspecific(destructor_key, nullptr);
    }
}
} // namespace

int __cxa_thread_atexit(
    destructor_t dtor, void* obj, void* dso_symbol) noexcept {
    if (__cxa_thread_atexit_impl) {
        return __cxa_thread_atexit_impl(dtor, obj, dso_symbol);
    }

    auto* head =
        static_cast<destructor_node_t*>(pthread_getspecific(destructor_key));

    auto* node_storage = rpmalloc(sizeof(destructor_node_t));
    if (!node_storage) {
        return -1;
    }

    auto* node = ::new (node_storage) destructor_node_t{
        .destructor = dtor,
        .object = obj,
        .next = head,
    };

    if (pthread_setspecific(destructor_key, node) != 0) {
        rpfree(node);
        return -1;
    }

    return 0;
}
#endif
