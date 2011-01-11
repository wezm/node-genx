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
#include <node_events.h>
#include <iostream>

#include "genx.h"
#include "node-genx.h"
#include "element.h"
#include "attribute.h"

using namespace v8;
using namespace node;
using namespace std;

static Persistent<String> sym_data; // TODO: I don't think this needs to be global
Persistent<FunctionTemplate> Element::constructor_template;

void Element::Initialize(Handle<Object> target)
{
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(New);

  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("Element"));
}

Element::Element(genxElement el) : element(el)
{
}

Element::~Element()
{
}

Handle<Value> Element::New(const Arguments& args)
{
  HandleScope scope;
  REQ_EXT_ARG(0, e);

  Element* el = new Element((genxElement)e->Value());
  el->Wrap(args.This());
  return args.This();
}

genxStatus Element::start()
{
  return genxStartElement(element);
}

class Writer: public EventEmitter
{
private:
  genxWriter writer;
  genxSender sender;
  Persistent<Object> stream;
public:
  static void Initialize(Handle<Object> target)
  {
    HandleScope scope;

    Local<FunctionTemplate> t = FunctionTemplate::New(New);
    t->Inherit(EventEmitter::constructor_template);
    t->InstanceTemplate()->SetInternalFieldCount(1);
    t->SetClassName(String::NewSymbol("Writer"));

    NODE_SET_PROTOTYPE_METHOD(t, "startDocument", StartDoc);
    NODE_SET_PROTOTYPE_METHOD(t, "endDocument", EndDocument);

    NODE_SET_PROTOTYPE_METHOD(t, "declareElement", DeclareElement);

    NODE_SET_PROTOTYPE_METHOD(t, "startElement", StartElement);
    NODE_SET_PROTOTYPE_METHOD(t, "startElementLiteral", StartElementLiteral);

    NODE_SET_PROTOTYPE_METHOD(t, "addText", AddText);
    NODE_SET_PROTOTYPE_METHOD(t, "addComment", AddComment);

    NODE_SET_PROTOTYPE_METHOD(t, "declareAttribute", DeclareAttribute);
    NODE_SET_PROTOTYPE_METHOD(t, "addAttribute", AddAttribute);
    NODE_SET_PROTOTYPE_METHOD(t, "addAttributeLiteral", AddAttributeLiteral);

    NODE_SET_PROTOTYPE_METHOD(t, "endElement", EndElement);

    target->Set(String::NewSymbol("Writer"), t->GetFunction());

    sym_data = NODE_PSYMBOL("data");
  }

  Writer()
  {
    // alloc, free, userData
    writer = genxNew(NULL, NULL, this);
    sender.send = sender_send;
    sender.sendBounded = sender_sendBounded;
    sender.flush = sender_flush;
    stream = Persistent<Object>::Persistent();
  }

  ~Writer()
  {
    if(!stream.IsEmpty()) stream.Dispose(); // or Clear?
    genxDispose(writer);
  }

protected:

  static Handle<Value> New(const Arguments& args)
  {
    HandleScope scope;
    Writer* writer = new Writer();
    writer->Wrap(args.This());
    return args.This();
  }

  static Handle<Value> StartDoc(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

    w->startDoc();
    return args.This();
  }

  genxStatus startDoc()
  {
    return genxStartDocSender(writer, &sender);
  }

  static Handle<Value> EndDocument(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

    w->endDocument();
    return args.This();
  }

  genxStatus endDocument()
  {
    return genxEndDocument(writer);
  }

  static Handle<Value> DeclareElement(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
    utf8 name = NULL;

    if (args.Length() <1 ||
        !args[0]->IsString()) {
      return ThrowException(Exception::Error(String::New(
        "First argument must be a String")));
    }

    Local<String> Text = args[0]->ToString();
    name = createUtf8FromString(Text);
    
    Handle<Value> elem = w->declareElement(name);
    delete[] name;
    
    return elem;
  }

  Handle<Value> declareElement(constUtf8 name)
  {
    HandleScope scope;
    genxStatus status = GENX_SUCCESS;
    genxNamespace ns = NULL;
    genxElement element = genxDeclareElement(writer, ns, name, &status);

    Local<Value> argv[1];
    argv[0] = External::New(element);
    Persistent<Object> e (Element::constructor_template->GetFunction()->NewInstance(1, argv));
    // return Undefined();
    return Persistent<Value>::New(e);
  }

  static Handle<Value> StartElement(const Arguments& args)
  {
    HandleScope scope;
    Element *e = ObjectWrap::Unwrap<Element>(args[0]->ToObject());

    genxStatus status = e->start(); //genxStartElement(elem);

    return args.This();
  }

  static Handle<Value> StartElementLiteral(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
    utf8 type = NULL;

    if (args.Length() <1 ||
        !args[0]->IsString()) {
      return ThrowException(Exception::Error(String::New(
        "First argument must be a String")));
    }

    Local<String> Type = args[0]->ToString();
    type = createUtf8FromString(Type);

    w->startElementLiteral(type);
    delete[] type;

    return args.This();
  }

  genxStatus startElementLiteral(constUtf8 type)
  {
    return genxStartElementLiteral(writer, NULL, type);
  }

