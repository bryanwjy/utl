/* Copyright 2023-2025 Bryan Wong */

#pragma once

#include "utl/configuration/compiler.h" // IWYU pragma: keep
#include "utl/configuration/declspec.h" // IWYU pragma: keep
#include "utl/configuration/keywords.h" // IWYU pragma: keep
#include "utl/configuration/standard.h"

#if defined(__cplusplus) && defined(__has_cpp_attribute)
#  define UTL_HAS_CPP_ATTRIBUTE(NAME) __has_cpp_attribute(NAME)
#else /* ifdef __has_cpp_attribute */
#  define UTL_HAS_CPP_ATTRIBUTE(NAME) 0
#endif /* ifdef __has_cpp_attribute */

#ifdef __has_attribute
#  define UTL_HAS_GNU_ATTRIBUTE(NAME) __has_attribute(NAME)
#else /* ifdef __has_attribute */
#  define UTL_HAS_GNU_ATTRIBUTE(NAME) 0
#endif /* ifdef __has_attribute */

#if UTL_HAS_CPP_ATTRIBUTE(nodiscard) && UTL_CXX17
#  define UTL_NODISCARD [[nodiscard]]
#  define __UTL_ATTRIBUTE_NODISCARD nodiscard
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::warn_unused_result)
#  define UTL_NODISCARD [[gnu::warn_unused_result]]
#  define __UTL_ATTRIBUTE_NODISCARD gnu::warn_unused_result
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD
#elif UTL_HAS_CPP_ATTRIBUTE(clang::warn_unused_result)
#  define UTL_NODISCARD [[clang::warn_unused_result]]
#  define __UTL_ATTRIBUTE_NODISCARD clang::warn_unused_result
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODISCARD
#elif UTL_HAS_GNU_ATTRIBUTE(__warn_unused_result__)
#  define UTL_NODISCARD __attribute__((__warn_unused_result__))
#  define __UTL_ATTRIBUTE_NODISCARD __warn_unused_result__
#  define __UTL_ATTRIBUTE_TYPE_GNU_NODISCARD
#elif UTL_HAS_KEYWORD(_Check_return_)
#  define UTL_NODISCARD _Check_return_
#  define __UTL_ATTRIBUTE_NODISCARD _Check_return_
#  define __UTL_ATTRIBUTE_TYPE_MSVC_EXT_NODISCARD
#else
#  define UTL_NODISCARD
#endif /* UTL_HAS_CPP_ATTRIBUTE(nodiscard) */

#if UTL_HAS_CPP_ATTRIBUTE(noreturn)
#  define UTL_NORETURN [[noreturn]]
#  define __UTL_ATTRIBUTE_NORETURN noreturn
#  define __UTL_ATTRIBUTE_TYPE_CPP_NORETURN
#else
#  define UTL_NORETURN
#endif

#if UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound)
#  define UTL_LIFETIMEBOUND [[msvc::lifetimebound]]
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND msvc::lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIFETIMEBOUND
#elif UTL_HAS_CPP_ATTRIBUTE( \
    clang::lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define UTL_LIFETIMEBOUND [[clang::lifetimebound]]
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND clang::lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIFETIMEBOUND
#elif UTL_HAS_CPP_ATTRIBUTE( \
    gnu::lifetimebound) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define UTL_LIFETIMEBOUND [[gnu::lifetimebound]]
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND gnu::lifetimebound
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIFETIMEBOUND
#elif UTL_HAS_GNU_ATTRIBUTE( \
    __lifetimebound__) /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */
