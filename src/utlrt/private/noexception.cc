/**
 * Copyright 2021 Microsoft. All rights reserved.
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
#include "atomic.h"
#include "utlrt/config.h"

namespace std {

/**
 * Returns whether there are any exceptions currently being thrown that
 * have not been caught. Without exception support this is always false.
 */
__UTL_ABI_PUBLIC bool uncaught_exception() noexcept {
    return false;
}
/**
 * Returns the number of exceptions currently being thrown that have not
 * been caught. Without exception support this is always 0.
 */
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) int uncaught_exceptions() noexcept {
    return 0;
}

using terminate_handler = void (*)();

static atomic<terminate_handler> terminateHandler = abort;

UTL_ATTRIBUTES(_ABI_PUBLIC, NORETURN) void terminate() noexcept {
    terminateHandler.load()();
    // Should not be reached - a terminate handler is not expected
    // to return.
    abort();
}

/**
 * Sets the function that is called to terminate the program.
 */
__UTL_ABI_PUBLIC terminate_handler set_terminate(terminate_handler f) noexcept {
    return terminateHandler.exchange(f);
}

/**
 * Returns the current terminate handler.
 */
UTL_ATTRIBUTES(_ABI_PUBLIC, NODISCARD) terminate_handler get_terminate() noexcept {
    return terminateHandler.load();
}

} // namespace std
