/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/support/new/runtime.h"
#endif // !UTL_MODULES

UTL_EXPORT_BEGIN

UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, MSVC_CONSTEXPR) constexpr void* operator new(
    __UTL size_t, void* ptr) noexcept {
    return ptr;
}
UTL_ATTRIBUTES(_HIDE_FROM_ABI, NODISCARD, MSVC_CONSTEXPR) constexpr void* operator new[](
    __UTL size_t, void* ptr) noexcept {
    return ptr;
}
__UTL_HIDE_FROM_ABI constexpr void operator delete(void*, void*) noexcept {}
__UTL_HIDE_FROM_ABI constexpr void operator delete[](void*, void*) noexcept {}

UTL_EXPORT_END
