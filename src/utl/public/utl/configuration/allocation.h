/* Copyright 2023-2025 Bryan Wong */

#pragma once

#ifndef __cpp_aligned_new
#  define UTL_SUPPORTS_ALIGNED_ALLOCATION 0
#elif __cpp_aligned_new >= 201606L
#  define UTL_SUPPORTS_ALIGNED_ALLOCATION 1
#else
#  define UTL_SUPPORTS_ALIGNED_ALLOCATION 0
#endif

#ifndef __cpp_sized_deallocation
#  define UTL_SUPPORTS_SIZED_DEALLOCATION 0
#elif __cpp_sized_deallocation >= 201309L
#  define UTL_SUPPORTS_SIZED_DEALLOCATION 1
#else
#  define UTL_SUPPORTS_SIZED_DEALLOCATION 0
#endif

#ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
#  define UTL_DEFAULT_NEW_ALIGNMENT __STDCPP_DEFAULT_NEW_ALIGNMENT__
#endif
