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

#include <iostream>

#include <node.h>
#include <nan.h>

#include "writer.h"
#include "namespace.h"

#define DEFAULT_NEWLINE "\n"
#define DEFAULT_SPACER  "\t"

using v8::Function;
using v8::FunctionTemplate;
using v8::Object;
using v8::Context;
using v8::HandleScope;
using v8::Local;
using v8::Isolate;
using v8::String;
using v8::Value;
using v8::External;
using v8::Exception;

using Nan::Persistent;
using Nan::ObjectWrap;

Persistent<Function> Writer::constructor;

void Writer::Initialize(Local<Object> exports)
{
  Nan::HandleScope scope;

  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Writer").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "startDocument", StartDoc);
  Nan::SetPrototypeMethod(tpl, "endDocument", EndDocument);

  Nan::SetPrototypeMethod(tpl, "declareNamespace", DeclareNamespace);

  Nan::SetPrototypeMethod(tpl, "declareElement", DeclareElement);
  Nan::SetPrototypeMethod(tpl, "startElement", StartElement);
  Nan::SetPrototypeMethod(tpl, "startElementLiteral", StartElementLiteral);

  Nan::SetPrototypeMethod(tpl, "addText", AddText);
  Nan::SetPrototypeMethod(tpl, "addComment", AddComment);

  Nan::SetPrototypeMethod(tpl, "declareAttribute", DeclareAttribute);
  Nan::SetPrototypeMethod(tpl, "addAttribute", AddAttribute);
  Nan::SetPrototypeMethod(tpl, "addAttributeLiteral", AddAttributeLiteral);

  Nan::SetPrototypeMethod(tpl, "endElement", EndElement);
  Nan::SetPrototypeMethod(tpl, "endElementInline", EndElementInline);

  Local<Context> context = Nan::GetCurrentContext();
  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(exports, Nan::New("Writer").ToLocalChecked(), tpl->GetFunction(context).ToLocalChecked());
}

Writer::Writer(const bool prettyPrint, constUtf8 newLine, constUtf8 spacer)
{
  // alloc, free, userData
  writer = genxNew(NULL, NULL, this, prettyPrint, newLine, spacer);
  sender.send = sender_send;
  sender.sendBounded = sender_sendBounded;
  sender.flush = sender_flush;
}

Writer::~Writer()
{
  genxDispose(writer);
}

void Writer::New(const Nan::FunctionCallbackInfo <Value> &args)
{
  bool prettyPrint  = false;
  utf8 newLine = (utf8) DEFAULT_NEWLINE;
  utf8 spacer  = (utf8) DEFAULT_SPACER;
  switch(args.Length()) {
    case 0: break;
    case 3:
      if(!args[2]->IsString()) {
        Nan::ThrowTypeError("Third argument to Writer's constructor must be a string");
        return;
      }
      spacer = createUtf8FromString(Nan::To<String>(args[2]).ToLocalChecked());
    case 2:
      if(!args[1]->IsString()) {
        Nan::ThrowTypeError("Second argument to Writer's constructor must be a string");
        return;
      }
      newLine = createUtf8FromString(Nan::To<String>(args[1]).ToLocalChecked());
    case 1:
      if(!args[0]->IsBoolean()) {
        Nan::ThrowTypeError("First argument to Writer's constructor must be a boolean");
        return;
      }
      prettyPrint = Nan::To<bool>(args[0]).FromJust();
      break;
    default:
      Nan::ThrowError("Invalid number of arguments given to Writer's constructor");
      return;
  }

  Writer* writer = new Writer(prettyPrint, newLine, spacer);
  writer->Wrap(args.This());
  args.GetReturnValue().Set(args.This());
}

void Writer::StartDoc(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  w->startDoc();
  args.GetReturnValue().Set(args.This());
}

genxStatus Writer::startDoc()
{
  return genxStartDocSender(writer, &sender);
}

void Writer::EndDocument(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  w->endDocument();
  args.GetReturnValue().Set(args.This());
}

genxStatus Writer::endDocument()
{
  return genxEndDocument(writer);
}

