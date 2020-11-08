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

#ifndef JANET_AMALG
#include "features.h"
#include <janet.h>
#endif

/* Run a string */
int janet_dobytes(JanetTable *env, const uint8_t *bytes, int32_t len, const char *sourcePath, Janet *out) {
    JanetParser parser;
    int errflags = 0, done = 0;
    int32_t index = 0;
    Janet ret = janet_wrap_nil();
    const uint8_t *where = sourcePath ? janet_cstring(sourcePath) : NULL;

    if (where) janet_gcroot(janet_wrap_string(where));
    if (NULL == sourcePath) sourcePath = "<unknown>";
    janet_parser_init(&parser);

    /* While we haven't seen an error */
    while (!done) {

        /* Evaluate parsed values */
        while (janet_parser_has_more(&parser)) {
            Janet form = janet_parser_produce(&parser);
            JanetCompileResult cres = janet_compile(form, env, where);
            if (cres.status == JANET_COMPILE_OK) {
                JanetFunction *f = janet_thunk(cres.funcdef);
                JanetFiber *fiber = janet_fiber(f, 64, 0, NULL);
                fiber->env = env;
                JanetSignal status = janet_continue(fiber, janet_wrap_nil(), &ret);
                if (status != JANET_SIGNAL_OK && status != JANET_SIGNAL_EVENT) {
                    janet_stacktrace(fiber, ret);
                    errflags |= 0x01;
                    done = 1;
                }
            } else {
                ret = janet_wrap_string(cres.error);
                if (cres.macrofiber) {
                    janet_eprintf("compile error in %s: ", sourcePath);
                    janet_stacktrace(cres.macrofiber, ret);
                } else {
                    janet_eprintf("compile error in %s: %s\n", sourcePath,
                                  (const char *)cres.error);
                }
                errflags |= 0x02;
                done = 1;
            }
        }

        if (done) break;

        /* Dispatch based on parse state */
        switch (janet_parser_status(&parser)) {
            case JANET_PARSE_DEAD:
                done = 1;
                break;
            case JANET_PARSE_ERROR: {
                const char *e = janet_parser_error(&parser);
                errflags |= 0x04;
                ret = janet_cstringv(e);
                janet_eprintf("parse error in %s: %s\n", sourcePath, e);
                done = 1;
                break;
            }
            case JANET_PARSE_ROOT:
            case JANET_PARSE_PENDING:
                if (index >= len) {
                    janet_parser_eof(&parser);
                } else {
                    janet_parser_consume(&parser, bytes[index++]);
                }
                break;
        }

    }

    /* Clean up and return errors */
    janet_parser_deinit(&parser);
    if (where) janet_gcunroot(janet_wrap_string(where));
    if (out) *out = ret;
    return errflags;
}

int janet_dostring(JanetTable *env, const char *str, const char *sourcePath, Janet *out) {
    int32_t len = 0;
    while (str[len]) ++len;
    return janet_dobytes(env, (const uint8_t *)str, len, sourcePath, out);
}

