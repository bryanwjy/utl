/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/architecture.h"

#if UTL_ARCH_x86

/* Use SSE4.2 as a minimum SIMD support */
#  ifdef __SSE4_2__
#    define UTL_SIMD_X86_SSE4_2 __SSE4_2__
#  endif
#  ifdef __AVX__
#    define UTL_SIMD_X86_AVX __AVX__
#  endif

#  ifdef __AVX2__
#    define UTL_SIMD_X86_AVX2 __AVX2__
#  endif

#  ifdef __AVX512BF16__
#    define UTL_SIMD_X86_AVX512BF16 __AVX512BF16__
#  endif

#  ifdef __AVX512FP16__
#    define UTL_SIMD_X86_AVX512FP16 __AVX512FP16__
#  endif

#  ifdef __AVX512BW__
#    define UTL_SIMD_X86_AVX512BW __AVX512BW__
#  endif

#  ifdef __AVX512CD__
#    define UTL_SIMD_X86_AVX512CD __AVX512CD__
#  endif

#  ifdef __AVX512DQ__
#    define UTL_SIMD_X86_AVX512DQ __AVX512DQ__
#  endif

#  ifdef __AVX512ER__
#    define UTL_SIMD_X86_AVX512ER __AVX512ER__
#  endif

#  ifdef __AVX512PF__
#    define UTL_SIMD_X86_AVX512PF __AVX512PF__
#  endif

#  ifdef __AVX512VL__
#    define UTL_SIMD_X86_AVX512VL __AVX512VL__
#  endif

#  ifdef __AVX512F__
#    define UTL_SIMD_X86_AVX512 __AVX512F__
#  endif

#elif UTL_ARCH_ARM

#  ifdef __ARM_NEON
#    define UTL_SIMD_ARM_NEON 1
#  endif

#  ifdef __ARM_FEATURE_SVE
#    define UTL_SIMD_ARM_SVE 1
#    ifdef __ARM_FEATURE_SVE_BITS
#      define UTL_SIMD_ARM_SVE_BITS __ARM_FEATURE_SVE_BITS
#    endif
#  endif

#  ifdef __ARM_FEATURE_SVE2
#    define UTL_SIMD_ARM_SVE2 1
#  endif
#endif