// uri, [prefix]
void Writer::DeclareNamespace(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  Local<String> Uri;
  Local<String> Prefix;

  utf8 uri = NULL;
  utf8 prefix = NULL;

  // Prefix is optional
  switch(args.Length()) {
    case 1:
      if (!args[0]->IsString()) {
        Nan::ThrowTypeError("First argument to declareNamespace must be a string");
        return;
      }
      Uri = Nan::To<String>(args[0]).ToLocalChecked();
      break;
    case 2:
      if (!args[0]->IsString()) {
        Nan::ThrowTypeError("First argument to declareNamespace must be a string");
        return;
      }
      if (!args[1]->IsString()) {
        Nan::ThrowTypeError("Second argument to declareNamespace must be a string");
        return;
      }
      Uri = Nan::To<String>(args[0]).ToLocalChecked();
      Prefix = Nan::To<String>(args[1]).ToLocalChecked();
      prefix = createUtf8FromString(Prefix);
      break;
    default:
      Nan::ThrowError("Wrong number of arguments to declareNamespace");
      return;
  }

  uri = createUtf8FromString(Uri);
  w->declareNamespace(args, uri, prefix);
  delete[] uri;
  if (prefix != NULL) delete[] prefix;
}

void Writer::declareNamespace(const Nan::FunctionCallbackInfo<Value> &args, constUtf8 uri, constUtf8 prefix)
{
  genxStatus status = GENX_SUCCESS;
  genxNamespace name_space = genxDeclareNamespace(writer, uri, prefix, &status);

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  Local<Value> argv[1] = { GET_EXTERNAL(args.GetIsolate(), name_space) };
  Local<Function> con  = Nan::New<Function>(Namespace::constructor);

  args.GetReturnValue().Set(Nan::NewInstance(con, 1, argv).ToLocalChecked());
}

// [namespace], elementName
void Writer::DeclareElement(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  Namespace *name_space = NULL;
  Local<String> Text;

  utf8 name = NULL;
  genxNamespace ns = NULL;

  // Namespace is optional
  switch(args.Length()) {
    case 1:
      if (!args[0]->IsString()) {
        Nan::ThrowTypeError("First argument must be a string");
        return;
      }
      Text = Nan::To<String>(args[0]).ToLocalChecked();
      break;
    case 2:
      if (!args[0]->IsObject()) {
        Nan::ThrowTypeError("First argument must a Namespace");
        return;
      }
      if (!args[1]->IsString()) {
        Nan::ThrowTypeError("Second argument must be a string");
        return;
      }
      name_space = ObjectWrap::Unwrap<Namespace>(Nan::To<v8::Object>(args[0]).ToLocalChecked());
      Text = Nan::To<String>(args[1]).ToLocalChecked();
      break;
    default:
      Nan::ThrowError("Wrong number of arguments to declareElement");
      return;
  }

  name = createUtf8FromString(Text);
  if (name_space != NULL) ns = name_space->getNamespace();

  w->declareElement(args, ns, name);
  delete[] name;
}

void Writer::declareElement(const Nan::FunctionCallbackInfo<Value> &args, genxNamespace ns, constUtf8 name)
{
  genxStatus status = GENX_SUCCESS;
  genxElement element = genxDeclareElement(writer, ns, name, &status);

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  Local<Value> argv[1] = { GET_EXTERNAL(args.GetIsolate(), element) };
  Local<Function> cons = Nan::New<Function>(Element::constructor);

  args.GetReturnValue().Set(Nan::NewInstance(cons, 1, argv).ToLocalChecked());
}

void Writer::StartElement(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

  if (args.Length() < 1) {
    Nan::ThrowError("Not enough arguments to startElement");
    return;
  }
  else if(!args[0]->IsObject()) {
    Nan::ThrowTypeError("Argument to startElement must be an Element");
    return;
  }

  Element *e = ObjectWrap::Unwrap<Element>(Nan::To<v8::Object>(args[0]).ToLocalChecked());

  w->startElement(args, e);
}

void Writer::startElement(const Nan::FunctionCallbackInfo<Value> &args, Element *elem)
{
  genxStatus status = elem->start();

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  args.GetReturnValue().Set(args.This());
}

