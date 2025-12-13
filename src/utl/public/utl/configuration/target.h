/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/compiler.h"

#if UTL_COMPILER_CLANG

#  if defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64) || \
      defined(_MSC_VER)
#    define UTL_TARGET_MICROSOFT 1
#  elif defined(__APPLE__)
#    define UTL_TARGET_APPLE 1
#  elif defined(RENAME_TO_SONY_IDENTIFIER) /* NDA */
#    define UTL_TARGET_SONY 1
#  elif defined(RENAME_TO_NINTENDO_IDENTIFIER) /* NDA */
#    define UTL_TARGET_NINTENDO 1
#  elif defined(__FreeBSD__) || defined(__DragonFly__) || \
      defined(__NetBSD__) || defined(__OpenBSD__)
#    define UTL_TARGET_BSD 1
#  elif defined(__linux__)
#    define UTL_TARGET_LINUX 1
#  elif defined(__unix__)
#    define UTL_TARGET_UNIX 1
#  else
#    error "Unrecognized target for Clang Compiler"
#  endif

#elif UTL_COMPILER_GCC

#  if defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
#    define UTL_TARGET_MICROSOFT 1
#  elif defined(__APPLE__)
#    define UTL_TARGET_APPLE 1
#  elif defined(__FreeBSD__) || defined(__DragonFly__) || \
      defined(__NetBSD__) || defined(__OpenBSD__)
#    define UTL_TARGET_BSD 1
#  elif defined(__linux__)
#    define UTL_TARGET_LINUX 1
#  elif defined(__unix__)
#    define UTL_TARGET_UNIX 1
#  else
#    error "Unrecognized target for GNU Compiler"
#  endif

#elif UTL_COMPILER_MSVC

#  define UTL_TARGET_MICROSOFT 1

#elif UTL_COMPILER_ICC || UTL_COMPILER_ICX

#  if defined(__CYGWIN__) || defined(_WIN32) || defined(_WIN64)
#    define UTL_TARGET_MICROSOFT 1
#  elif defined(__APPLE__)
#    define UTL_TARGET_APPLE 1
#  elif defined(__FreeBSD__) || defined(__DragonFly__) || \
      defined(__NetBSD__) || defined(__OpenBSD__)
#    define UTL_TARGET_BSD 1
#  elif defined(__linux__)
#    define UTL_TARGET_LINUX 1
#  elif defined(__unix__)
#    define UTL_TARGET_UNIX 1
#  else
#    error "Unrecognized target for Intel Compiler"
#  endif

#endif /* if UTL_COMPILER_CLANG */

#if UTL_TARGET_APPLE && !UTL_TARGET_UNIX
#  define UTL_TARGET_UNIX 1
#endif /* if UTL_TARGET_APPLE && !defined(UTL_TARGET_UNIX) */

#if UTL_TARGET_LINUX && !UTL_TARGET_UNIX
#  define UTL_TARGET_UNIX 1
#endif /* if UTL_TARGET_LINUX && !defined(UTL_TARGET_UNIX) */

#if UTL_TARGET_SONY
#  if !UTL_TARGET_BSD
#    define UTL_TARGET_BSD 1
#  endif
#  if !UTL_TARGET_UNIX
#    define UTL_TARGET_UNIX 1
#  endif
#endif /* if UTL_TARGET_SONY && !defined(UTL_TARGET_BSD) */

#if UTL_TARGET_BSD && !UTL_TARGET_UNIX
#  define UTL_TARGET_UNIX 1
#endif /* if UTL_TARGET_BSD && !defined(UTL_TARGET_UNIX) */

#if UTL_TARGET_SONY
// AFAIK the Pro models are not dedicated targets, I could be wrong though
#  if RENAME_TO_PS4_IDENTIFIER /* NDA */
#    define UTL_TARGET_SONY_PS4 1
#  elif RENAME_TO_PS5_IDENTIFIER /* NDA */
#    define UTL_TARGET_SONY_PS5 1
#  else
#    error "Unrecognized Sony target"
#  endif
#endif /* if UTL_TARGET_SONY */

#if UTL_TARGET_MICROSOFT

#  if defined(__CYGWIN__)
#    define UTL_TARGET_MICROSOFT_WINDOWS 1
#    define UTL_TARGET_MICROSOFT_CYGWIN 1
#  elif defined(_WIN32) || defined(_WIN64)
#    define UTL_TARGET_MICROSOFT_WINDOWS 1
#  elif RENAME_TO_GDK_XBO_IDENTIFIER /* NDA */
#    define UTL_TARGET_MICROSOFT_GDK_XBO 1
#  elif RENAME_TO_GDK_XSX_IDENTIFIER /* NDA */
#    define UTL_TARGET_MICROSOFT_GDK_XSX 1
#  else
#    error "Unrecognized Microsoft target"
#  endif

#  if RENAME_TO_DESKTOP_GDK_IDENTIFIER /* NDA */
#    define UTL_TARGET_MICROSOFT_GDK_WINDOWS 1
#  endif

#endif /* if UTL_TARGET_MICROSOFT */

#if UTL_COMPILER_CLANG | UTL_COMPILER_GCC | UTL_COMPILER_ICX | \
    (UTL_COMPILER_ICC & !UTL_TARGET_MICROSOFT)
#  define UTL_SUPPORTS_GNU_ASM 1
#endif

#ifdef __wasm__
#  error "Unsupported target"
#endif
