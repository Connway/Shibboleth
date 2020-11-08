/*
* Copyright (c) 2020 Calvin Rose
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

/* This is an example janetconf.h file. This will be usually generated
 * by the build system. */

#ifndef JANETCONF_H
#define JANETCONF_H

#define JANET_VERSION_MAJOR 1
#define JANET_VERSION_MINOR 13
#define JANET_VERSION_PATCH 0
#define JANET_VERSION_EXTRA "-dev"
#define JANET_VERSION "1.13.0-dev"

/* #define JANET_BUILD "local" */

/* These settings all affect linking, so use cautiously. */
/* #define JANET_SINGLE_THREADED */
#define JANET_NO_DYNAMIC_MODULES
/* #define JANET_NO_NANBOX */
/* #define JANET_API __attribute__((visibility ("default"))) */

/* These settings should be specified before amalgamation is
* built. Any build with these set should be considered non-standard, and
* certain Janet libraries should be expected not to work. */
/* #define JANET_NO_DOCSTRINGS */
/* #define JANET_NO_SOURCEMAPS */
#define JANET_REDUCED_OS 
#define JANET_NO_PROCESSES 
#define JANET_NO_ASSEMBLER 
/* #define JANET_NO_PEG */
#define JANET_NO_NET 
/* #define JANET_NO_TYPED_ARRAY */
/* #define JANET_NO_INT_TYPES */
#define JANET_NO_REALPATH 
#define JANET_NO_SYMLINKS 
#define JANET_NO_UMASK 
#define JANET_NO_THREADS  // Disables registering the thread libary. Janet library can still be multithreaded.
#define JANET_NO_PARSER   // Disables registering the parser library. Janet library will still have parser functions.
#define JANET_NO_COMPILE  // Disables registering the compile library. Janet library will still have parser functions.
#define JANET_NO_OS       // Disables registering the os library.
#define JANET_NO_FIBER    // Disables registering the fiber library. Janet library will still have fiber functions.
#define JANET_NO_DEBUG    // Disables registering the debug library. Janet library will still have debug functions.
#define JANET_NO_MARSHAL  // Disables registering the marshal library. Janet library will still have marshalling functions.
#define JANET_NO_IO       // Disables registering the io library.

/* Other settings */
#ifdef _DEBUG
	#define JANET_DEBUG 
#endif

/* #define JANET_PRF */
/* #define JANET_NO_UTC_MKTIME */
/* #define JANET_OUT_OF_MEMORY do { printf("janet out of memory\n"); exit(1); } while (0) */
/* #define JANET_EXIT(msg) do { printf("C assert failed executing janet: %s\n", msg); exit(1); } while (0) */
/* #define JANET_TOP_LEVEL_SIGNAL(msg) call_my_function((msg), stderr) */
/* #define JANET_RECURSION_GUARD 1024 */
/* #define JANET_MAX_PROTO_DEPTH 200 */
/* #define JANET_MAX_MACRO_EXPAND 200 */
/* #define JANET_STACK_MAX 16384 */
/* #define JANET_OS_NAME my-custom-os */
/* #define JANET_ARCH_NAME pdp-8 */

/* Main client settings, does not affect library code */
/* #define JANET_SIMPLE_GETLINE */

#endif /* end of include guard: JANETCONF_H */
