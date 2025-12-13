/**
 * Copyright 2010-2011 PathScale, Inc. All rights reserved.
 * Copyright 2025 Bryan Wong. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * stdexcept.h - provides a stub version of <stdexcept>, which defines enough
 * of the exceptions for the runtime to use.
 */
#pragma once

#include "utlrt/config.h"

UTL_NAMESPACE_BEGIN(std)

class __UTL_ABI_PUBLIC exception {
public:
    __UTL_ABI_PRIVATE exception() noexcept = default;
    __UTL_ABI_PRIVATE exception(exception const&) noexcept = default;
    __UTL_ABI_PRIVATE exception& operator=(exception const&) noexcept = default;
    virtual ~exception();
    virtual char const* what() const noexcept;
};

/**
 * Bad allocation exception.  Thrown by ::operator new() if it fails.
 */
class __UTL_ABI_PUBLIC bad_alloc : public exception {
public:
    __UTL_ABI_PRIVATE bad_alloc() noexcept = default;
    __UTL_ABI_PRIVATE bad_alloc(bad_alloc const&) noexcept = default;
    __UTL_ABI_PRIVATE bad_alloc& operator=(bad_alloc const&) noexcept = default;
    ~bad_alloc();
    virtual char const* what() const noexcept;
};

/**
 * Bad cast exception.  Thrown by the __cxa_bad_cast() helper function.
 */
class __UTL_ABI_PUBLIC bad_cast : public exception {
public:
    __UTL_ABI_PRIVATE bad_cast() noexcept = default;
    __UTL_ABI_PRIVATE bad_cast(bad_cast const&) noexcept = default;
    __UTL_ABI_PRIVATE bad_cast& operator=(bad_cast const&) noexcept = default;
    virtual ~bad_cast();
    virtual char const* what() const noexcept;
};

/**
 * Bad typeid exception.  Thrown by the __cxa_bad_typeid() helper function.
 */
class __UTL_ABI_PUBLIC bad_typeid : public exception {
public:
    __UTL_ABI_PRIVATE bad_typeid() noexcept = default;
    __UTL_ABI_PRIVATE bad_typeid(bad_typeid const&) noexcept = default;
    __UTL_ABI_PRIVATE bad_typeid& operator=(
        bad_typeid const&) noexcept = default;
    virtual ~bad_typeid();
    virtual char const* what() const noexcept;
};

class __UTL_ABI_PUBLIC bad_array_new_length : public bad_alloc {
public:
    __UTL_ABI_PRIVATE bad_array_new_length() noexcept = default;
    __UTL_ABI_PRIVATE bad_array_new_length(
        bad_array_new_length const&) noexcept = default;
    __UTL_ABI_PRIVATE bad_array_new_length& operator=(
        bad_array_new_length const&) noexcept = default;
    virtual ~bad_array_new_length();
    virtual char const* what() const noexcept;
};

UTL_NAMESPACE_END(std)
