/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/abi.h"

#define UTL_NAMESPACE_BEGIN(NAME) namespace __UTL_PUBLIC_TYPE_VISIBILITY NAME {

#define UTL_NAMESPACE_END(NAME) }

#ifdef _LIBCPP_ABI_NAMESPACE
#  define UTL_STD_ABI_NAMESPACE_BEGIN inline namespace _LIBCPP_ABI_NAMESPACE {
#  define UTL_STD_ABI_NAMESPACE_END }
#elif defined(_GLIBCXX_BEGIN_NAMESPACE_VERSION)
#  define UTL_STD_ABI_NAMESPACE_BEGIN _GLIBCXX_BEGIN_NAMESPACE_VERSION
#  define UTL_STD_ABI_NAMESPACE_END _GLIBCXX_END_NAMESPACE_VERSION
#endif

#ifndef UTL_STD_ABI_NAMESPACE_BEGIN
#  define UTL_STD_ABI_NAMESPACE_BEGIN
#  define UTL_STD_ABI_NAMESPACE_END
#endif

/* extern C++ for MSVC > C++20, no effect anywhere else */
/* Note: Does not apply to all std entities */
#define UTL_STD_NAMESPACE_BEGIN                                              \
    UTL_EXTERN_CXX_BEGIN namespace UTL_ATTRIBUTE(TYPE_VISIBILITY("default")) \
        std {                                                                \
        UTL_STD_ABI_NAMESPACE_BEGIN

#define UTL_STD_NAMESPACE_END \
    UTL_STD_ABI_NAMESPACE_END \
    }                         \
    UTL_EXTERN_CXX_END
