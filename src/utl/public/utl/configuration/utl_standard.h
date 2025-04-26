/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_compiler.h"

#if UTL_COMPILER_MSVC
#  ifdef __cplusplus
#    define UTL_CXX _MSVC_LANG
#  endif
#elif defined(__cplusplus)
#  define UTL_CXX __cplusplus
#endif

#ifdef UTL_CXX

#  if UTL_CXX < 201103L
#    error "C++11 is required"
#  endif /* UTL_CXX < 201103L */

#  define UTL_CXX11 1

#  if UTL_CXX >= 201402L
#    define UTL_CXX14 1
#    define UTL_CONSTEXPR_CXX14 constexpr
#  else /* UTL_CXX >= 201402L */
#    define UTL_CONSTEXPR_CXX14
#  endif /* UTL_CXX >= 201402L */

#  if UTL_CXX >= 201703L
#    define UTL_CXX17 1
#    define UTL_CONSTEXPR_CXX17 constexpr
#    define UTL_INLINE_CXX17 inline
#    define UTL_IF_CONSTEXPR if constexpr
#  else /* UTL_CXX >= 201703L */
#    define UTL_INLINE_CXX17
#    define UTL_CONSTEXPR_CXX17
#    define UTL_IF_CONSTEXPR if
#  endif /* UTL_CXX >= 201703L */

#  if UTL_CXX >= 202002L
#    define UTL_CXX20 1
#    define UTL_CONSTEXPR_CXX20 constexpr
#    define UTL_CONSTEVAL consteval
#    define UTL_EXPLICIT_IF(...) explicit(__VA_ARGS__)
#    define UTL_IMPLICIT_IF(...) explicit(!(__VA_ARGS__))

#    define UTL_TEMPLATE_CXX11(...)
#    define UTL_ENABLE_IF_CXX11(TYPE, ...) TYPE
#    define UTL_CONSTRAINT_CXX11(...)
#    define UTL_CONSTRAINT_CXX20(...) requires (__VA_ARGS__)
#    define UTL_CONCEPT_CXX20(...) __VA_ARGS__

#  else /* UTL_CXX >= 202002L */

#    define UTL_CONSTEXPR_CXX20
#    define UTL_CONSTEVAL constexpr
#    define UTL_EXPLICIT_IF(...)
#    define UTL_IMPLICIT_IF(...) explicit
#    define UTL_TEMPLATE_CXX11(...) , __VA_ARGS__
#    define UTL_ENABLE_IF_CXX11(TYPE, ...) __UTL enable_if_t<(__VA_ARGS__), TYPE>
#    define UTL_CONSTRAINT_CXX11(...) , __UTL enable_if_t<(__VA_ARGS__), int> = __LINE__
#    define UTL_CONSTRAINT_CXX20(...)
#    define UTL_CONCEPT_CXX20(...) typename
#  endif /* UTL_CXX >= 202002L */

#  if UTL_CXX >= 202302L
#    define UTL_CXX23 1
#    define UTL_CONSTEXPR_CXX23 constexpr
#    define UTL_CONSTEVAL_CXX23 consteval
#    define UTL_IF_CONSTEVAL(...) if consteval
#  else /* UTL_CXX >= 202302L */
#    define UTL_CONSTEXPR_CXX23
#    define UTL_CONSTEVAL_CXX23 constexpr
#    define UTL_IF_CONSTEVAL(...) \
        if (UTL_CONSTANT_P(__VA_ARGS__)) /* This requires including 'utl_constant_p.h' */

#  endif /* UTL_CXX >= 202302L */

#else /* ifdef UTL_CXX */

/* Not C++, so only define qualifiers usable on global functions/variables */
#  define UTL_CONSTEXPR_CXX14 const
#  define UTL_INLINE_CXX17
#  define UTL_CONSTEXPR_CXX17 const
#  define UTL_CONSTEXPR_CXX20 const
#  define UTL_CONSTEXPR_CXX23 const
#  define UTL_CONSTEVAL

#endif /* ifdef UTL_CXX */
