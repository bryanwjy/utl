
#include "stdexcept.h"
#include "utl/new.h"
#include "utl/utility/basic.h"
#include "utlrt/config.h"

namespace std {
UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) void terminate() noexcept;
__UTL_ABI_PUBLIC int uncaught_exceptions() noexcept;
} // namespace std

extern "C" {
__UTL_ABI_PUBLIC void* __cxa_vec_new(size_t element_count, size_t element_size,
    size_t padding_size, void (*constructor)(void*), void (*destructor)(void*));

__UTL_ABI_PUBLIC void* __cxa_vec_new2(size_t element_count, size_t element_size,
    size_t padding_size, void (*constructor)(void*), void (*destructor)(void*),
    void* (*alloc)(size_t), void (*dealloc)(void*));

__UTL_ABI_PUBLIC void* __cxa_vec_new3(size_t element_count, size_t element_size,
    size_t padding_size, void (*constructor)(void*), void (*destructor)(void*),
    void* (*alloc)(size_t), void (*dealloc)(void*, size_t));

__UTL_ABI_PUBLIC void __cxa_vec_cctor(void* dest_array, void* src_array,
    size_t element_count, size_t element_size,
    void (*constructor)(void*, void*), void (*destructor)(void*));

__UTL_ABI_PUBLIC void __cxa_vec_ctor(void* array_address, size_t element_count,
    size_t element_size, void (*constructor)(void*), void (*destructor)(void*));

__UTL_ABI_PUBLIC void __cxa_vec_dtor(void* array_address, size_t element_count,
    size_t element_size, void (*destructor)(void*));

__UTL_ABI_PUBLIC void __cxa_vec_cleanup(void* array_address,
    size_t element_count, size_t element_size, void (*destructor)(void*));

__UTL_ABI_PUBLIC void __cxa_vec_delete(void* array_address, size_t element_size,
    size_t padding_size, void (*destructor)(void*));

__UTL_ABI_PUBLIC void __cxa_vec_delete2(void* array_address,
    size_t element_size, size_t padding_size, void (*destructor)(void*),
    void (*dealloc)(void*));

__UTL_ABI_PUBLIC void __cxa_vec_delete3(void* __array_address,
    size_t element_size, size_t padding_size, void (*destructor)(void*),
    void (*dealloc)(void*, size_t));
}

namespace {
UTL_ATTRIBUTES(NORETURN)
void throw_bad_array_new_length() {
#if UTL_WITH_EXCEPTIONS
    throw std::bad_array_new_length();
#else
    fprintf(stderr, "__cxa_vec_new failed to allocate memory");
    abort();
#endif
}

bool mul_overflow(size_t x, size_t y, size_t* res) {
#if UTL_HAS_BUILTIN(__builtin_mul_overflow)
    return __builtin_mul_overflow(x, y, res);
#else
    *res = x * y;
    return x && ((*res / x) != y);
#endif
}

bool add_overflow(size_t x, size_t y, size_t* res) {
#if UTL_HAS_BUILTIN(__builtin_add_overflow)
    return __builtin_add_overflow(x, y, res);
#else
    *res = x + y;
    return *res < y;
#endif
}

size_t calculate_allocation_size_or_throw(
    size_t element_count, size_t element_size, size_t padding_size) {
    size_t element_heap_size;
    if (mul_overflow(element_count, element_size, &element_heap_size))
        throw_bad_array_new_length();

    size_t allocation_size;
    if (add_overflow(element_heap_size, padding_size, &allocation_size))
        throw_bad_array_new_length();

    return allocation_size;
}

size_t get_element_count(void* p) {
    return static_cast<size_t*>(p)[-1];
}

void set_element_count(void* p, size_t element_count) {
    auto* cookie_ptr = static_cast<size_t*>(p) - 1;
    ::new (p) size_t(element_count);
}

template <typename F>
class scope_object {
    template <typename T>
    explicit scope_object(T&& func) noexcept : func(__UTL forward<T>(func)) {}

    void release() noexcept { enabled_ = false; }

    ~scope_object() noexcept {
        if (enabled_) {
            func();
        }
    }

    F func;
    bool enabled_ = true;
};

template <typename T>
T&& decl() noexcept;
template <typename T>
using decay_t = decltype([](auto&& val) { return val; }(decl<T>()));

template <typename F>
explicit scope_object(F&& func) -> scope_object<decay_t<F>>;

} // namespace

void* __cxa_vec_new(size_t element_count, size_t element_size,
    size_t padding_size, void (*constructor)(void*),
    void (*destructor)(void*)) {
    return __cxa_vec_new2(element_count, element_size, padding_size,
        constructor, destructor, &::operator new[], &::operator delete[]);
}

void* __cxa_vec_new2(size_t element_count, size_t element_size,
    size_t padding_size, void (*constructor)(void*), void (*destructor)(void*),
    void* (*allocate)(size_t), void (*deallocate)(void*)) {
    size_t const heap_size = calculate_allocation_size_or_throw(
        element_count, element_size, padding_size);
    char* const heap_block = static_cast<char*>(allocate(heap_size));
    char* vec_base = heap_block;

    if (nullptr == vec_base) {
        return vec_base;
    }

    scope_object scope([=]() { deallocate(heap_block); });

    //  put the padding before the array elements
    if (0 != padding_size) {
        vec_base += padding_size;
        set_element_count(vec_base, element_count);
    }

    //  Construct the elements
    __cxa_vec_ctor(
        vec_base, element_count, element_size, constructor, destructor);
    scope.release(); // We're good!
    return vec_base;
}

