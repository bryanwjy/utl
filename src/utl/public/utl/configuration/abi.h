/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/attributes.h"
#include "utl/configuration/compiler.h"
#include "utl/configuration/exceptions.h"
#include "utl/configuration/standard.h"
#include "utl/configuration/target.h"

#include "utl/preprocessor/concatenation.h"
#include "utl/preprocessor/to_string.h"

#ifndef UTL_DISABLE_VISIBILITY_ANNOTATIONS
#  define UTL_DISABLE_VISIBILITY_ANNOTATIONS 0
#endif

#if UTL_DISABLE_VISIBILITY_ANNOTATIONS

#  define __UTL_ABI_PRIVATE
#  define __UTL_ABI_PUBLIC
#  define __UTL_OVERRIDABLE_FUNCTION

#elif UTL_TARGET_MICROSOFT

#  if !UTL_HAS_DECLSPEC(dllexport) || !UTL_HAS_DECLSPEC(dllimport)
#    error "Unrecognized compiler"
#  endif

#  ifdef UTL_BUILDING_LIBRARY
#    define __UTL_ATTRIBUTE__ABI_PUBLIC (DLLEXPORT)
#    define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PUBLIC
#    define __UTL_ABI_PUBLIC __declspec(dllexport)
#    define __UTL_ATTRIBUTE__OVERRIDABLE_FUNCTION (DLLEXPORT)
#    define __UTL_ATTRIBUTE_TYPE_AGGREGATE__OVERRIDABLE_FUNCTION
#    define __UTL_OVERRIDABLE_FUNCTION __declspec(dllexport)
#  else
#    define __UTL_ATTRIBUTE__ABI_PUBLIC (DLLIMPORT)
#    define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PUBLIC
#    define __UTL_ABI_PUBLIC __declspec(dllimport)
#    define __UTL_OVERRIDABLE_FUNCTION
#  endif
#  define __UTL_ABI_PRIVATE
#  define __UTL_ATTRIBUTE__ABI_PRIVATE
#  ifdef __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PRIVATE
#    error '__UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PRIVATE' cannot be defined
#  endif

#else

#  if !UTL_HAS_GNU_ATTRIBUTE(__visibility__)
#    error "Unrecognized compiler"
#  endif

#  define __UTL_ATTRIBUTE__ABI_PUBLIC (VISIBILITY("default"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PUBLIC
#  define __UTL_ATTRIBUTE__ABI_PRIVATE (VISIBILITY("hidden"))
#  define __UTL_ATTRIBUTE_TYPE_AGGREGATE__ABI_PRIVATE

#  define __UTL_ABI_PUBLIC __attribute__((__visibility__("default")))
#  define __UTL_ABI_PRIVATE __attribute__((__visibility__("hidden")))
#  define __UTL_OVERRIDABLE_FUNCTION __attribute__((__visibility__("default")))

#endif

#if !UTL_HAS_GNU_ATTRIBUTE(TYPE_VISIBILITY) || \
    UTL_DISABLE_VISIBILITY_ANNOTATIONS
/* For GNU compilers that don't have type visibility we must keep the templates
 * visible */
#  define __UTL_PUBLIC_TYPE_VISIBILITY
#  define __UTL_PRIVTAE_TYPE_VISIBILITY
#else
#  define __UTL_PUBLIC_TYPE_VISIBILITY \
      __attribute__((__type_visibility__("default")))
#  define __UTL_PRIVTAE_TYPE_VISIBILITY \
      __attribute__((__type_visibility__("hidden")))
#endif

#define __UTL_ODR_SIGNATURE_1(_0, _1, _2) _0##_1##_2
#define __UTL_ODR_SIGNATURE_0(_0, _1, _2) __UTL_ODR_SIGNATURE_1(_0, _1, _2)
#define __UTL_ODR_SIGNATURE              \
    UTL_TO_STRING(__UTL_ODR_SIGNATURE_0( \
        utlcxx, __UTL_HARDENING_MODE, __UTL_ABI_EXCEPTION_TAG))

#if UTL_HAS_GNU_ATTRIBUTE(__exclude_from_explicit_instantiation__)
#  define __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
      __attribute__((__exclude_from_explicit_instantiation__))
#else
#  define __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#endif

#if UTL_HAS_GNU_ATTRIBUTE(__abi_tag__)
#  define __UTL_ABI_TAG(TAG) __attribute__((__abi_tag__(TAG)))
#else
#  define __UTL_ABI_TAG(TAG)
#endif

#define __UTL_HIDE_FROM_ABI_AS(TAG)
__UTL_ABI_PRIVATE __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION __UTL_ABI_TAG(TAG)
#define __UTL_ATTRIBUTE__HIDE_FROM_ABI_AS(TAG)                        \
    __UTL_ATTRIBUTE__ABI_PRIVATE(EXCLUDE_FROM_EXPLICIT_INSTANTIATION) \
    (ABI_TAG(TAG))
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE__HIDE_FROM_ABI_AS

#define __UTL_HIDE_FROM_ABI_UNTAGGED \
    __UTL_ABI_PRIVATE __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#define __UTL_ATTRIBUTE__HIDE_FROM_ABI_UNTAGGED \
    __UTL_ATTRIBUTE__ABI_PRIVATE(EXCLUDE_FROM_EXPLICIT_INSTANTIATION)
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE__HIDE_FROM_ABI_UNTAGGED

/* virtual functions must be linked to the same symbol */
#define __UTL_HIDE_FROM_ABI_VIRTUAL \
    __UTL_ABI_PRIVATE __UTL_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#define __UTL_ATTRIBUTE__HIDE_FROM_ABI_VIRTUAL \
    __UTL_ATTRIBUTE__ABI_PRIVATE(EXCLUDE_FROM_EXPLICIT_INSTANTIATION)
#define __UTL_ATTRIBUTE_TYPE_AGGREGATE__HIDE_FROM_ABI_VIRTUAL

#if UTL_WITH_EXCEPTIONS
#  define __UTL_ABI_EXCEPTION_TAG e
#else
#  define __UTL_ABI_EXCEPTION_TAG n
#endif

#ifndef UTL_COMPILER_TAG
#  error Undefined compiler tag
#endif

#ifndef __UTL_ABI_EXCEPTION_TAG
#  error Undefined exception tag
#endif

#define __UTL_HARDENING_MODE n

#if UTL_CXX
#  define UTL_EXTERN_C extern "C"
#  define UTL_EXTERN_C_BEGIN UTL_EXTERN_C {
#  define UTL_EXTERN_C_END }
#else
#  define UTL_EXTERN_C
#  define UTL_EXTERN_C_BEGIN
#  define UTL_EXTERN_C_END
#endif

#if UTL_COMPILER_MSVC && UTL_CXX20
#  define UTL_EXTERN_CXX extern "C++"
#  define UTL_EXTERN_CXX_BEGIN UTL_EXTERN_CXX {
#  define UTL_EXTERN_CXX_END }
#else
#  define UTL_EXTERN_CXX
#  define UTL_EXTERN_CXX_BEGIN
#  define UTL_EXTERN_CXX_END
#endif

#if UTL_SET_ABI_MICROSOFT || !UTL_SET_ABI_ITANIUM || UTL_TARGET_MICROSOFT
#  define UTL_ABI_MICROSOFT 1
#  define UTL_ABI_ITANIUM 0
#else
#  define UTL_ABI_MICROSOFT 0
#  define UTL_ABI_ITANIUM 1
#endif
