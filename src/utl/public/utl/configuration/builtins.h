/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/builtin_check.h"
#include "utl/configuration/compiler.h"
#include "utl/configuration/msvc_builtins.h" // IWYU pragma: keep
#include "utl/configuration/standard.h"      // IWYU pragma: keep

#if UTL_HAS_BUILTIN(__builtin_launder)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#elif UTL_COMPILER_GCC_AT_LEAST(7, 1, 0)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#elif UTL_COMPILER_ICC_AT_LEAST(1900)
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#elif UTL_COMPILER_ICX
#  define UTL_BUILTIN_launder(X) __builtin_launder(X)
#endif /* UTL_HAS_BUILTIN(__builtin_launder) */

#if UTL_HAS_BUILTIN(__builtin_addressof)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#elif UTL_COMPILER_GCC_AT_LEAST(7, 1, 0)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#elif UTL_COMPILER_MSVC_AT_LEAST(1914)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#elif UTL_COMPILER_ICC_AT_LEAST(1900)
#  define UTL_BUILTIN_addressof(X) __builtin_addressof(X)
#endif /* if UTL_HAS_BUILTIN(__builtin_addressof) */

#if UTL_HAS_BUILTIN(__builtin_unreachable)
#  define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif UTL_COMPILER_GCC_AT_LEAST(4, 5, 0)
#  define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif UTL_COMPILER_ICC_AT_LEAST(1300)
#  define UTL_BUILTIN_unreachable() __builtin_unreachable()
#elif UTL_COMPILER_MSVC /* UTL_HAS_BUILTIN(__builtin_unreachable) */
#  define UTL_BUILTIN_unreachable() __assume(0)
#else /* UTL_HAS_BUILTIN(__builtin_unreachable) */

#  if UTL_HAS_BUILTIN(__builtin_expect)
#    define UTL_BUILTIN_expect(VALUE, EXP) __builtin_expect(VALUE, EXP)
#  else
#    define UTL_BUILTIN_expect(VALUE, _1) VALUE
#  endif

#  ifdef UTL_CXX
extern "C" void abort(void);
#  else
void abort(void);
#  endif

#  define UTL_BUILTIN_unreachable() abort()
#endif /* UTL_HAS_BUILTIN(__builtin_unreachable) */

#if UTL_HAS_BUILTIN(__builtin_is_constant_evaluated)
#  define UTL_BUILTIN_is_constant_evaluated() __builtin_is_constant_evaluated()
#endif /* UTL_HAS_BUILTIN(__builtin_is_constant_evaluated) */

#if UTL_HAS_BUILTIN(__builtin_bit_cast)
#  define UTL_BUILTIN_bit_cast(...) __builtin_bit_cast(__VA_ARGS__)
#endif /* UTL_HAS_BUILTIN(__builtin_is_constant_evaluated) */

#if UTL_HAS_BUILTIN(__builtin_assume)
#  define UTL_BUILTIN_assume(...) __builtin_assume(__VA_ARGS__)
#elif UTL_COMPILER_MSVC /* UTL_HAS_BUILTIN(__builtin_unreachable) */
#  define UTL_BUILTIN_assume(...) __assume(__VA_ARGS__)
#elif UTL_HAS_BUILTIN(__builtin_unreachable)
#  define UTL_BUILTIN_assume(...) \
      ((__VA_ARGS__) ? (void)0 : __builtin_unreachable())
#else
#  define UTL_BUILTIN_assume(...) (void)0
#endif /* UTL_HAS_BUILTIN(__builtin_assume) */

#if UTL_HAS_BUILTIN(__is_layout_compatible) || \
    UTL_COMPILER_GCC_AT_LEAST(12, 0, 0)
#  define UTL_BUILTIN_is_layout_compatible(...) \
      __is_layout_compatible(__VA_ARGS__)
#endif

#if UTL_HAS_BUILTIN(__is_pointer_interconvertible_base_of) || \
    UTL_COMPILER_GCC_AT_LEAST(12, 0, 0)
#  define UTL_BUILTIN_is_pointer_interconvertible_base_of(...) \
      __is_pointer_interconvertible_base_of(__VA_ARGS__)
#endif

#if UTL_HAS_BUILTIN(__builtin_source_location)
#  define UTL_BUILTIN_source_location() __builtin_source_location()
#endif

#if UTL_HAS_BUILTIN(__builtin_FILE)
#  define UTL_BUILTIN_FILE() __builtin_FILE()
#endif

#if UTL_HAS_BUILTIN(__builtin_FUNCTION)
#  define UTL_BUILTIN_FUNCTION() __builtin_FUNCTION()
#endif

#if UTL_HAS_BUILTIN(__builtin_LINE)
#  define UTL_BUILTIN_LINE() __builtin_LINE()
#endif

#if UTL_HAS_BUILTIN(__builtin_COLUMN)
#  define UTL_BUILTIN_COLUMN() __builtin_COLUMN()
#endif

#if UTL_HAS_BUILTIN(__builtin_FUNCSIG)
#  define UTL_BUILTIN_FUNCSIG() __builtin_FUNCSIG()
#endif
