/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/support/new/runtime.h"
#  include "utl/support/new/types.h"
#endif // !UTL_MODULES

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN
inline constexpr nothrow_t nothrow{};
inline constexpr destroying_delete_t destroying_delete{};
UTL_PUBLIC_DEFAULT_NAMESPACE_END

#if !UTL_ABI_MICROSOFT
UTL_EXPORT_BEGIN

UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new(__UTL size_t size);
UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new(
    __UTL size_t size, __UTL nothrow_t) noexcept;
__UTL_ABI_PUBLIC void operator delete(void* ptr) noexcept;
__UTL_ABI_PUBLIC void operator delete(void* ptr, __UTL nothrow_t) noexcept;
#  if UTL_SUPPORTS_SIZED_DEALLOCATION
__UTL_ABI_PUBLIC void operator delete(void* ptr, __UTL size_t size) noexcept;
#  endif // UTL_SUPPORTS_SIZED_DEALLOCATION

UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new[](__UTL size_t size);
UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new[](
    __UTL size_t size, __UTL nothrow_t) noexcept;
__UTL_ABI_PUBLIC void operator delete[](void* ptr) noexcept;
__UTL_ABI_PUBLIC void operator delete[](void* ptr, __UTL nothrow_t) noexcept;
#  if UTL_SUPPORTS_SIZED_DEALLOCATION
__UTL_ABI_PUBLIC void operator delete[](void* ptr, __UTL size_t size) noexcept;
#  endif // UTL_SUPPORTS_SIZED_DEALLOCATION

#  if UTL_SUPPORTS_ALIGNED_ALLOCATION
UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new(
    __UTL size_t size, __UTL align_val_t);
UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new(
    __UTL size_t size, __UTL align_val_t, __UTL nothrow_t) noexcept;
__UTL_ABI_PUBLIC void operator delete(void* ptr, __UTL align_val_t) noexcept;
__UTL_ABI_PUBLIC void operator delete(
    void* ptr, __UTL align_val_t, __UTL nothrow_t) noexcept;
#    if UTL_SUPPORTS_SIZED_DEALLOCATION
__UTL_ABI_PUBLIC void operator delete(
    void* ptr, __UTL size_t size, __UTL align_val_t) noexcept;
#    endif // UTL_SUPPORTS_SIZED_DEALLOCATION

UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new[](
    __UTL size_t size, __UTL align_val_t);
UTL_ATTRIBUTES(NODISCARD, _ABI_PUBLIC, MALLOC) void* operator new[](
    __UTL size_t size, __UTL align_val_t, __UTL nothrow_t) noexcept;
__UTL_ABI_PUBLIC void operator delete[](void* ptr, __UTL align_val_t) noexcept;
__UTL_ABI_PUBLIC void operator delete[](
    void* ptr, __UTL align_val_t, __UTL nothrow_t) noexcept;
#    if UTL_SUPPORTS_SIZED_DEALLOCATION
__UTL_ABI_PUBLIC void operator delete[](
    void* ptr, __UTL size_t size, __UTL align_val_t) noexcept;
#    endif // UTL_SUPPORTS_SIZED_DEALLOCATION
#  endif   // UTL_SUPPORTS_ALIGNED_ALLOCATION

UTL_EXPORT_END

#endif // !UTL_ABI_MICROSOFT
