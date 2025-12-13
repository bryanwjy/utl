// Copyright 2025 Bryan Wong

#pragma once

/**
 * Begin User Configurable
 */
#ifndef UTL_NS
#  define UTL_NS utl
#endif

#ifdef UTL_MODULES
#  if !UTL_MODULES
#    undef UTL_MODULES
#  endif
#endif

#ifndef UTL_ENABLE_STD_INTEROP
#  define UTL_ENABLE_STD_INTEROP 0
#endif

#ifndef UTL_DISABLE_INT128_SUPPORT
#  define UTL_DISABLE_INT128_SUPPORT 0
#endif

#ifndef UTL_SET_ABI_MICROSOFT
#  define UTL_SET_ABI_MICROSOFT 0
#endif

#ifndef UTL_SET_ABI_ITANIUM
#  define UTL_SET_ABI_ITANIUM 0
#endif

/**
 * End User Configurable
 */

#if UTL_SET_ABI_MICROSOFT & UTL_SET_ABI_ITANIUM
#  error Only 1 ABI may be selected
#endif

/**
 * Begin Build Configuration
 */
#ifndef UTL_CUSTOM_CXXABI
#  define UTL_CUSTOM_CXXABI 0
#endif

#ifndef UTL_LIBUTLCXXRT
#  define UTL_LIBUTLCXXRT 0
#endif

#if UTL_LIBUTLCXXRT & UTL_CUSTOM_CXXABI
#  error Only 1 ABI build configuration may be selected
#endif

#ifdef UTL_LIBUTLCXXRT
#  if UTL_TARGET_MICROSOFT | UTL_SET_ABI_MICROSOFT
#    error \
        "Unsupported configuration, Microsoft targets must utilize the vcruntime library"
#  endif
#endif
/**
 * End Build Configuration
 */

// IWYU pragma: always_keep

// IWYU pragma: begin_exports
#include "utl/configuration/abi.h"              // IWYU pragma: keep
#include "utl/configuration/allocation.h"       // IWYU pragma: keep
#include "utl/configuration/architecture.h"     // IWYU pragma: keep
#include "utl/configuration/attributes.h"       // IWYU pragma: keep
#include "utl/configuration/builtins.h"         // IWYU pragma: keep
#include "utl/configuration/compiler.h"         // IWYU pragma: keep
#include "utl/configuration/compiler_barrier.h" // IWYU pragma: keep
#include "utl/configuration/exceptions.h"       // IWYU pragma: keep
#include "utl/configuration/keywords.h"         // IWYU pragma: keep
#include "utl/configuration/memcmp.h"           // IWYU pragma: keep
#include "utl/configuration/memcpy.h"           // IWYU pragma: keep
#include "utl/configuration/modules.h"          // IWYU pragma: keep
#include "utl/configuration/namespace.h"        // IWYU pragma: keep
#include "utl/configuration/pragma.h"           // IWYU pragma: keep
#include "utl/configuration/simd.h"             // IWYU pragma: keep
#include "utl/configuration/standard.h"         // IWYU pragma: keep
#include "utl/configuration/stl.h"              // IWYU pragma: keep
#include "utl/configuration/target.h"           // IWYU pragma: keep
#include "utl/configuration/types.h"            // IWYU pragma: keep

#include "utl/preprocessor/attribute_list.h" // IWYU pragma: keep
// IWYU pragma: end_exports

#if !UTL_CXX20
#  error "C++20 requirement not met"
#endif

// TODO append version?
#define __UTL_HIDE_FROM_ABI                                                    \
    __UTL_ABI_PRIVATE __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION __UTL_ABI_TAG( \
        __UTL_ODR_SIGNATURE)
#define __UTL_ATTRIBUTE__HIDE_FROM_ABI                                \
    __UTL_ATTRIBUTE__ABI_PRIVATE(EXCLUDE_FROM_EXPLICIT_INSTANTIATION) \
    (ABI_TAG(__UTL_ODR_SIGNATURE))
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE__HIDE_FROM_ABI

#define __UTL ::UTL_NS::

#define UTL_DEFAULT_NAMESPACE_BEGIN UTL_NAMESPACE_BEGIN(UTL_NS)

#define UTL_DEFAULT_NAMESPACE_END UTL_NAMESPACE_END(UTL_NS)

#define UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN \
    UTL_EXPORT UTL_DEFAULT_NAMESPACE_BEGIN

#define UTL_PUBLIC_DEFAULT_NAMESPACE_END UTL_DEFAULT_NAMESPACE_END

#if UTL_WITH_EXCEPTIONS

#  define UTL_THROW(...) throw(__VA_ARGS__)
#  define UTL_RETHROW(...) throw
#  define UTL_TRY try
#  define UTL_CATCH(...) catch (__VA_ARGS__)
#else

#  include <cassert>

#  define UTL_THROW(...)                           \
      assert(sizeof(decltype(__VA_ARGS__)*) == 0); \
      UTL_BUILTIN_unreachable()
#  define UTL_RETHROW(...) \
      assert(false);       \
      UTL_BUILTIN_unreachable()

#  define UTL_TRY if constexpr (true)

#  define UTL_CATCH(...)                                        \
      else if constexpr ([](auto&&) noexcept { return false; }( \
                             [](__VA_ARGS__) noexcept -> void {}))
#endif

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN

using ptrdiff_t = decltype((char*)0 - (char*)0);
using size_t = decltype(sizeof(0));

UTL_PUBLIC_DEFAULT_NAMESPACE_END
