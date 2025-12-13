/**
 * Copyright 2025 Bryan Wong. All rights reserved.
 */

#if UTL_ARCH_WIDTH != 64
#  error Unsupported architecture
#endif

#include "utlrt/atomic.h"
#include "utlrt/bit/endian.h"
#include "utlrt/config.h"
#include "utlrt/utility/basic.h"

#include <rpmalloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
using destructor_t = void (*)(void*);
__UTL_ABI_PUBLIC int __cxa_atexit(
    destructor_t destructor, void* object, void* dso_symbol) noexcept;

__UTL_ABI_PUBLIC int __cxa_finalize(void* dso_symbol) noexcept;
}

namespace {
struct destructor_node_t {
    destructor_t destructor;
    void* object;
    void* dso_symbol;
    destructor_node_t* next;
};

struct hazard_storage_t {
    static constexpr size_t capacity = 64;
    destructor_node_t* hazards[capacity]{};

    auto begin() noexcept { return hazards; }

    auto end() noexcept { return hazards + capacity; }
} hazard_storage;

inline constexpr auto spin_count = 64;

struct thread_hazards_t {
    static constexpr size_t extent = 2;
    destructor_node_t** base = nullptr;

    thread_hazards_t(thread_hazards_t const&) = delete;
    thread_hazards_t& operator=(thread_hazards_t const&) = delete;
    thread_hazards_t(thread_hazards_t&&) = delete;
    thread_hazards_t& operator=(thread_hazards_t&&) = delete;

    thread_hazards_t(destructor_node_t* head) noexcept {
        initialize_base(head);
    }

    template <size_t I>
    void protect(destructor_node_t* node) noexcept {
        static_assert(I < extent);
        assert(base != nullptr);
        __UTL atomic_relaxed::store(base[I], node);
        return 0;
    }

    explicit operator bool() const noexcept { return base != nullptr; }

    void clear() noexcept {
        if (base) {
            __UTL atomic_relaxed::store(base[1], nullptr);
            __UTL atomic_release::store(base[0], nullptr);
            base = nullptr;
        }
    }

    ~thread_hazards_t() noexcept { clear(); }

private:
    void wait_for_second_slot() noexcept {
        for (auto i = 0u; i < spin_count; ++i) {
            if (__UTL atomic_relaxed::load(base[1]) == nullptr) {
                base[1] = base[0];
                return;
            }
            yield();
        }

    wait_again:
        auto const wait_val = []() {
            auto* second = __UTL atomic_relaxed::load(base[1]);
            return static_cast<uint32_t>(
                reinterpret_cast<uintptr_t>(second) & 0xffffffffu);
        }();

        auto const result = [&]() {
            if constexpr (__UTL endian::native == __UTL endian::little) {
                return __UTL futex(
                    reinterpret_cast<uint32_t*>(base + 1), wait_val, 50us);
            } else {
                static_assert(__UTL endian::native == __UTL endian::big,
                    "Unsupported architecture");

                return __UTL futex(
                    reinterpret_cast<uint32_t*>(
                        reinterpret_cast<unsigned char*>(base + 1) + 4),
                    wait_val, 50us);
            }
        }();

        if (result) {
            __UTL atomic_release::store(*base, nullptr);
            base = nullptr;
            return;
        }

        if (__UTL atomic_relaxed::load(base[1]) == nullptr) {
            base[1] = base[0];
            return;
        }
        goto wait_again;
    }

