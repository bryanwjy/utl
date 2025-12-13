/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/support/new/runtime.h"
#endif

namespace std {
struct destroying_delete_t {
    explicit constexpr destroying_delete_t() noexcept = default;
};
#if !UTL_ABI_MICROSOFT
struct nothrow_t {
    explicit constexpr nothrow_t() noexcept = default;
};
enum class align_val_t : __UTL size_t {
};
#endif
} // namespace std

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN
using ::std::align_val_t;
using ::std::destroying_delete_t;
using ::std::nothrow_t;
UTL_PUBLIC_DEFAULT_NAMESPACE_END
