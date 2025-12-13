/* Copyright 2023-2025 Bryan Wong */

#pragma once

/**
 * Concatenates the last token of the variadic argument with the first argument
 *
 * All of the arguments to a C preprocessor macro are fully expanded before the macro itself is
 * expanded, unless the # or ## operator is applied to them; then they're not expanded. So to get
 * full expansion before ##, pass the arguments through a wrapper macro that doesn't use ##.
 */
#define UTL_SWAP_CONCAT(Y, ...) UTL_PRIMITIVE_SWAP_CONCAT(Y, __VA_ARGS__)
/**
 * Internal implementation of UTL_SWAP_CONCAT @see UTL_SWAP_CONCAT
 */
#define UTL_PRIMITIVE_SWAP_CONCAT(Y, ...) __VA_ARGS__##Y

/**
 * Concatenates the first argument with the first token of the variadic argument
 */
#define UTL_CONCAT(_1, ...) UTL_PRIMITIVE_CONCAT(_1, __VA_ARGS__)
/**
 * Internal implementation of UTL_CONCAT @see UTL_CONCAT
 */
#define UTL_PRIMITIVE_CONCAT(_1, ...) _1##__VA_ARGS__