#  define UTL_LIFETIMEBOUND __attribute__((__lifetimebound__))
#  define __UTL_ATTRIBUTE_LIFETIMEBOUND __lifetimebound__
#  define __UTL_ATTRIBUTE_TYPE_GNU_LIFETIMEBOUND
#else
#  define UTL_LIFETIMEBOUND
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::lifetimebound) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::pure)
#  define __UTL_ATTRIBUTE_PURE gnu::pure
#  define __UTL_ATTRIBUTE_TYPE_CPP_PURE
#elif UTL_HAS_GNU_ATTRIBUTE(__pure__) /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */
#  define __UTL_ATTRIBUTE_PURE __pure__
#  define __UTL_ATTRIBUTE_TYPE_GNU_PURE
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::pure) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::const)
#  define __UTL_ATTRIBUTE_CONST gnu::const
#  define __UTL_ATTRIBUTE_TYPE_CPP_CONST
#elif UTL_HAS_GNU_ATTRIBUTE(__const__)
#  define __UTL_ATTRIBUTE_CONST __const__
#  define __UTL_ATTRIBUTE_TYPE_GNU_CONST
#elif UTL_HAS_DECLSPEC(noalias)
#  define __UTL_ATTRIBUTE_CONST noalias
#  define __UTL_ATTRIBUTE_TYPE_DECLSPEC_CONST
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::weak)
#  define __UTL_ATTRIBUTE_WEAK gnu::weak
#  define __UTL_ATTRIBUTE_TYPE_CPP_WEAK
#elif UTL_HAS_GNU_ATTRIBUTE(__weak__) /* UTL_HAS_CPP_ATTRIBUTE(gnu::weak) */
#  define __UTL_ATTRIBUTE_WEAK __weak__
#  define __UTL_ATTRIBUTE_TYPE_GNU_WEAK
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::weak) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic)
#  define __UTL_ATTRIBUTE_INTRINSIC msvc::intrinsic
#  define __UTL_ATTRIBUTE_TYPE_CPP_INTRINSIC
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::constexpr)
#  define __UTL_ATTRIBUTE_MSVC_CONSTEXPR msvc::constexpr
#  define __UTL_ATTRIBUTE_TYPE_CPP_MSVC_CONSTEXPR
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::intrinsic) */

#if UTL_HAS_CPP_ATTRIBUTE(gnu::flatten)
#  define __UTL_ATTRIBUTE_FLATTEN gnu::flatten
#  define __UTL_ATTRIBUTE_TYPE_CPP_FLATTEN
#elif UTL_HAS_CPP_ATTRIBUTE(msvc::flatten)
#  define __UTL_ATTRIBUTE_FLATTEN msvc::flatten
#  define __UTL_ATTRIBUTE_TYPE_CPP_FLATTEN
#elif UTL_HAS_GNU_ATTRIBUTE(__flatten__)
#  define __UTL_ATTRIBUTE_FLATTEN __flatten__
#  define __UTL_ATTRIBUTE_TYPE_GNU_FLATTEN
#endif /* UTL_HAS_CPP_ATTRIBUTE(gnu::const) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS msvc::no_unique_address
#  define __UTL_ATTRIBUTE_TYPE_CPP_NO_UNIQUE_ADDRESS
#elif UTL_HAS_CPP_ATTRIBUTE(no_unique_address)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS no_unique_address
#  define __UTL_ATTRIBUTE_TYPE_CPP_NO_UNIQUE_ADDRESS
#elif UTL_HAS_GNU_ATTRIBUTE(__no_unique_address__)
#  define __UTL_ATTRIBUTE_NO_UNIQUE_ADDRESS __no_unique_address__
#  define __UTL_ATTRIBUTE_TYPE_GNU_NO_UNIQUE_ADDRESS
#endif /* UTL_HAS_CPP_ATTRIBUTE(msvc::no_unique_address) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::always_inline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE clang::always_inline
#  define __UTL_ATTRIBUTE_TYPE_CPP_ALWAYS_INLINE
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::always_inline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE gnu::always_inline
#  define __UTL_ATTRIBUTE_TYPE_CPP_ALWAYS_INLINE
#elif UTL_HAS_CPP_ATTRIBUTE(msvc::forceinline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE msvc::forceinline
#  define __UTL_ATTRIBUTE_TYPE_CPP_ALWAYS_INLINE
#elif UTL_HAS_GNU_ATTRIBUTE(__always_inline__)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE __always_inline__
#  define __UTL_ATTRIBUTE_TYPE_GNU_ALWAYS_INLINE
#elif UTL_HAS_MSVC_KEYWORD(__forceinline)
#  define __UTL_ATTRIBUTE_ALWAYS_INLINE __forceinline
#  define __UTL_ATTRIBUTE_TYPE_MSVC_EXT_ALWAYS_INLINE
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::always_inline) */