// [namespace], type
void Writer::StartElementLiteral(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  Local<String> Namespace;
  Local<String> Type;

  utf8 type = NULL;
  utf8 name_space = NULL;

  // Namespace is optional
  switch(args.Length()) {
    case 1:
      if (!args[0]->IsString()) {
        Nan::ThrowTypeError("First argument must be a string");
        return;
      }
      Type = Nan::To<String>(args[0]).ToLocalChecked();
      break;
    case 2:
      if (!args[0]->IsString()) {
        Nan::ThrowTypeError("First argument must a string");
        return;
      }
      if (!args[1]->IsString()) {
        Nan::ThrowTypeError("Second argument must be a string");
        return;
      }
      Namespace = Nan::To<String>(args[0]).ToLocalChecked();
      name_space = createUtf8FromString(Namespace);
      Type = Nan::To<String>(args[1]).ToLocalChecked();
      break;
    default:
      Nan::ThrowError("Wrong number of arguments to startElementLiteral");
      return;
  }

  type = createUtf8FromString(Type);

  w->startElementLiteral(args, name_space, type);
  delete[] type;
}

void Writer::startElementLiteral(const Nan::FunctionCallbackInfo<Value> &args, constUtf8 ns, constUtf8 type)
{
  genxStatus status = genxStartElementLiteral(writer, ns, type);

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }
  args.GetReturnValue().Set(args.This());
}

void Writer::AddText(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  utf8 text = NULL;

  if (args.Length() < 1) {
    Nan::ThrowError("Not enough arguments to addText");
    return;
  }
  else if(!args[0]->IsString()) {
    Nan::ThrowTypeError("Argument to addText must be a string");
    return;
  }

  Local<String> Text = Nan::To<String>(args[0]).ToLocalChecked();
  text = createUtf8FromString(Text);

  w->addText(args, text);
  delete[] text;
}

void Writer::addText(const Nan::FunctionCallbackInfo<Value> &args, constUtf8 text)
{
  genxStatus status = genxAddText(writer, text);

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  args.GetReturnValue().Set(args.This());
}

void Writer::AddComment(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  utf8 text = NULL;

  if (args.Length() < 1) {
    Nan::ThrowError("Not enough arguments to addComment");
    return;
  }
  else if(!args[0]->IsString()) {
    Nan::ThrowTypeError("Argument to addComment must be a string");
    return;
  }

  Local<String> Text = Nan::To<String>(args[0]).ToLocalChecked();
  text = createUtf8FromString(Text);

  w->addComment(args, text);
  delete[] text;
}

void Writer::addComment(const Nan::FunctionCallbackInfo<Value> &args, constUtf8 comment)
{
  genxStatus status = genxComment(writer, comment);

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  args.GetReturnValue().Set(args.This());
}

// [namespace], name
void Writer::DeclareAttribute(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  Namespace *name_space = NULL;
  Local<String> Text;

  utf8 name = NULL;
  genxNamespace ns = NULL;

  // Namespace is optional
  switch(args.Length()) {
    case 1:
      if (!args[0]->IsString()) {
        Nan::ThrowTypeError("First argument must be a string");
        return;
      }
      Text = Nan::To<String>(args[0]).ToLocalChecked();
      break;
    case 2:
      if (!args[0]->IsObject()) {
        Nan::ThrowTypeError("First argument must a Namespace");
        return;
      }
      if (!args[1]->IsString()) {
        Nan::ThrowTypeError("Second argument must be a string");
        return;
      }
      name_space = ObjectWrap::Unwrap<Namespace>(Nan::To<v8::Object>(args[0]).ToLocalChecked());
      Text = Nan::To<String>(args[1]).ToLocalChecked();
      break;
    default:
      Nan::ThrowError("Wrong number of arguments to declareAttribute");
      return;
  }

  name = createUtf8FromString(Text);
  if (name_space != NULL) ns = name_space->getNamespace();

  w->declareAttribute(args, ns, name);
  delete[] name;
}

void Writer::declareAttribute(const Nan::FunctionCallbackInfo<Value> &args, genxNamespace ns, constUtf8 name)
{
  genxStatus status = GENX_SUCCESS;
  genxAttribute attribute = genxDeclareAttribute(writer, ns, name, &status);

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  Local<Value> argv[1] = {GET_EXTERNAL(args.GetIsolate(), attribute) };
  Local<Function> cons = Nan::New<Function>(Attribute::constructor);
  args.GetReturnValue().Set(Nan::NewInstance(cons, 1, argv).ToLocalChecked());
}

