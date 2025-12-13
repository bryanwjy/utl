/* Copyright 2023-2025 Bryan Wong */

#ifdef __SIZEOF_INT128__
#  if !UTL_DISABLE_INT128_SUPPORT
#    define UTL_SUPPORTS_INT128 1
#  endif
#endif

#ifdef __STDCPP_FLOAT16_T__
#  define UTL_SUPPORTS_FLOAT16 1
#else
#  define UTL_SUPPORTS_FLOAT16 0
#endif
#ifdef __STDCPP_FLOAT32_T__
#  define UTL_SUPPORTS_FLOAT32 1
#else
#  define UTL_SUPPORTS_FLOAT32 0
#endif
#ifdef __STDCPP_FLOAT64_T__
#  define UTL_SUPPORTS_FLOAT64 1
#else
#  define UTL_SUPPORTS_FLOAT64 0
#endif
#ifdef __STDCPP_FLOAT128_T__
#  define UTL_SUPPORTS_FLOAT128 1
#else
#  define UTL_SUPPORTS_FLOAT128 0
#endif
#ifdef __STDCPP_BFLOAT16_T__
#  define UTL_SUPPORTS_BFLOAT16 1
#else
#  define UTL_SUPPORTS_BFLOAT16 0
#endif

#ifdef __cpp_char8_t
#  if __cpp_char8_t >= 201811L
#    define UTL_SUPPORTS_CHAR8_T 1
#  endif
#endif
