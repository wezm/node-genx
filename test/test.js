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
.startElementLiteral('genx')
  .startElementLiteral('test')
    .addText("Hello & World")
  .endElement()
.endElement()
.endDocument();
