var genx = require('../build/default/genx');

var w = new genx.Writer();

w.on('data', function(data) {
  process.stdout.write(data);
})
.on('error', function(error) {
  console.log("error");
  // Handle the error...
});

// Tests predeclared elements
var genxElem = w.declareElement("genx");
var testElem = w.declareElement("test");
var versionAttr = w.declareAttribute("version");
var testAttr = w.declareAttribute("test");

w.startDocument()
.addComment(" Declared XML generation test " + (new Date()).toString() + " ")
.startElement(genxElem)
  .addAttribute(versionAttr, '1.0')
  .startElement(testElem)
    .addAttribute(testAttr, '<&"')
    .addText("Hello UTF-8 World: ⚡")
  .endElement()
.endElement()
.endDocument();
process.stdout.write("\n\n")

// that a single writer can be used for multiple documents
w.startDocument()
.addComment(" Literal XML generation test " + (new Date()).toString() + " ")
.startElementLiteral('genx')
  .addAttributeLiteral('version', '1.0')
  .startElementLiteral('test')
    .addAttributeLiteral('test', '<&"')
    .addText("Hello UTF-8 World: ⚡")
  .endElement()
.endElement()
.endDocument();
process.stdout.write("\n")
