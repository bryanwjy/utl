// Copyright 2023-2024 Bryan Wong

#pragma once

#include "utl/filesystem/utl_filesystem_fwd.h"

#include "utl/type_traits/utl_constants.h"

__UFS_NAMESPACE_BEGIN

template <typename>
struct __UTL_PUBLIC_TEMPLATE is_file_like : false_type {};

template <typename Alloc>
struct __UTL_PUBLIC_TEMPLATE is_file_like<basic_file<Alloc>> : true_type {};

template <typename Alloc>
struct __UTL_PUBLIC_TEMPLATE is_file_like<basic_file_snapshot<Alloc>> : true_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_file_like<file_view_snapshot> : true_type {};

template <>
struct __UTL_PUBLIC_TEMPLATE is_file_like<file_view> : true_type {};

template <file_type T>
struct __UTL_PUBLIC_TEMPLATE is_file_like<explicit_file_view_snapshot<T>> : true_type {};

template <file_type T>
struct __UTL_PUBLIC_TEMPLATE is_file_like<explicit_file_view<T>> : true_type {};

template <typename Alloc, file_type T>
struct __UTL_PUBLIC_TEMPLATE is_file_like<basic_explicit_file<T, Alloc>> : true_type {};

template <typename Alloc, file_type T>
struct __UTL_PUBLIC_TEMPLATE is_file_like<basic_explicit_file_snapshot<T, Alloc>> : true_type {};

#if UTL_CXX14
template <typename>
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr bool is_file_like_v = false;
template <typename Alloc>
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr bool is_file_like_v<basic_file<Alloc>> = true;

template <typename Alloc>
__UTL_HIDE_FROM_ABI
    UTL_INLINE_CXX17 constexpr bool is_file_like_v<basic_file_snapshot<Alloc>> = true;

template <>
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr bool is_file_like_v<file_view_snapshot> = true;

template <>
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr bool is_file_like_v<file_view> = true;

template <file_type T>
__UTL_HIDE_FROM_ABI
    UTL_INLINE_CXX17 constexpr bool is_file_like_v<explicit_file_view_snapshot<T>> = true;

template <file_type T>
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr bool is_file_like_v<explicit_file_view<T>> = true;

template <typename Alloc, file_type T>
__UTL_HIDE_FROM_ABI
    UTL_INLINE_CXX17 constexpr bool is_file_like_v<basic_explicit_file<T, Alloc>> = true;

template <typename Alloc, file_type T>
__UTL_HIDE_FROM_ABI
    UTL_INLINE_CXX17 constexpr bool is_file_like_v<basic_explicit_file_snapshot<T, Alloc>> =
    true;
#endif

#if UTL_CXX20
template <typename T>
concept file_like = is_file_like_v<T>;
#endif

template <typename T>
struct __UTL_PUBLIC_TEMPLATE typeof_file {};

template <>
struct __UTL_PUBLIC_TEMPLATE typeof_file<path_view> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = file_type::unknown;
};

template <>
struct __UTL_PUBLIC_TEMPLATE typeof_file<zpath_view> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = file_type::unknown;
};

template <typename Alloc>
struct __UTL_PUBLIC_TEMPLATE typeof_file<basic_file<Alloc>> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = file_type::unknown;
};

template <typename Alloc>
struct __UTL_PUBLIC_TEMPLATE typeof_file<basic_file_snapshot<Alloc>> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = file_type::unknown;
};

template <>
struct __UTL_PUBLIC_TEMPLATE typeof_file<file_view_snapshot> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = file_type::unknown;
};

template <>
struct __UTL_PUBLIC_TEMPLATE typeof_file<file_view> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = file_type::unknown;
};

template <file_type T>
struct __UTL_PUBLIC_TEMPLATE typeof_file<explicit_file_view_snapshot<T>> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = T;
};

template <file_type T>
struct __UTL_PUBLIC_TEMPLATE typeof_file<explicit_file_view<T>> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = T;
};

template <typename Alloc, file_type T>
struct __UTL_PUBLIC_TEMPLATE typeof_file<basic_explicit_file<T, Alloc>> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = T;
};

template <typename Alloc, file_type T>
struct __UTL_PUBLIC_TEMPLATE typeof_file<basic_explicit_file_snapshot<T, Alloc>> {
    __UTL_HIDE_FROM_ABI static constexpr file_type value = T;
};

#if UTL_CXX14
template <typename T>
__UTL_HIDE_FROM_ABI UTL_INLINE_CXX17 constexpr file_type typeof_file_v = typeof_file<T>::value;

#  define __UFS_TYPEOF_FILE(...) __UFS typeof_file_v<__VA_ARGS__>

#else

#  define __UFS_TYPEOF_FILE(...) __UFS typeof_file<__VA_ARGS__>::value

#endif

__UFS_NAMESPACE_END