#if UTL_HAS_CPP_ATTRIBUTE(msvc::noinline)
#  define __UTL_ATTRIBUTE_NOINLINE msvc::noinline
#  define __UTL_ATTRIBUTE_TYPE_CPP_NOINLINE
#elif UTL_HAS_CPP_ATTRIBUTE(clang::noinline)
#  define __UTL_ATTRIBUTE_NOINLINE clang::noinline
#  define __UTL_ATTRIBUTE_TYPE_CPP_NOINLINE
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::noinline)
#  define __UTL_ATTRIBUTE_NOINLINE gnu::noinline
#  define __UTL_ATTRIBUTE_TYPE_CPP_NOINLINE
#elif UTL_HAS_GNU_ATTRIBUTE(__noinline__)
#  define __UTL_ATTRIBUTE_NOINLINE __noinline__
#  define __UTL_ATTRIBUTE_TYPE_GNU_NOINLINE
#elif UTL_HAS_DECLSPEC(noinline)
#  define __UTL_ATTRIBUTE_NOINLINE noinline
#  define __UTL_ATTRIBUTE_TYPE_DECLSPEC_NOINLINE
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::noinline) */

#if UTL_HAS_CPP_ATTRIBUTE(clang::malloc)
#  define __UTL_ATTRIBUTE_MALLOC clang::malloc
#  define __UTL_ATTRIBUTE_TYPE_CPP_MALLOC
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::malloc)
#  define __UTL_ATTRIBUTE_MALLOC gnu::malloc
#  define __UTL_ATTRIBUTE_TYPE_CPP_MALLOC
#elif UTL_HAS_GNU_ATTRIBUTE(__malloc__)
#  define __UTL_ATTRIBUTE_MALLOC __malloc__
#  define __UTL_ATTRIBUTE_TYPE_GNU_MALLOC
#elif UTL_HAS_DECLSPEC(allocator) | UTL_HAS_DECLSPEC(restrict)
#  if UTL_HAS_DECLSPEC(allocator) & UTL_HAS_DECLSPEC(restrict)
#    define __UTL_ATTRIBUTE_MALLOC allocator, restrict
#    define __UTL_ATTRIBUTE_TYPE_DECLSPEC_MALLOC
#  elif UTL_HAS_DECLSPEC(allocator)
#    define __UTL_ATTRIBUTE_MALLOC allocator
#    define __UTL_ATTRIBUTE_TYPE_DECLSPEC_MALLOC
#  elif UTL_HAS_DECLSPEC(restrict)
#    define __UTL_ATTRIBUTE_MALLOC restrict
#    define __UTL_ATTRIBUTE_TYPE_DECLSPEC_MALLOC
#  endif
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::malloc) */

#if UTL_COMPILER_GCC
#  if UTL_HAS_CPP_ATTRIBUTE(gnu::malloc)
#    define __UTL_ATTRIBUTE_DEALLOCATED_BY gnu::malloc
#    define __UTL_ATTRIBUTE_TYPE_CPP_DEALLOCATED_BY(FUNC, ...)
#  elif UTL_HAS_GNU_ATTRIBUTE(__malloc__)
#    define __UTL_ATTRIBUTE_DEALLOCATED_BY __malloc__
#    define __UTL_ATTRIBUTE_TYPE_GNU_DEALLOCATED_BY(FUNC, ...)
#  endif /* UTL_HAS_CPP_ATTRIBUTE(clang::malloc) */
#endif

#if UTL_HAS_CPP_ATTRIBUTE(gnu::visibility)
#  define __UTL_ATTRIBUTE_VISIBILITY gnu::visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_VISIBILITY(VISIBILITY)
#elif UTL_HAS_GNU_ATTRIBUTE(__visibility__)
#  define __UTL_ATTRIBUTE_VISIBILITY __visibility__
#  define __UTL_ATTRIBUTE_TYPE_GNU_VISIBILITY(VISIBILITY)
#endif

#if UTL_HAS_CPP_ATTRIBUTE(type_visibility) && UTL_CXX17
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY type_visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_TYPE_VISIBILITY(VISIBILITY)
#elif UTL_HAS_CPP_ATTRIBUTE(clang::type_visibility) && UTL_CXX17
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY clang::type_visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_TYPE_VISIBILITY(VISIBILITY)
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::type_visibility) && UTL_CXX17
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY gnu::type_visibility
#  define __UTL_ATTRIBUTE_TYPE_CPP_TYPE_VISIBILITY(VISIBILITY)
#elif UTL_HAS_GNU_ATTRIBUTE(__type_visibility__)
#  define __UTL_ATTRIBUTE_TYPE_VISIBILITY __type_visibility__
#  define __UTL_ATTRIBUTE_TYPE_GNU_TYPE_VISIBILITY(VISIBILITY)
#endif