void Writer::AddAttribute(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  utf8 value = NULL;

  if (args.Length() < 2) {
    Nan::ThrowError("Wrong number of arguments to addAttribute");
    return;
  }
  else if(!args[0]->IsObject()) {
    Nan::ThrowTypeError("First argument to addAttribute must be an Attribute");
    return;
  }
  else if(!args[1]->IsString()) {
    Nan::ThrowTypeError("Second argument to addAttribute must be a string");
    return;
  }

  Attribute *attr = ObjectWrap::Unwrap<Attribute>(Nan::To<v8::Object>(args[0]).ToLocalChecked());
  Local<String> Text = Nan::To<String>(args[1]).ToLocalChecked();
  value = createUtf8FromString(Text);

  w->addAttribute(args, attr, value);
  delete[] value;
}

void Writer::addAttribute(const Nan::FunctionCallbackInfo<Value> &args, Attribute *attr, constUtf8 value)
{
  genxStatus status = attr->add(value);

  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  args.GetReturnValue().Set(args.This());
}

void Writer::AddAttributeLiteral(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  utf8 name = NULL;
  utf8 value = NULL;

  if (args.Length() < 2 || !args[0]->IsString() || !args[1]->IsString()) {
    Nan::ThrowError("Two string arguments must be supplied to addAttributeLiteral");
    return;
  }

  Local<String> Name = Nan::To<String>(args[0]).ToLocalChecked();
  Local<String> Val = Nan::To<String>(args[1]).ToLocalChecked();

  // Get the raw UTF-8 strings
  name = createUtf8FromString(Name);
  value = createUtf8FromString(Val);

  w->addAttributeLiteral(args, name, value);
  delete[] name;
  delete[] value;
}

void Writer::addAttributeLiteral(const Nan::FunctionCallbackInfo<Value> &args, constUtf8 name, constUtf8 value)
{
  constUtf8 xmlns = NULL;

  genxStatus status = genxAddAttributeLiteral(writer, xmlns, name, value);
  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  args.GetReturnValue().Set(args.This());
}

void Writer::EndElement(const Nan::FunctionCallbackInfo <Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

  w->endElement(args);
}

void Writer::endElement(const Nan::FunctionCallbackInfo<Value> &args)
{
  genxStatus status = genxEndElement(writer);
  if (status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }

  args.GetReturnValue().Set(args.This());
}

void Writer::EndElementInline(const Nan::FunctionCallbackInfo<Value> &args)
{
  Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
  w->endElementInline(args);
}

void Writer::endElementInline(const Nan::FunctionCallbackInfo<Value> &args)
{
  genxStatus status =  genxEndElementInline(writer);
  if(status != GENX_SUCCESS) {
    Nan::ThrowError(genxGetErrorMessage(writer, status));
    return;
  }
  args.GetReturnValue().Set(args.This());
}

utf8 Writer::createUtf8FromString(v8::Local<v8::String> String)
{
#if NODE_9_0_MODULE_VERSION < NODE_MODULE_VERSION
  Local<Context> context = Nan::GetCurrentContext();
  Isolate* isolate = context->GetIsolate();
  utf8 string = NULL;
  int length = String->Utf8Length(isolate) + 1;  // +1 for NUL character

  string = new unsigned char[length];
  String->WriteUtf8(isolate, (char *)string, length);

  return string;
#else
  utf8 string = NULL;
  int length = String->Utf8Length() + 1;  // +1 for NUL character

  string = new unsigned char[length];
  String->WriteUtf8((char *)string, length);

  return string;
#endif
}

void Writer::Emit(int argc, Local<Value>argv[])
{
  Local<Function> emit = Local<Function>::Cast(Nan::Get(handle(), Nan::New("emit").ToLocalChecked()).ToLocalChecked());
  Nan::Call(emit, handle(), argc, argv);
}

genxStatus Writer::sender_send(void *userData, constUtf8 s)
{
  Writer *w = reinterpret_cast<Writer *>(userData);

  // Deliver the data event
  Local<String> dataString = Nan::New((const char *)s).ToLocalChecked();
  Local<Value> argv[2] = { Nan::New("data").ToLocalChecked(), dataString };
  w->Emit(2, argv);

  return GENX_SUCCESS;
}

genxStatus Writer::sender_sendBounded(void *userData, constUtf8 start, constUtf8 end)
{
  Writer *w = reinterpret_cast<Writer *>(userData);

  // Deliver the data event
  Local<String> dataString = Nan::New((const char *)start, (int) (end - start)).ToLocalChecked();
  Local<Value> argv[2] = { Nan::New("data").ToLocalChecked(), dataString };
  w->Emit(2, argv);

  return GENX_SUCCESS;
}

genxStatus Writer::sender_flush(void * userData)
{
  return GENX_SUCCESS;
}

