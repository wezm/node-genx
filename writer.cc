#include "writer.h"
#include "element.h"
#include "attribute.h"

void Writer::Initialize(Handle<Object> target)
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

Writer::Writer()
{
	// alloc, free, userData
	writer = genxNew(NULL, NULL, this);
	sender.send = sender_send;
	sender.sendBounded = sender_sendBounded;
	sender.flush = sender_flush;
	stream = Persistent<Object>::Persistent();
}

Writer::~Writer()
{
	if(!stream.IsEmpty()) stream.Dispose(); // or Clear?
	genxDispose(writer);
}

Handle<Value> Writer::New(const Arguments& args)
{
	HandleScope scope;
	Writer* writer = new Writer();
	writer->Wrap(args.This());
	return args.This();
}

Handle<Value> Writer::StartDoc(const Arguments& args)
{
	HandleScope scope;
	Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

	w->startDoc();
	return args.This();
}

genxStatus Writer::startDoc()
{
	return genxStartDocSender(writer, &sender);
}

Handle<Value> Writer::EndDocument(const Arguments& args)
{
	HandleScope scope;
	Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

	w->endDocument();
	return args.This();
}

genxStatus Writer::endDocument()
{
	return genxEndDocument(writer);
}

Handle<Value> Writer::DeclareElement(const Arguments& args)
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

Handle<Value> Writer::declareElement(constUtf8 name)
{
	HandleScope scope;
	genxStatus status = GENX_SUCCESS;
	genxNamespace ns = NULL;
	genxElement element = genxDeclareElement(writer, ns, name, &status);

	Local<Value> argv[1];
	argv[0] = External::New(element);
	Persistent<Object> e (Element::constructor_template->GetFunction()->NewInstance(1, argv));

	return Persistent<Value>::New(e);
}

Handle<Value> Writer::StartElement(const Arguments& args)
{
	HandleScope scope;
	Element *e = ObjectWrap::Unwrap<Element>(args[0]->ToObject());

	genxStatus status = e->start(); //genxStartElement(elem);

	return args.This();
}

Handle<Value> Writer::StartElementLiteral(const Arguments& args)
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

genxStatus Writer::startElementLiteral(constUtf8 type)
{
	return genxStartElementLiteral(writer, NULL, type);
}

Handle<Value> Writer::AddText(const Arguments& args)
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

genxStatus Writer::addText(constUtf8 text)
{
	// TODO handle the return value from genx here?
	return genxAddText(writer, text);
}

Handle<Value> Writer::AddComment(const Arguments& args)
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

genxStatus Writer::addComment(constUtf8 comment)
{
	// TODO handle the return value from genx here?
	return genxComment(writer, comment);
}

Handle<Value> Writer::DeclareAttribute(const Arguments& args)
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

Handle<Value> Writer::declareAttribute(constUtf8 name)
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

Handle<Value> Writer::AddAttribute(const Arguments& args)
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


Handle<Value> Writer::AddAttributeLiteral(const Arguments& args)
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

genxStatus Writer::addAttributeLiteral(constUtf8 name, constUtf8 value)
{
	constUtf8 xmlns = NULL;
	return genxAddAttributeLiteral(writer, xmlns, name, value);
}

Handle<Value> Writer::EndElement(const Arguments& args)
{
	HandleScope scope;
	Writer* w = ObjectWrap::Unwrap<Writer>(args.This());

	w->endElement();
	return args.This();
}

genxStatus Writer::endElement()
{
	return genxEndElement(writer);
}

utf8 Writer::createUtf8FromString(Handle<String> String)
{
	utf8 string = NULL;
	int length = String->Utf8Length() + 1;  // +1 for NUL character

	string = new unsigned char[length];
	String->WriteUtf8((char *)string, length);

	return string;
}

genxStatus Writer::sender_send(void *userData, constUtf8 s)
{
	HandleScope scope;
	Writer *w = reinterpret_cast<Writer *>(userData);

	// Deliver the data event
	Local<String> dataString = String::New((const char *)s);
	Handle<Value> argv[1] = { dataString };
	w->Emit(sym_data, 1, argv);

	return GENX_SUCCESS;
}

genxStatus Writer::sender_sendBounded(void *userData, constUtf8 start, constUtf8 end)
{
	HandleScope scope;
	Writer *w = reinterpret_cast<Writer *>(userData);

	// Deliver the data event
	Local<String> dataString = String::New((const char *)start, end - start);
	Handle<Value> argv[1] = { dataString };
	w->Emit(sym_data, 1, argv);

	return GENX_SUCCESS;
}

genxStatus Writer::sender_flush(void * userData)
{
	return GENX_SUCCESS;
}

