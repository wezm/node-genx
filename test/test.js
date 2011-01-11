var genx = require('../build/default/genx');

var w = new genx.Writer();

w.on('data', function(data) {
  process.stdout.write(data);
})
.on('error', function(error) {
  console.log("error");
  // Handle the error...
});

w.startDocument()
.addComment(" Generated " + (new Date()).toString() + " ")
.startElementLiteral('genx')
  .addAttributeLiteral('version', '1.0')
  .startElementLiteral('test')
    .addAttributeLiteral('test', '<&"')
    .addText("Hello UTF-8 World: ⚡")
  .endElement()
.endElement()
.endDocument();
process.stdout.write("\n")