    void initialize_base(destructor_node_t* const head) noexcept {
        using chunk_t = hazard_storage_t::chunk_t;
        auto* storage = &hazard_storage.main_chunk;

    retry:
        constexpr auto stride = 8u;
        for (auto offset = 0u; offset < stride; offset += extent) {
            for (auto *ptr = hazard_storage.begin() + offset,
                      *const end = hazard_storage.end();
                ptr < end; ptr += stride) {
                destructor_node_t* null = nullptr;
                if (__UTL atomic_release::compare_exchange_strong(
                        *ptr, null, head, atomics::relaxed_failure)) {
                    base = ptr;
                    wait_for_second_slot();
                    return;
                }
            }
        }

        __UTL this_thread::sleep(50us);
        goto retry;
    }
};

bool is_hazard(destructor_node_t* node) noexcept {
    for (auto* ptr : hazard_storage) {
        if (ptr == node) {
            return true;
        }
    }

    return false;
};

inline constexpr size_t retirement_threshold = 7;
thread_local class retired_nodes_t {
    struct heap_t {
        destructor_node_t** begin;
        destructor_node_t** end;
    };
    union union_t {
        destructor_node_t* local[retirement_threshold];
        heap_t heap;
    };

    union_t union_{};
    size_t size_ : sizeof(size_t) * CHAR_BIT - 1;
    size_t is_heap_ : 1 = 0;

    int grow_by(size_t count) noexcept {
        auto const new_capacity = size_ + count;
        if (new_capacity <= capacity()) {
            return 0;
        }

        assert(is_heap_);
        auto const alloc_size = new_capacity + (new_capacity >> 1);
        auto const alloc_size_bytes = alloc_size * sizeof(destructor_node_t*);
        auto* storage = rprealloc(heap.begin, alloc_size_bytes);
        if (!storage) {
            return -1;
        }

        heap.begin = reinterpret_cast<destructor_node_t**>(
            ::memmove(storage, storage, alloc_size_bytes));
        heap.end = heap.begin + alloc_size;
    }

public:
    retire_nodes_t() noexcept = default;
    size_t capacity() const noexcept {
        return is_heap_
            ? static_cast<size_t>(union_.heap.end - union_.heap.begin)
            : retirement_threshold;
    }

    int emplace_back(destructor_node_t* node) noexcept {
        if (grow_by(1) != 0) {
            return -1;
        }

        if (is_heap_) {
            heap.begin[size++] = node;
        } else {
            local[size++] = node;
        }

        return 0;
    }

    auto begin() noexcept {
        return is_heap_ ? union_.heap.begin : union_.local;
    }
    auto end() noexcept { return begin() + size_; }

    auto begin() const noexcept {
        return is_heap_ ? union_.heap.begin : union_.local;
    }

    auto end() const noexcept { return begin() + size_; }

    auto size() const noexcept { return size_; }

    auto empty() const noexcept { return size_ == 0; }

    void erase_if_not_hazards() noexcept {
        constexpr auto find_if_not_hazard = [this]() {
            for (auto** it = begin(); it != end(); ++it) {
                if (!is_hazard(*it)) {
                    return it;
                }
            }

            return end();
        };

        auto** first = find_if_not_hazard();
        if (first != end()) {
            for (auto** it = first; ++it != end();) {
                if (is_hazard(*it)) {
                    __UTL swap(*first++, *it);
                }
            }
        }

        auto const erased = (end() - first);
        for (first != end(); ++first) {
            rpfree(*first);
        }
        size_ -= erased;
    }

    ~retired_nodes_t() noexcept {
        /**
         * Since this is not trivially destructible, a thread_atexit handler
         * will be registered.
         *
         * It is the programmer's responsibility to ensure threads depending on
         * a DSO are cleaned up before the aforementioned DSO is unloaded or
         * risk undefined behaviour (Typically a segfault).
         */
        for (auto i = 0; i < spin_count; ++i) {
            erase_non_hazards();
            if (empty()) {
                return;
            }
            yield();
        }

        while (!empty()) {
            erase_non_hazards();
            __UTL this_thread::sleep(50us);
        }
    }
} retired_nodes;

static constinit destructor_node_t error_node{};

destructor_node_t atexit_handlers{};

int retire_node(destructor_node_t* node) noexcept {
    if (node == nullptr) {
        return;
    }

    if (retired_nodes.size() >= retirement_threshold - 1) {
        erase_if_not_hazards();
    }

    return retire_nodes.emplace_back(node);
}

void wait_until_no_hazards() noexcept {
    static constexpr auto any_alive = []() {
        for (auto ptr : hazard_storage) {
            if (ptr != nullptr) {
                return true;
            }
        }

        return false;
    };

    for (auto i = 0; i < spin_count; ++i) {
        if (!any_alive()) {
            return;
        }
        yield(); // TODO
    }

    while (any_alive()) {
        __UTL this_thread::sleep(50us);
    }
}
} // namespace

int __cxa_atexit(
    destructor_t destructor, void* object, void* dso_symbol) noexcept {
    auto* node_storage = rpmalloc(sizeof(destructor_node_t));
    if (!node_storage) {
        fprintf(stderr,
            "utlrt: allocation failure encountered during destructor "
            "registration\n");
        return -1;
    }
    auto* node = ::new (node_storage) destructor_node_t{
        .destructor = destructor,
        .object = object,
        .dso_symbol = dso_symbol,
        .next = __UTL atomic_relaxed::load(atexit_handlers.next),
    };

    while (!__UTL atomic_release::compare_exchange_weak(atexit_handlers.next,
        node->next, node, __UTL atomics::relaxed_failure)) {}
    return 0;
}

int __cxa_finalize(void* dso_symbol) noexcept {
    static thread_local bool entered = false;
    if (entered) {
        fprintf(stderr, "utlrt: reentrant static destructor encountered\n");
        abort();
    }

    entered = true;
    destructor_node_t* prev = &atexit_handlers;
    destructor_node_t* current = atexit_handlers.next;
    thread_hazards_t hazards(current);
    if (!hazards) {
        goto error_path;
    }

    while (current) {
        destructor_node_t* next = nullptr;
        do {
            next = __UTL atomic_acquire::load(current->next);
            thread_hazards.protect<1>(next);
        } while (next != __UTL atomic_acquire::load(current->next));

        if (dso_symbol != nullptr && current->dso_symbol != dso_symbol) {
            // this should never be error
            prev = current;
            current = next;
            // no need to loop, since next was already protected
            thread_hazards.protect<0>(current);
        } else {
            auto* expected = current;
            if (!__UTL atomic_release::compare_exchange_strong(prev->next,
                    expected, next, __UTL atomics::acquire_failure)) {
                // Race lost; restart
                prev = &atexit_handlers;
                do {
                    current = __UTL atomic_acquire::load(prev->next);
                    thread_hazards.protect<0>(current);
                } while (current != __UTL atomic_acquire::load(prev->next));
                continue;
            }

            current->destructor(current->object);
            if (retire_node(current) != 0) {
                fprintf(stderr,
                    "utlrt: allocation failure during static "
                    "destructor encountered\n");
                goto error_path;
            }

            current = next;
            thread_hazards.protect<0>(current);
        }
    }

    if (dso_symbol == nullptr) {
        wait_until_no_hazards();
    }

    entered = false;
    return 0;
error_path:
    entered = false;
    return -1;
}