void* __cxa_vec_new3(size_t element_count, size_t element_size,
    size_t padding_size, void (*constructor)(void*), void (*destructor)(void*),
    void* (*allocate)(size_t), void (*deallocate)(void*, size_t)) {
    size_t const heap_size = calculate_allocation_size_or_throw(
        element_count, element_size, padding_size);
    char* const heap_block = static_cast<char*>(allocate(heap_size));
    char* vec_base = heap_block;

    if (nullptr == vec_base) {
        return vec_base;
    }

    scope_object scope([=]() { deallocate(heap_block, heap_size); });

    //  put the padding before the array elements
    if (0 != padding_size) {
        vec_base += padding_size;
        set_element_count(vec_base, element_count);
    }

    //  Construct the elements
    __cxa_vec_ctor(
        vec_base, element_count, element_size, constructor, destructor);
    scope.release();

    return vec_base;
}

void __cxa_vec_cctor(void* dest_array, void* src_array, size_t element_count,
    size_t element_size, void (*constructor)(void*, void*),
    void (*destructor)(void*)) {
    if (nullptr == constructor) {
        return;
    }

    size_t idx = 0;
    char* src_ptr = static_cast<char*>(src_array);
    char* dest_ptr = static_cast<char*>(dest_array);
    scope_object cleanup([&idx, dest_array, element_size, destructor]() {
        __cxa_vec_cleanup(dest_array, idx, element_size, destructor);
    });

    for (; idx < element_count;
        ++idx, src_ptr += element_size, dest_ptr += element_size) {
        constructor(dest_ptr, src_ptr);
    }
    cleanup.release();
}

void __cxa_vec_ctor(void* array_address, size_t element_count,
    size_t element_size, void (*constructor)(void*),
    void (*destructor)(void*)) {
    if (nullptr == constructor) {
        return;
    }

    size_t idx = 0;
    char* ptr = static_cast<char*>(array_address);
    scope_object cleanup([&idx, array_address, element_size, destructor]() {
        __cxa_vec_cleanup(array_address, idx, element_size, destructor);
    });
    //  Construct the elements
    for (; idx < element_count; ++idx, ptr += element_size) {
        constructor(ptr);
    }
    cleanup.release();
}

void __cxa_vec_dtor(void* array_address, size_t element_count,
    size_t element_size, void (*destructor)(void*)) {
    if (nullptr == destructor) {
        return;
    }

    char* ptr = static_cast<char*>(array_address);
    size_t idx = element_count;

    // If exception thrown, cleanup is executed during stack unwinding to
    // continue destroying the rest of the elements. When cleanup completes
    // without another exception thrown, stack unwinding continues with the
    // initial exception propagating outwards. If another exception is raised
    // within cleanup, std::terminate is called.
    scope_object cleanup([&idx, array_address, element_size, destructor]() {
        __cxa_vec_cleanup(array_address, idx, element_size, destructor);
    });
    ptr += element_count * element_size; // one past the last element
    while (idx-- > 0) {
        ptr -= element_size;
        destructor(ptr);
    }
    cleanup.release();
}

void __cxa_vec_cleanup(void* array_address, size_t element_count,
    size_t element_size, void (*destructor)(void*)) {
    if (nullptr == destructor) {
        return;
    }

    char* ptr = static_cast<char*>(array_address);
    size_t idx = element_count;
    scope_object exception_guard([&, count = std::uncaught_exceptions()]() {
        auto const diff = std::uncaught_exceptions() - count;
        if (diff > 0) {
            std::terminate();
        }
    });

    ptr += element_count * element_size; // one past the last element
    while (idx-- > 0) {
        ptr -= element_size;
        destructor(ptr);
    }
}

void __cxa_vec_delete(void* array_address, size_t element_size,
    size_t padding_size, void (*destructor)(void*)) {
    __cxa_vec_delete2(array_address, element_size, padding_size, destructor,
        &::operator delete[]);
}

void __cxa_vec_delete2(void* array_address, size_t element_size,
    size_t padding_size, void (*destructor)(void*), void (*deallocate)(void*)) {
    if (nullptr == array_address) {
        return;
    }

    char* vec_base = static_cast<char*>(array_address);
    char* heap_block = vec_base - padding_size;
    scope_object const heap([=]() { deallocate(heap_block); });

    if (0 != padding_size && nullptr != destructor) {
        __cxa_vec_dtor(array_address, get_element_count(vec_base), element_size,
            destructor);
    }
}

void __cxa_vec_delete3(void* array_address, size_t element_size,
    size_t padding_size, void (*destructor)(void*),
    void (*deallocate)(void*, size_t)) {
    if (nullptr == array_address) {
        return;
    }

    char* vec_base = static_cast<char*>(array_address);
    char* heap_block = vec_base - padding_size;
    size_t const element_count =
        padding_size ? __get_element_count(vec_base) : 0;
    size_t const heap_block_size = element_size * element_count + padding_size;
    scope_object const heap([=]() { deallocate(heap_block, heap_block_size); });

    if (0 != padding_size && nullptr != destructor) {
        __cxa_vec_dtor(array_address, element_count, element_size, destructor);
    }
}