  static Handle<Value> AddText(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
    utf8 text = NULL;
    genxStatus status;

    if (args.Length() <1 ||
        !args[0]->IsString()) {
      return ThrowException(Exception::Error(String::New(
        "First argument must be a String")));
    }

    Local<String> Text = args[0]->ToString();
    text = createUtf8FromString(Text);

    status = w->addText(text);
    delete[] text;

    return args.This();
  }

  genxStatus addText(constUtf8 text)
  {
    // TODO handle the return value from genx here?
    return genxAddText(writer, text);
  }

  static Handle<Value> AddComment(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
    utf8 text = NULL;
    genxStatus status;

    if (args.Length() <1 ||
        !args[0]->IsString()) {
      return ThrowException(Exception::Error(String::New(
        "First argument must be a String")));
    }

    Local<String> Text = args[0]->ToString();
    text = createUtf8FromString(Text);

    status = w->addComment(text);
    delete[] text;

    return args.This();
  }

  genxStatus addComment(constUtf8 comment)
  {
    // TODO handle the return value from genx here?
    return genxComment(writer, comment);
  }

  static Handle<Value> DeclareAttribute(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
    utf8 name = NULL;

    if (args.Length() <1 ||
        !args[0]->IsString()) {
      return ThrowException(Exception::Error(String::New(
        "First argument must be a String")));
    }

    Local<String> Text = args[0]->ToString();
    name = createUtf8FromString(Text);
    
    Handle<Value> attr = w->declareAttribute(name);
    delete[] name;
    
    return attr;
  }

  Handle<Value> declareAttribute(constUtf8 name)
  {
    HandleScope scope;
    genxStatus status = GENX_SUCCESS;
    genxNamespace ns = NULL;
    genxAttribute attribute = genxDeclareAttribute(writer, ns, name, &status);

    Local<Value> argv[1];
    argv[0] = External::New(attribute);
    Persistent<Object> a (Attribute::constructor_template->GetFunction()->NewInstance(1, argv));

    return Persistent<Value>::New(a);
  }

  static Handle<Value> AddAttribute(const Arguments& args)
  {
    HandleScope scope;
    utf8 value = NULL;

    if (args.Length() < 2    ||
        !args[0]->IsObject() ||
        !args[1]->IsString()) {
      return ThrowException(Exception::Error(String::New(
        "First argument must be a String")));
    }

    Attribute *attr = ObjectWrap::Unwrap<Attribute>(args[0]->ToObject());
    Local<String> Text = args[1]->ToString();
    value = createUtf8FromString(Text);

    genxStatus status = attr->add(value);
    delete[] value;

    return args.This();
  }


  static Handle<Value> AddAttributeLiteral(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());
    utf8 name = NULL;
    utf8 value = NULL;

    if (args.Length() < 2    ||
       !args[0]->IsString() ||
       !args[1]->IsString()) {
      return ThrowException(Exception::Error(String::New(
        "Must supply two string arguments")));
    }

    Local<String> Name = args[0]->ToString();
    Local<String> Value = args[1]->ToString();

    // Get the raw UTF-8 strings
    name = createUtf8FromString(Name);
    value = createUtf8FromString(Value);

    w->addAttributeLiteral(name, value);
    delete[] name;
    delete[] value;

    return args.This();
  }

  genxStatus addAttributeLiteral(constUtf8 name, constUtf8 value)
  {
    constUtf8 xmlns = NULL;
    return genxAddAttributeLiteral(writer, xmlns, name, value);
  }

  static Handle<Value> EndElement(const Arguments& args)
  {
    HandleScope scope;
    Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

    w->endElement();
    return args.This();
  }

  genxStatus endElement()
  {
    return genxEndElement(writer);
  }

private:
  static utf8 createUtf8FromString(Handle<String> String)
  {
    utf8 string = NULL;
    int length = String->Utf8Length() + 1;  // +1 for NUL character

    string = new unsigned char[length];
    String->WriteUtf8((char *)string, length);

    return string;
  }

  static genxStatus sender_send(void *userData, constUtf8 s)
  {
    HandleScope scope;
    Writer *w = reinterpret_cast<Writer *>(userData);

    // Deliver the data event
    Local<String> dataString = String::New((const char *)s);
    Handle<Value> argv[1] = { dataString };
    w->Emit(sym_data, 1, argv);

  	return GENX_SUCCESS;
  }

  static genxStatus sender_sendBounded(void *userData, constUtf8 start, constUtf8 end)
  {
    HandleScope scope;
    Writer *w = reinterpret_cast<Writer *>(userData);

    // Deliver the data event
    Local<String> dataString = String::New((const char *)start, end - start);
    Handle<Value> argv[1] = { dataString };
    w->Emit(sym_data, 1, argv);

  	return GENX_SUCCESS;
  }

  static genxStatus sender_flush(void * userData)
  {
  	return GENX_SUCCESS;
  }

};

extern "C" {
  static void init (Handle<Object> target)
  {
    Writer::Initialize(target);
    Element::Initialize(target);
    Attribute::Initialize(target);
  }

  NODE_MODULE(genx, init);
}
