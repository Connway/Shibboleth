#pragma once

#include <stdint.h> // For size_t

/**
 * @def MPACK_STDLIB
 *
 * Enables the use of C stdlib. This allows the library to use malloc
 * for debugging and in allocation helpers.
 */
#define MPACK_STDLIB 1

/**
 * @def MPACK_STDIO
 *
 * Enables the use of C stdio. This adds helpers for easily
 * reading/writing C files and makes debugging easier.
 */
#define MPACK_STDIO 1

/**
 * @def MPACK_MALLOC
 *
 * Defines the memory allocation function used by MPack. This is used by
 * helpers for automatically allocating data the correct size, and for
 * debugging functions. If this macro is undefined, the allocation helpers
 * will not be compiled.
 *
 * The default is @c malloc() if @ref MPACK_STDLIB is enabled.
 */
#define MPACK_MALLOC MPackInternalAlloc

/**
 * @def MPACK_FREE
 *
 * Defines the memory free function used by MPack. This is used by helpers
 * for automatically allocating data the correct size. If this macro is
 * undefined, the allocation helpers will not be compiled.
 *
 * The default is @c free() if @ref MPACK_MALLOC has not been customized and
 * @ref MPACK_STDLIB is enabled.
 */
#define MPACK_FREE MPackInternalFree

void* MPackInternalAlloc(size_t size);
void MPackInternalFree(void* data);