#if UTL_HAS_CPP_ATTRIBUTE(gnu::dllimport)
#  define __UTL_ATTRIBUTE_DLLIMPORT gnu::dllimport
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLIMPORT
#elif UTL_HAS_GNU_ATTRIBUTE(__dllimport__)
#  define __UTL_ATTRIBUTE_DLLIMPORT __dllimport__
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLIMPORT
#elif UTL_HAS_DECLSPEC(dllimport)
#  define __UTL_ATTRIBUTE_DLLIMPORT dllimport
#  define __UTL_ATTRIBUTE_TYPE_DECLSPEC_DLLIMPORT
#endif

#if UTL_HAS_CPP_ATTRIBUTE(gnu::dllexport)
#  define __UTL_ATTRIBUTE_DLLEXPORT gnu::dllexport
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLEXPORT
#elif UTL_HAS_GNU_ATTRIBUTE(__dllexport__)
#  define __UTL_ATTRIBUTE_DLLEXPORT __dllexport__
#  define __UTL_ATTRIBUTE_TYPE_CPP_DLLEXPORT
#elif UTL_HAS_DECLSPEC(dllexport)
#  define __UTL_ATTRIBUTE_DLLIMPORT dllexport
#  define __UTL_ATTRIBUTE_TYPE_DECLSPEC_DLLIMPORT
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::exclude_from_explicit_instantiation)
#  define __UTL_ATTRIBUTE_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
      clang::exclude_from_explicit_instantiation
#  define __UTL_ATTRIBUTE_TYPE_CPP_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::exclude_from_explicit_instantiation)
#  define __UTL_ATTRIBUTE_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
      gnu::exclude_from_explicit_instantiation
#  define __UTL_ATTRIBUTE_TYPE_CPP_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#elif UTL_HAS_GNU_ATTRIBUTE(__exclude_from_explicit_instantiation__)
#  define __UTL_ATTRIBUTE_EXCLUDE_FROM_EXPLICIT_INSTANTIATION \
      __exclude_from_explicit_instantiation__
#  define __UTL_ATTRIBUTE_TYPE_GNU_EXCLUDE_FROM_EXPLICIT_INSTANTIATION
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::abi_tag)
#  define __UTL_ATTRIBUTE_ABI_TAG clang::abi_tag
#  define __UTL_ATTRIBUTE_TYPE_CPP_ABI_TAG(STR)
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::abi_tag)
#  define __UTL_ATTRIBUTE_ABI_TAG gnu::abi_tag
#  define __UTL_ATTRIBUTE_TYPE_CPP_ABI_TAG(STR)
#elif UTL_HAS_GNU_ATTRIBUTE(__abi_tag__)
#  define __UTL_ATTRIBUTE_ABI_TAG __abi_tag__
#  define __UTL_ATTRIBUTE_TYPE_GNU_ABI_TAG(STR)
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes)
#  define __UTL_ATTRIBUTE_REINITIALIZES clang::reinitializes
#  define __UTL_ATTRIBUTE_TYPE_CPP_REINITIALIZES
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::reinitializes)
#  define __UTL_ATTRIBUTE_REINITIALIZES gnu::reinitializes
#  define __UTL_ATTRIBUTE_TYPE_CPP_REINITIALIZES
#elif UTL_HAS_GNU_ATTRIBUTE(__reinitializes__)
#  define __UTL_ATTRIBUTE_REINITIALIZES __reinitializes__
#  define __UTL_ATTRIBUTE_TYPE_GNU_REINITIALIZES
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::reinitializes) */

#if UTL_HAS_CPP_ATTRIBUTE(maybe_unused) && UTL_CXX17
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED maybe_unused
#  define __UTL_ATTRIBUTE_TYPE_CPP_MAYBE_UNUSED
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::unused)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED gnu::unused
#  define __UTL_ATTRIBUTE_TYPE_CPP_MAYBE_UNUSED
#elif UTL_HAS_GNU_ATTRIBUTE(__unused__)
#  define __UTL_ATTRIBUTE_MAYBE_UNUSED __unused__
#  define __UTL_ATTRIBUTE_TYPE_GNU_MAYBE_UNUSED
#endif /* UTL_HAS_CPP_ATTRIBUTE(maybe_unused) */

