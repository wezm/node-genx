#include <element.h>

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


