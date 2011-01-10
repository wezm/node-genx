var genx = require('../build/default/genx');

var w = new genx.Writer();

w.startDocument();
w.startElementLiteral('genx');
w.startElementLiteral('test');
w.endElement();
w.endElement();
w.endDocument();
