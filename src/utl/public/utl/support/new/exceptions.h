/* Copyright 2023-2025 Bryan Wong */
#pragma once

#ifndef UTL_MODULES
#  include "utl/config.h"
#  include "utl/support/exceptions/runtime.h"
#endif // !UTL_MODULES

#if !UTL_ABI_MICROSOFT
UTL_NAMESPACE_BEGIN(std)
class __UTL_ABI_PUBLIC bad_alloc : public exception {
public:
#  if UTL_LIBCXX
    bad_alloc() noexcept;
#  else  // UTL_LIBCXX
    bad_alloc() noexcept = default;
#  endif // UTL_LIBCXX
    __UTL_HIDE_FROM_ABI bad_alloc(bad_alloc const&) noexcept = default;
    __UTL_HIDE_FROM_ABI bad_alloc& operator=(
        bad_alloc const&) noexcept = default;

    ~bad_alloc() noexcept override;
    char const* what() const noexcept override;
};

class __UTL_ABI_PUBLIC bad_array_new_length : public bad_alloc {
public:
#  if UTL_LIBCXX
    bad_array_new_length() noexcept;
#  else  // UTL_LIBCXX
    bad_array_new_length() noexcept = default;
#  endif // UTL_LIBCXX

    __UTL_HIDE_FROM_ABI bad_array_new_length(
        bad_array_new_length const&) noexcept = default;
    __UTL_HIDE_FROM_ABI bad_array_new_length& operator=(
        bad_array_new_length const&) noexcept = default;

    ~bad_array_new_length() noexcept override;
    char const* what() const noexcept override;
};
UTL_NAMESPACE_END(std)

#elif !UTL_WITH_EXCEPTIONS // !UTL_ABI_MICROSOFT

UTL_NAMESPACE_BEGIN(std)
class __UTL_ABI_PUBLIC bad_alloc : public exception {
public:
    bad_alloc() noexcept : exception("bad allocation") {}
    __UTL_HIDE_FROM_ABI bad_alloc(bad_alloc const&) noexcept = default;
    __UTL_HIDE_FROM_ABI bad_alloc& operator=(
        bad_alloc const&) noexcept = default;

private:
    friend class bad_array_new_length;

    bad_alloc(char const* const message) noexcept : exception(message) {}
};

class __UTL_ABI_PUBLIC bad_array_new_length : public bad_alloc {
public:
    bad_array_new_length() noexcept : bad_alloc("bad array new length") {}
    __UTL_HIDE_FROM_ABI bad_array_new_length(
        bad_array_new_length const&) noexcept = default;
    __UTL_HIDE_FROM_ABI bad_array_new_length& operator=(
        bad_array_new_length const&) noexcept = default;
};
UTL_NAMESPACE_END(std)

#endif // !UTL_ABI_MICROSOFT

UTL_PUBLIC_DEFAULT_NAMESPACE_BEGIN
using ::std::bad_alloc;
using ::std::bad_array_new_length;
UTL_PUBLIC_DEFAULT_NAMESPACE_END
