/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/preprocessor/utl_concatenation.h"

#define __UTL_COMPILER_TAG_CONCAT_1(NAME, MAJOR, MINOR, PATCH) NAME##_##MAJOR##MINOR##PATCH
#define __UTL_COMPILER_TAG_CONCAT(NAME, MAJOR, MINOR, PATCH) \
    __UTL_COMPILER_TAG_CONCAT_1(NAME, MAJOR, MINOR, PATCH)

#if defined(__INTEL_LLVM_COMPILER)
#  define UTL_COMPILER_ICX 1
#  define UTL_COMPILER_ICX_AT_LEAST(VERSION) __INTEL_LLVM_COMPILER >= VERSION
#  if defined(SYCL_LANGUAGE_VERSION)
#    define UTL_COMPILER_ICX_DPCPP 1
#    define UTL_COMPILER_ICX_DPCPP_AT_LEAST(VERSION) SYCL_LANGUAGE_VERSION >= VERSION
#    define UTL_COMPILER_TAG \
        __UTL_COMPILER_TAG_CONCAT(ICX, __INTEL_LLVM_COMPILER, DPCPP, SYCL_LANGUAGE_VERSION)
#  else
#    define UTL_COMPILER_TAG UTL_CONCAT(ICX, __INTEL_LLVM_COMPILER)
#  endif
#elif defined(__APPLE__)
#  ifndef __clang__
#    error "Unrecognized compiler for Apple platform"
#  endif
#  include "utl/configuration/utl_apple_clang.h"

#  if __UTL_APPLE_CLANG_MAJOR == 0
#    error Unrecognized AppleClang version, please update the version map in utl_apple_clang
#  endif

#  define UTL_COMPILER_CLANG 1
#  define UTL_COMPILER_APPLE_CLANG 1
#  define UTL_COMPILER_APPLE_CLANG_AT_LEAST(VERSION) __apple_build_version__ >= VERSION
#  define UTL_COMPILER_CLANG_AT_LEAST(MAJOR, MINOR, PATCH) \
      __UTL_APPLE_CLANG_MAJOR > MAJOR ||                   \
          (__UTL_APPLE_CLANG_MAJOR == MAJOR &&             \
              (__UTL_APPLE_CLANG_MINOR > MINOR ||          \
                  (__UTL_APPLE_CLANG_MINOR == MINOR && __UTL_APPLE_CLANG_PATCH >= PATCH)))

#  define UTL_COMPILER_TAG __UTL_COMPILER_TAG_CONCAT(AppleClang, __apple_build_version__, , )
#elif defined(__clang__)
#  define UTL_COMPILER_CLANG 1
#  define UTL_COMPILER_CLANG_AT_LEAST(MAJOR, MINOR, PATCH) \
      __clang_major__ > MAJOR ||                           \
          (__clang_major__ == MAJOR &&                     \
              (__clang_minor__ > MINOR ||                  \
                  (__clang_minor__ == MINOR && __clang_patchlevel__ >= PATCH)))

#  define UTL_COMPILER_TAG \
      __UTL_COMPILER_TAG_CONCAT(CLANG, __clang_major__, __clang_minor__, __clang_patchlevel__)
#elif defined(__INTEL_COMPILER)
#  define UTL_COMPILER_ICC 1
#  define UTL_COMPILER_ICC_AT_LEAST(YEAR, UPDATE) \
      __INTEL_COMPILER > YEAR || (__INTEL_COMPILER == YEAR && __INTEL_COMPILER_UPDATE >= UPDATE)
#  define UTL_COMPILER_TAG \
      __UTL_COMPILER_TAG_CONCAT(ICC, __INTEL_COMPILER, __INTEL_COMPILER_UPDATE, )

#elif defined(__GNUC__)
#  define UTL_COMPILER_GCC 1
#  define UTL_COMPILER_GCC_AT_LEAST(MAJOR, MINOR, PATCH) \
      __GNUC__ > MAJOR ||                                \
          (__GNUC__ == MAJOR &&                          \
              (__GNUC_MINOR__ > MINOR ||                 \
                  (__GNUC_MINOR__ == MINOR && __GNUC_PATCHLEVEL__ >= PATCH)))
#  define UTL_COMPILER_TAG \
      __UTL_COMPILER_TAG_CONCAT(GCC, __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#elif defined(_MSC_VER)
#  define UTL_COMPILER_MSVC 1
#  define UTL_SUPPORTS_DECLSPEC 1
#  define UTL_COMPILER_MSVC_AT_LEAST(VERSION) _MSC_VER >= VERSION
#  define UTL_COMPILER_TAG UTL_CONCAT(MSVC, _MSC_VER)
#  if _MSVC_TRADITIONAL
/* Using non-conformant MSVC preprocessor */
#    define UTL_MSVC_PREPROCESSOR 1
#  endif
#elif
#  error "Unknown compiler"
#endif

#ifndef UTL_COMPILER_CLANG_AT_LEAST
#  define UTL_COMPILER_CLANG_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_CLANG_AT_LEAST */

#ifndef UTL_COMPILER_APPLE_CLANG_AT_LEAST
#  define UTL_COMPILER_APPLE_CLANG_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_APPLE_CLANG_AT_LEAST */

#ifndef UTL_COMPILER_ICX_AT_LEAST
#  define UTL_COMPILER_ICX_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_ICX_AT_LEAST */

#ifndef UTL_COMPILER_ICC_AT_LEAST
#  define UTL_COMPILER_ICC_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_ICX_AT_LEAST */

#ifndef UTL_COMPILER_ICX_DPCPP_AT_LEAST
#  define UTL_COMPILER_ICX_DPCPP_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_ICX_DPCPP_AT_LEAST */

#ifndef UTL_COMPILER_GCC_AT_LEAST
#  define UTL_COMPILER_GCC_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_GCC_AT_LEAST */

#ifndef UTL_COMPILER_MSVC_AT_LEAST
#  define UTL_COMPILER_MSVC_AT_LEAST(...) 0
#endif /* ifndef UTL_COMPILER_MSVC_AT_LEAST */

#if UTL_COMPILER_MSVC && !UTL_COMPILER_MSVC_AT_LEAST(1914)
#  error "Minimum version for MSVC not met"
#endif

#if UTL_COMPILER_GCC && !UTL_COMPILER_GCC_AT_LEAST(4, 8, 1)
#  error "Minimum version for GCC not met"
#endif

#if UTL_COMPILER_CLANG && !UTL_COMPILER_CLANG_AT_LEAST(3, 4, 1)
/* Clang has a preprocessor bug in 3.3 */
#  error "Minimum version for Clang not met"
#endif

#ifdef __GNUC__
#  define UTL_COMPILER_GNU_BASED 1
#endif

#ifdef __clang__
#  define UTL_COMPILER_CLANG_BASED 1
#endif
