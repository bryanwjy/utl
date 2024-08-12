/* Copyright 2023-2024 Bryan Wong */

#pragma once

#include "utl/configuration/utl_compiler.h"

#if UTL_COMPILER_MSVC
#  define UTL_HAS_KEYWORD(NAME)                            \
      UTL_IS_EMPTY(UTL_CONCAT(__UTL_MSVC_KEYWORD, NAME)) | \
          UTL_IS_EMPTY(UTL_CONCAT(__UTL_SAL_ANNOTATION, NAME))

#  define UTL_HAS_SAL_ANNOTATION(NAME) UTL_IS_EMPTY(UTL_CONCAT(__UTL_SAL_ANNOTATION, NAME))

#  define UTL_CDECL __cdecl
#  define UTL_FASTCALL __fastcall
#  define UTL_STDCALL __stdcall
#  define UTL_RESTRICT __restrict
#  define UTL_UNALIGNED __unaligned
#  define UTL_VECTORCALL __vectorcall
#  define UTL_W64 __w64
#  define UTL_FORCEINLINE __forceinline

#  define __UTL_MSVC_KEYWORD__declspec
#  define __UTL_MSVC_KEYWORD__cdecl
#  define __UTL_MSVC_KEYWORD__fastcall
#  define __UTL_MSVC_KEYWORD__stdcall
#  define __UTL_MSVC_KEYWORD__restrict
#  define __UTL_MSVC_KEYWORD__vectorcall
#  define __UTL_MSVC_KEYWORD__unaligned
#  define __UTL_MSVC_KEYWORD__w64
#  define __UTL_MSVC_KEYWORD__forceinline

#  define __UTL_SAL_ANNOTATION_Check_return_

#elif UTL_COMPILER_GNU_BASED
#  define UTL_HAS_KEYWORD(NAME) UTL_IS_EMPTY(UTL_CONCAT(__UTL_GNU_KEYWORD, NAME))

#  define __UTL_GNU_KEYWORD__restrict__

#  define UTL_CDECL
#  define UTL_FASTCALL
#  define UTL_STDCALL
#  define UTL_RESTRICT __restrict__
#  define UTL_UNALIGNED
#  define UTL_VECTORCALL
#  define UTL_W64
#  define UTL_FORCEINLINE
#else

#  define UTL_HAS_KEYWORD(NAME) 0
#  define UTL_CDECL
#  define UTL_FASTCALL
#  define UTL_STDCALL
#  define UTL_RESTRICT
#  define UTL_UNALIGNED
#  define UTL_VECTORCALL
#  define UTL_W64
#  define UTL_FORCEINLINE
#endif
