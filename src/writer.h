/*
Copyright (c) 2011, Wesley Moore http://www.wezm.net/
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the node-genx Project, Wesley Moore, nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef NODE_GENX_WRITER_H
#define NODE_GENX_WRITER_H

#include <node.h>
#include <nan.h>

#include "genx.h"
#include "node-genx.h"
#include "attribute.h"
#include "element.h"

class Writer: public Nan::ObjectWrap
{
private:
  genxWriter writer;
  genxSender sender;

  static Nan::Persistent<v8::Function> constructor;
public:
  static void Initialize(v8::Local<v8::Object> target);

  Writer(const bool prettyPrint, constUtf8 newLine, constUtf8 spacer);

  ~Writer();

protected:

  static void New(const Nan::FunctionCallbackInfo <v8::Value> &args);

  static void StartDoc(const Nan::FunctionCallbackInfo <v8::Value> &args);
  genxStatus startDoc();

  static void EndDocument(const Nan::FunctionCallbackInfo <v8::Value> &args);
  genxStatus endDocument();

  static void DeclareNamespace(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void declareNamespace(const Nan::FunctionCallbackInfo<v8::Value> &args, constUtf8 ns, constUtf8 name);

  static void DeclareElement(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void declareElement(const Nan::FunctionCallbackInfo<v8::Value> &args, genxNamespace ns, constUtf8 name);

  static void StartElement(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void startElement(const Nan::FunctionCallbackInfo<v8::Value> &args, Element *elem);

  static void StartElementLiteral(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void startElementLiteral(const Nan::FunctionCallbackInfo<v8::Value> &args, constUtf8 ns, constUtf8 type);

  static void AddText(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void addText(const Nan::FunctionCallbackInfo<v8::Value> &args, constUtf8 text);

  static void AddComment(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void addComment(const Nan::FunctionCallbackInfo<v8::Value> &args, constUtf8 comment);

  static void DeclareAttribute(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void declareAttribute(const Nan::FunctionCallbackInfo<v8::Value> &args, genxNamespace ns, constUtf8 name);

  static void AddAttribute(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void addAttribute(const Nan::FunctionCallbackInfo<v8::Value> &args, Attribute *attr, constUtf8 value);

  static void AddAttributeLiteral(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void addAttributeLiteral(const Nan::FunctionCallbackInfo<v8::Value> &args, constUtf8 name, constUtf8 value);

  static void EndElement(const Nan::FunctionCallbackInfo <v8::Value> &args);
  void endElement(const Nan::FunctionCallbackInfo<v8::Value> &args);

  static void EndElementInline(const Nan::FunctionCallbackInfo<v8::Value> &args);
  void endElementInline(const Nan::FunctionCallbackInfo<v8::Value> &args);

private:
  static utf8 createUtf8FromString(v8::Local<v8::String> String);

  void Emit(int argc, v8::Local<v8::Value>argv[]);
  static genxStatus sender_send(void *userData, constUtf8 s);
  static genxStatus sender_sendBounded(void *userData, constUtf8 start, constUtf8 end);
  static genxStatus sender_flush(void * userData);
};

#endif

