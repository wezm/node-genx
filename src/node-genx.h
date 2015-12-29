/*
Copyright (c) 2010, Orlando Vazquez <ovazquez@gmail.com>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef NODE_GENX_H
#define NODE_GENX_H

#include <node_version.h>

#if NODE_MAJOR_VERSION == 0 && NODE_MINOR_VERSION <= 10
#define GET_EXTERNAL(ISOLATE, VAL) v8::External::New(VAL)
#define HANDLE_TO_LOCAL(HANDLE_VAL, LOCAL_OBJ_NAME) v8::Local<v8::Object> LOCAL_OBJ_NAME(*HANDLE_VAL)
#else
#define GET_EXTERNAL(ISOLATE, VAL) v8::External::New(ISOLATE, VAL)
#define HANDLE_TO_LOCAL(HANDLE_VAL, LOCAL_OBJ_NAME) v8::Local<v8::Object> LOCAL_OBJ_NAME(HANDLE_VAL)
#endif

#define REQ_EXT_ARG(I, ARGS)                                            \
  if (ARGS.Length() <= (I) || ARGS[I]->IsUndefined() ||                 \
      !ARGS[I]->IsExternal()) {                                         \
    Nan::ThrowTypeError("Argument " #I " invalid");                     \
    return;                                                             \
  }
#endif
