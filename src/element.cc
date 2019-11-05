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

#include <node.h>
#include <nan.h>

#include "element.h"

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

Nan::Persistent<Function> Element::constructor;

void Element::Initialize(Local <Object> exports)
{
  Nan::HandleScope scope;

  Local <FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);

  tpl->SetClassName(Nan::New("Element").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Local<Context> context = Nan::GetCurrentContext();
  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(exports, Nan::New("Element").ToLocalChecked(), tpl->GetFunction(context).ToLocalChecked());
}

Element::Element(genxElement el) : element(el)
{
}

Element::~Element()
{
}

void Element::New(const Nan::FunctionCallbackInfo <Value> &args)
{
  REQ_EXT_ARG(0, args);

  Element* el = new Element((genxElement) args[0].As<External>()->Value());
  el->Wrap(args.This());
  args.GetReturnValue().Set(args.This());
}

genxStatus Element::start()
{
  return genxStartElement(element);
}


