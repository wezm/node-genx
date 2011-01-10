node-genx
=========

[node.js][node] bindings to the [GenX][genx] XML generation library. It is
currently a work in progress. When ready it will be released to [npm][npm].

[node]: http://nodejs.org/
[genx]: http://www.tbray.org/ongoing/When/200x/2004/02/20/GenxStatus
[npm]: http://npmjs.org/

Building
--------

    node-waf configure
    node-waf build

Usage
-----

    var genx = require('genx');

    var w = new genx.Writer();

    w.on('data', function(data) {
      process.stdout.write(data);
    })
    .on('error', function(error) {
      // Handle the error...
    });

    w.startDocument()
    .startElementLiteral('genx')
      .startElementLiteral('test')
      .endElement()
    .endElement()
    .endDocument();
