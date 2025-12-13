/* Copyright 2023-2025 Bryan Wong */

#pragma once

#ifdef UTL_MODULES
#  if !UTL_MODULES
#    undef UTL_MODULES
#  endif
#endif

#ifdef UTL_MODULES
#  define UTL_EXPORT export
#  define UTL_EXPORT_BEGIN export {
#  define UTL_EXPORT_END }
#else
#  define UTL_EXPORT
#  define UTL_EXPORT_BEGIN
#  define UTL_EXPORT_END
#endif
