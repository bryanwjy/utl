/* Copyright 2023-2025 Bryan Wong */

#include "utl/configuration/compiler.h"
#include "utl/configuration/target.h"

#if UTL_COMPILER_MSVC && !UTL_COMPILER_MSVC_AT_LEAST(1925)
#  define UTL_PRAGMA(ARG) __pragma(ARG)
#else
#  define UTL_PRAGMA(ARG) _Pragma(#ARG)
#endif

#if UTL_COMPILER_CLANG || UTL_COMPILER_ICX

#  define UTL_PRAGMA_WARN(x) UTL_PRAGMA(message #x)

#elif UTL_COMPILER_GCC

#  if UTL_COMPILER_GCC_AT_LEAST(9, 1, 0)
#    define UTL_PRAGMA_WARN(x) UTL_PRAGMA(GCC warning #x)
#  else /* UTL_COMPILER_GCC_AT_LEAST(9,1,0) */
#    define UTL_PRAGMA_WARN(x) UTL_PRAGMA(GCC message #x)
#  endif

#elif UTL_COMPILER_MSVC

#  define UTL_WARN_PRIVATE_STR2(x) #x
#  define UTL_WARN_PRIVATE_STR1(x) UTL_WARN_PRIVATE_STR2(x)
#  define UTL_WARN_PRIVATE_LOCATION \
      __FILE__ ":" UTL_WARN_PRIVATE_STR1(__LINE__) ":"
#  define UTL_PRAGMA_WARN(x) \
      UTL_PRAGMA(message(UTL_WARN_PRIVATE_LOCATION " warning: " #x))

#else

#  define UTL_PRAGMA_WARN(_)

#endif
/* TODO: Find out ICC's warning pragma */

#if UTL_COMPILER_MSVC | UTL_COMPILER_ICC
#  define UTL_DISABLE_WARNING_PUSH() UTL_PRAGMA(warning(push))
#  define UTL_DISABLE_WARNING_POP() UTL_PRAGMA(warning(pop))
#  define UTL_DISABLE_WARNING(warningNumber) \
      UTL_PRAGMA(warning(disable : warningNumber))
#elif UTL_COMPILER_CLANG | UTL_COMPILER_ICX
#  define UTL_DISABLE_WARNING_PUSH() UTL_PRAGMA(clang diagnostic push)
#  define UTL_DISABLE_WARNING_POP() UTL_PRAGMA(clang diagnostic pop)
#  define UTL_DISABLE_WARNING(warningName) \
      UTL_PRAGMA(clang diagnostic ignored warningName)
#elif UTL_COMPILER_GCC
#  define UTL_DISABLE_WARNING_PUSH() UTL_PRAGMA(GCC diagnostic push)
#  define UTL_DISABLE_WARNING_POP() UTL_PRAGMA(GCC diagnostic pop)
#  define UTL_DISABLE_WARNING(warningName) \
      UTL_PRAGMA(GCC diagnostic ignored warningName)
#else
#  define UTL_DISABLE_WARNING_PUSH()
#  define UTL_DISABLE_WARNING_POP()
#  define UTL_DISABLE_WARNING(warningName)
#endif
