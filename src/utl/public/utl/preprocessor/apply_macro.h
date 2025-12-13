/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/compiler.h"
#include "utl/configuration/standard.h"
#include "utl/preprocessor/paste.h"

#define UTL_EVAL1(...) UTL_PASTE(UTL_PASTE(UTL_PASTE(__VA_ARGS__)))
#define UTL_EVAL2(...) UTL_EVAL1(UTL_EVAL1(UTL_EVAL1(__VA_ARGS__)))
#define UTL_EVAL3(...) UTL_EVAL2(UTL_EVAL2(UTL_EVAL2(__VA_ARGS__)))
#define UTL_EVAL4(...) UTL_EVAL3(UTL_EVAL3(UTL_EVAL3(__VA_ARGS__)))
#define UTL_EVAL(...) UTL_EVAL4(UTL_EVAL4(UTL_EVAL4(__VA_ARGS__)))

#if UTL_CXX20 && !UTL_MSVC_PREPROCESSOR

#  define UTL_APPLY_MACRO_PARENTHESIS ()
#  define UTL_APPLY_MACRO_IMPL(F, _0, ...)                                   \
      F(_0)                                                                  \
      __VA_OPT__(UTL_APPLY_MACRO_LOOP UTL_APPLY_MACRO_PARENTHESIS UTL_PASTE( \
          (F, __VA_ARGS__)))
#  define UTL_APPLY_MACRO_LOOP() UTL_APPLY_MACRO_IMPL

#  define UTL_APPLY_MACRO(F, ...) \
      __VA_OPT__(UTL_EVAL(UTL_APPLY_MACRO_IMPL(F, __VA_ARGS__)))

#else

#  define UTL_APPLY_MACRO_END(...)
#  define UTL_APPLY_MACRO_OUT
#  define UTL_APPLY_MACRO_COMMA ,

#  define UTL_APPLY_MACRO_GET_END2() 0, UTL_APPLY_MACRO_END
#  define UTL_APPLY_MACRO_GET_END1(...) UTL_APPLY_MACRO_GET_END2
#  define UTL_APPLY_MACRO_GET_END(...) UTL_APPLY_MACRO_GET_END1
#  define UTL_APPLY_MACRO_NEXT0(test, next, ...) next UTL_APPLY_MACRO_OUT
#  if !UTL_MSVC_PREPROCESSOR
#    define UTL_APPLY_MACRO_NEXT1(test, next) \
        UTL_APPLY_MACRO_NEXT0(test, next, 0)
#    define UTL_APPLY_MACRO_NEXT(test, next) \
        UTL_APPLY_MACRO_NEXT1(UTL_APPLY_MACRO_GET_END test, next)
#    define UTL_APPLY_MACRO0(f, x, peek, ...) \
        f(x) UTL_APPLY_MACRO_NEXT(peek, UTL_APPLY_MACRO1)(f, peek, __VA_ARGS__)
#    define UTL_APPLY_MACRO1(f, x, peek, ...) \
        f(x) UTL_APPLY_MACRO_NEXT(peek, UTL_APPLY_MACRO0)(f, peek, __VA_ARGS__)
#  else /* if !UTL_MSVC_PREPROCESSOR */
#    define UTL_APPLY_MACRO_NEXT1(test, next) \
        UTL_PASTE(UTL_APPLY_MACRO_NEXT0(test, next, 0))
#    define UTL_APPLY_MACRO_NEXT(test, next) \
        UTL_PASTE(UTL_APPLY_MACRO_NEXT1(UTL_APPLY_MACRO_GET_END test, next))
#    define UTL_APPLY_MACRO0(f, x, peek, ...)                        \
        f(x) UTL_PASTE(UTL_APPLY_MACRO_NEXT(peek, UTL_APPLY_MACRO1)( \
            f, peek, __VA_ARGS__))
#    define UTL_APPLY_MACRO1(f, x, peek, ...)                        \
        f(x) UTL_PASTE(UTL_APPLY_MACRO_NEXT(peek, UTL_APPLY_MACRO0)( \
            f, peek, __VA_ARGS__))
#  endif /* if !UTL_MSVC_PREPROCESSOR */

#  define UTL_APPLY_MACRO_LIST_NEXT1(test, next) \
      UTL_APPLY_MACRO_NEXT0(test, UTL_APPLY_MACRO_COMMA next, 0)
#  define UTL_APPLY_MACRO_LIST_NEXT(test, next) \
      UTL_APPLY_MACRO_LIST_NEXT1(UTL_APPLY_MACRO_GET_END test, next)

#  define UTL_APPLY_MACRO_LIST0(f, x, peek, ...)                             \
      f(x) UTL_PASTE(UTL_APPLY_MACRO_LIST_NEXT(peek, UTL_APPLY_MACRO_LIST1)( \
          f, peek, __VA_ARGS__))
#  define UTL_APPLY_MACRO_LIST1(f, x, peek, ...)                             \
      f(x) UTL_PASTE(UTL_APPLY_MACRO_LIST_NEXT(peek, UTL_APPLY_MACRO_LIST0)( \
          f, peek, __VA_ARGS__))

/**
 * Applies the function macro `f` to each of the remaining parameters.
 */
#  define UTL_APPLY_MACRO(f, ...) \
      UTL_EVAL(UTL_APPLY_MACRO1(f, __VA_ARGS__, ()()(), ()()(), ()()(), 0))

#endif /* if UTL_CXX20 */
