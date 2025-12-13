/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/compiler.h" // IWYU pragma: keep

#if UTL_COMPILER_MSVC
#  include "utl/preprocessor/concatenation.h"
#  include "utl/preprocessor/is_empty.h"

#  define UTL_HAS_DECLSPEC(NAME) UTL_IS_EMPTY(UTL_CONCAT(UTL_DECLSPEC_, NAME))

#  define UTL_DECLSPEC_dllexport
#  define UTL_DECLSPEC_dllimport
#  define UTL_DECLSPEC_allocator
#  define UTL_DECLSPEC_deprecated
#  define UTL_DECLSPEC_empty_bases
#  define UTL_DECLSPEC_noalias
#  define UTL_DECLSPEC_noinline
#  define UTL_DECLSPEC_noreturn
#  define UTL_DECLSPEC_novtable
#  define UTL_DECLSPEC_no_sanitize_address
#  define UTL_DECLSPEC_restrict

#elif UTL_COMPILER_ARMCC /* NOT DEFINED */

#  define UTL_HAS_DECLSPEC(NAME) UTL_IS_EMPTY(UTL_CONCAT(UTL_DECLSPEC_, NAME))

#  define UTL_DECLSPEC_dllexport
#  define UTL_DECLSPEC_dllimport
#  define UTL_DECLSPEC_noinline
#  define UTL_DECLSPEC_noreturn
#  define UTL_DECLSPEC_notshared

#else
#  define UTL_HAS_DECLSPEC(NAME) 0
#endif