#if UTL_HAS_CPP_ATTRIBUTE(fallthrough) && UTL_CXX17
#  define UTL_FALLTHROUGH [[fallthrough]]
#  define __UTL_ATTRIBUTE_FALLTHROUGH fallthrough
#  define __UTL_ATTRIBUTE_TYPE_CPP_FALLTHROUGH
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::fallthrough)
#  define UTL_FALLTHROUGH [[gnu::fallthrough]]
#  define __UTL_ATTRIBUTE_FALLTHROUGH gnu::fallthrough
#  define __UTL_ATTRIBUTE_TYPE_CPP_FALLTHROUGH
#elif UTL_HAS_GNU_ATTRIBUTE(__fallthrough__)
#  define UTL_FALLTHROUGH __attribute__((__fallthrough__))
#  define __UTL_ATTRIBUTE_FALLTHROUGH __fallthrough__
#  define __UTL_ATTRIBUTE_TYPE_GNU_FALLTHROUGH
#else
#  define UTL_FALLTHROUGH
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::noescape)
#  define __UTL_ATTRIBUTE_NOESCAPE clang::noescape
#  define __UTL_ATTRIBUTE_TYPE_CPP_NOESCAPE
#endif

#if UTL_HAS_GNU_ATTRIBUTE(__optimize__)
#  define __UTL_ATTRIBUTE_OPTIMIZE __optimize__
#  define __UTL_ATTRIBUTE_TYPE_GNU_OPTIMIZE(STR)
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::nodebug)
#  define UTL_NODEBUG [[clang::nodebug]]
#  define __UTL_ATTRIBUTE_NODEBUG clang::nodebug
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODEBUG
#elif UTL_HAS_CPP_ATTRIBUTE(gnu::nodebug)
#  define UTL_NODEBUG [[gnu::nodebug]]
#  define __UTL_ATTRIBUTE_NODEBUG gnu::nodebug
#  define __UTL_ATTRIBUTE_TYPE_CPP_NODEBUG
#else
#  define UTL_NODEBUG
#endif

#if UTL_HAS_CPP_ATTRIBUTE(likely) && UTL_CXX20
#  define UTL_LIKELY [[likely]]
#  define __UTL_ATTRIBUTE_LIKELY likely
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIKELY
#elif UTL_HAS_CPP_ATTRIBUTE(clang::likely)
#  define UTL_LIKELY [[clang::likely]]
#  define __UTL_ATTRIBUTE_LIKELY clang::likely
#  define __UTL_ATTRIBUTE_TYPE_CPP_LIKELY
#else
#  define UTL_LIKELY
#endif

#if UTL_HAS_CPP_ATTRIBUTE(unlikely) && UTL_CXX20
#  define UTL_UNLIKELY [[unlikely]]
#  define __UTL_ATTRIBUTE_UNLIKELY unlikely
#  define __UTL_ATTRIBUTE_TYPE_CPP_UNLIKELY
#elif UTL_HAS_CPP_ATTRIBUTE(clang::unlikely)
#  define UTL_UNLIKELY [[clang::unlikely]]
#  define __UTL_ATTRIBUTE_UNLIKELY clang::unlikely
#  define __UTL_ATTRIBUTE_TYPE_CPP_UNLIKELY
#else
#  define UTL_UNLIKELY
#endif

#if UTL_HAS_CPP_ATTRIBUTE(clang::vectorcall)
#  define __UTL_ATTRIBUTE_VECTORCALL clang::vectorcall
#  define __UTL_ATTRIBUTE_TYPE_CPP_VECTORCALL
#elif UTL_HAS_KEYWORD(__vectorcall)
#  define UTL_NODISCARD __vectorcall
#  define __UTL_ATTRIBUTE_VECTORCALL __vectorcall
#  define __UTL_ATTRIBUTE_TYPE_MSVC_EXT_VECTORCALL
#endif /* UTL_HAS_CPP_ATTRIBUTE(clang::vectorcall) */
