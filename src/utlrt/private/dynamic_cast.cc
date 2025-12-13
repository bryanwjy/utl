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

#include "typeinfo.h"

#include <stdio.h>

using namespace __cxxabiv1;

/**
 * Vtable header.
 */
struct vtable_header {
    /** Offset of the leaf object. */
    ptrdiff_t leaf_offset;
    /** Type of the object. */
    __class_type_info const* type;
};

/**
 * Simple macro that does pointer arithmetic in bytes but returns a value of
 * the same type as the original.
 */
#define ADD_TO_PTR(x, off) \
    reinterpret_cast<decltype(x)>(reinterpret_cast<char*>(x) + off)

/**
 * ABI function used to implement the dynamic_cast<> operator.  Some cases of
 * this operator are implemented entirely in the compiler (e.g. to void*).
 * This function implements the dynamic casts of the form dynamic_cast<T>(v).
 * This will be translated to a call to this function with the value v as the
 * first argument.  The type id of the static type of v is the second argument
 * and the type id of the destination type (T) is the third argument.
 *
 * The third argument is a hint about the compiler's guess at the correct
 * pointer offset.  If this value is negative, then -1 indicates no hint, -2
 * that src is not a public base of dst, and -3 that src is a multiple public
 * base type but never a virtual base type
 */
extern "C" __UTL_ABI_PUBLIC void* __dynamic_cast(void const* sub,
    __class_type_info const* src, __class_type_info const* dst,
    ptrdiff_t src2dst_offset) {
    char const* vtable_location = *static_cast<char const* const*>(sub);
    vtable_header const* header = reinterpret_cast<vtable_header const*>(
        vtable_location - sizeof(vtable_header));
    void* leaf = ADD_TO_PTR(const_cast<void*>(sub), header->leaf_offset);
    return header->type->cast_to(leaf, dst);
}
