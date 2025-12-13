/* Copyright 2023-2025 Bryan Wong */

#pragma once

#ifdef __is_identifier
#  define UTL_IS_RESERVED_IDENTIFIER(X) !__is_identifier(X)
#else
#  define UTL_IS_RESERVED_IDENTIFIER(...) 0
#endif

#ifdef __has_builtin
#  define UTL_HAS_BUILTIN(BUILTIN) \
      (__has_builtin(BUILTIN) || UTL_IS_RESERVED_IDENTIFIER(BUILTIN))
#else
#  define UTL_HAS_BUILTIN(...) 0
#endif /* ifdef __has_builtin */

#define __UTL_SHOULD_USE_BUILTIN(BUILTIN) \
    (UTL_HAS_BUILTIN(__##BUILTIN) && !UTL_DISABLE_BUILTIN_##BUILTIN)
