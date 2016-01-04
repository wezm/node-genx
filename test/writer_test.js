var genx = require('../lib/genx');
//noinspection JSUnresolvedFunction
describe('genx', function(){

  describe('Writer', function(){
    var w = null;

    beforeEach(function() {
      w = new genx.Writer();
    });

    describe('declareNamespace', function() {
      it('raises an exception if there is the wrong number of arguments', function() {
        (function() {
          w.declareNamespace();
        }).should.throw('Wrong number of arguments to declareNamespace');

        (function() {
          w.declareNamespace(true, true, true);
        }).should.throw('Wrong number of arguments to declareNamespace');
      });

      describe('with a prefix', function() {
        it('raises an exception if the uri is not a string', function() {
          (function() {
            w.declareNamespace(1, 2);
          }).should.throw('First argument to declareNamespace must be a string');
        });

        it('raises an exception if the prefix is not a string', function() {
          (function() {
            w.declareNamespace('http://example.com/', 1);
          }).should.throw('Second argument to declareNamespace must be a string');
        });

        it('raises an exception if the prefix is not valid', function() {
          (function() {
            w.declareNamespace('http://example.com/', 'invalid/prefix');
          }).should.throw('Bad NAME');
        });

        it("returns a Namespace object", function() {
          w.declareNamespace('http://example.com/', 'test').should.be.ok;
        });
      });

      describe('without a prefix', function() {
        it("returns a Namespace object", function() {
          w.declareNamespace('http://example.com/').should.be.ok;
        })
      })
    });

    describe('declareElement', function() {
      it('raises an exception if there is the wrong number of arguments', function() {
        (function() {
          w.declareElement();
        }).should.throw('Wrong number of arguments to declareElement');

        (function() {
          w.declareElement(true, true, true);
        }).should.throw('Wrong number of arguments to declareElement');
      });

      it('raises an exception if the element name is invalid', function() {
        w.startDocument();
        (function() {
          w.declareElement('test>');
        }).should.throw('Bad NAME');
      });

      describe('without a namespace', function() {
        it('raises an exception if it isn\'t a string', function() {
          (function() {
            w.declareElement(1);
          }).should.throw('First argument must be a string');
        });

        it('returns an Element object', function() {
          w.declareElement('test').should.be.ok
        });
      });

      describe('with a namespace', function() {
        var ns = null;

        beforeEach(function() {
          ns = w.declareNamespace('test');
        });

        it('returns an Element object', function() {
          w.declareElement(ns, 'test').should.be.ok
        });
      });
    });

    describe('declareAttribute', function() {
      it('raises an exception if there is the wrong number of arguments', function() {
        (function() {
          w.declareAttribute();
        }).should.throw('Wrong number of arguments to declareAttribute');

        (function() {
          w.declareAttribute(true, true, true);
        }).should.throw('Wrong number of arguments to declareAttribute');
      });

      it('raises an exception if the attribute name is invalid', function() {
        (function() {
          w.declareElement('test=');
        }).should.throw('Bad NAME');
      });

      describe('without a namespace', function() {
        it('raises an exception if it isn\'t a string', function() {
          (function() {
            w.declareAttribute(1);
          }).should.throw('First argument must be a string');
        });

        it('returns an Attribute object', function() {
          w.declareAttribute('test').should.be.ok
        });
      });

      describe('with a namespace', function() {
        var ns = null;

        beforeEach(function() {
          ns = w.declareNamespace('test');
        });

        it('returns an Attribute object', function() {
          w.declareAttribute(ns, 'attr').should.be.ok
        });
      });
    });

    describe('startElementLiteral', function() {
      beforeEach(function() {
        w.startDocument();
      });

      it('raises an exception if there is the wrong number of arguments', function() {
        (function() {
          w.startElementLiteral();
        }).should.throw('Wrong number of arguments to startElementLiteral');

        (function() {
          w.startElementLiteral(true, true, true);
        }).should.throw('Wrong number of arguments to startElementLiteral');
      });

      it('raises an exception if the element name is invalid', function() {
        (function() {
          w.startElementLiteral('test>');
        }).should.throw('Bad NAME');
      });
    });


    describe('addAttributeLiteral', function() {
      beforeEach(function() {
        w.startDocument().startElementLiteral('test');
      });

      it('raises an exception if there is the wrong number of arguments', function() {
        (function() {
          w.addAttributeLiteral();
        }).should.throw('Two string arguments must be supplied to addAttributeLiteral');

        (function() {
          w.addAttributeLiteral(true, true, true);
        }).should.throw('Two string arguments must be supplied to addAttributeLiteral');
      });

      it('raises an exception if the attribute name is invalid', function() {
        (function() {
          w.addAttributeLiteral('test=', 'value');
        }).should.throw('Bad NAME');
      });
    });

    describe('startElement', function() {
      var elem = null;

      beforeEach(function() {
        elem = w.declareElement('test');
      });

      it('raises an exception if there is the wrong number of arguments', function() {
        w.startDocument();
        (function() {
          w.startElement();
        }).should.throw('Not enough arguments to startElement');
      });

      it('raises an exception if the first argument is not an Element', function() {
        w.startDocument();
        (function() {
          w.startElement('elem');
        }).should.throw('Argument to startElement must be an Element');
      });

      it('handles errors (by raising them) from genx', function() {
        (function() {
          w.startElement(elem);
        }).should.throw('Call out of sequence');
      });
    });

    describe('endElementInline', function() {
      beforeEach(function() {
        w.startDocument();
      });

      it('can generate a self-closing tag', function() {
        w.startElementLiteral('elem')
         .endElementInline();
      });

      it('raises an exception if no element has been started', function() {
        (function() {
          w.endElementInline();
        }).should.throw('Call out of sequence');
      });

      it('raises an exception if an element has children', function() {
        (function() {
          w.startElementLiteral('elem')
           .startElementLiteral('elem2')
           .endElement()
           .endElementInline();
        }).should.throw('Call out of sequence');
      });
    });

    describe('addAttribute', function() {
      var attr = null;

      beforeEach(function() {
        attr = w.declareAttribute('attr');
        w.startDocument();
      });

      it('raises an exception if there is the wrong number of arguments', function() {
        w.startElementLiteral('test');
        (function() {
          w.addAttribute();
        }).should.throw('Wrong number of arguments to addAttribute');

        (function() {
          w.addAttribute(attr);
        }).should.throw('Wrong number of arguments to addAttribute');
      });

      it('raises an exception if the first argument is not an Attribute', function() {
        (function() {
          w.addAttribute(true, 'value');
        }).should.throw('First argument to addAttribute must be an Attribute');
      });

      it('raises an exception if the second argument is not a string', function() {
        (function() {
          w.addAttribute(attr, true);
        }).should.throw('Second argument to addAttribute must be a string');
      });

      it('raises a sequence error if called in the wrong sequence', function() {
        w.startElementLiteral('test').addText('text');

        (function() {
          w.addAttribute(attr, 'value');
        }).should.throw('Call out of sequence');
      });
    });

    describe('addText', function() {
      beforeEach(function() {
        w.startDocument().startElementLiteral('test');
      });

      it('raises an exception if there is the wrong number of arguments', function() {
        (function() {
          w.addText();
        }).should.throw('Not enough arguments to addText');
      });

      it('raises an exception if the argument is not a string', function() {
        (function() {
          w.addText(true);
        }).should.throw('Argument to addText must be a string');
      });
    });

    describe('addComment', function() {
      beforeEach(function() {
        w.startDocument();
      });

      it('raises an exception if there is the wrong number of arguments', function() {
        (function() {
          w.addComment();
        }).should.throw('Not enough arguments to addComment');
      });

      it('raises an exception if the argument is not a string', function() {
        (function() {
          w.addComment(true);
        }).should.throw('Argument to addComment must be a string');
      });
    });

    describe('generating a document', function() {
      describe('using literal nodes', function() {
        it('generates the correct XML', function() {
          var result = '';

          w.on('data', function(data) {
            result += data;
          });

          w.startDocument()
            .addComment(' Testing ')
            .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
              .startElementLiteral('title').addAttributeLiteral('type', 'text')
                .addText('Testing').endElement().endElement().endDocument();
          result.should.equal("<!-- Testing -->\n<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\"><title type=\"text\">Testing</title></g1:feed>");
        });
      });

      describe('using pre-declared nodes', function() {
        it('generates the correct XML', function() {
          var feed, ns, result, title, type;
          result = '';
          w.on('data', function(data) {
            result += data;
          });
          ns = w.declareNamespace('http://www.w3.org/2005/Atom', '');
          feed = w.declareElement(ns, 'feed');
          title = w.declareElement(ns, 'title');
          type = w.declareAttribute('type');
          w.startDocument()
            .startElement(feed)
              .startElement(title).addAttribute(type, 'text')
                .addText('Testing')
              .endElement()
            .endElement()
          .endDocument();

          result.should.equal("<feed xmlns=\"http://www.w3.org/2005/Atom\"><title type=\"text\">Testing</title></feed>");
        });
      });

      describe('using a mix of literal and declared nodes', function() {
        it('generates the correct XML', function() {
          var feed, ns, result, type, uri;
          result = '';

          w.on('data', function(data) {
            result += data;
          });

          uri = 'http://www.w3.org/2005/Atom';
          ns = w.declareNamespace(uri, '');
          feed = w.declareElement(ns, 'feed');
          type = w.declareAttribute('type');

          w.startDocument()
            .startElement(feed)
              .startElementLiteral(uri, 'title').addAttribute(type, 'text')
                .addText('Testing')
              .endElement()
            .endElement()
          .endDocument();

         result.should.equal("<feed xmlns=\"http://www.w3.org/2005/Atom\"><title type=\"text\">Testing</title></feed>");
        });
      });
    });

    describe('generating a prettyprinted document', function() {
      beforeEach(function() {
        w = new genx.Writer(true);
      });

      describe('using literal nodes', function(){
        it('generates the correct XML', function() {
          var result = '';

          w.on('data', function(data) {
            result += data;
          });

          w.startDocument()
           .addComment(' Testing ')
           .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
           .startElementLiteral('title').addAttributeLiteral('type', 'text')
           .addText('Testing').endElement().endElement().endDocument();
          result.should.equal("<!-- Testing -->\n<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\n\t" +
                              "<title type=\"text\">Testing</title>\n</g1:feed>");
        });

        it('correctly spaces trailing text nodes', function() {
          var result = '';

          w.on('data', function(data) {
            result += data;
          });

          w.startDocument()
           .addComment(' Testing ')
           .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
           .startElementLiteral('title').addAttributeLiteral('type', 'text')
           .addText('Testing')
           .endElement()
           .addText('Trailing Text')
           .endElement()
           .endDocument();
          result.should.equal("<!-- Testing -->\n<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\n\t" +
                              "<title type=\"text\">Testing</title>\n\tTrailing Text\n</g1:feed>");
        });

        it('correctly spaces comments', function() {
          var result = '';

          w.on('data', function(data) {
            result += data;
          });

          w.startDocument()
            .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
              .startElementLiteral('title').addAttributeLiteral('type', 'text')
                .addText('Testing')
              .endElement()
              .addComment(' Testing ')
              .addText('Trailing Text')
              .startElementLiteral('comment-only')
                .addComment(' Testing ')
              .endElement()
            .endElement()
            .endDocument();
          result.should.equal("<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\n\t" +
                              "<title type=\"text\">Testing</title>\n\t<!-- Testing -->\n\t" +
                              "Trailing Text\n\t<comment-only><!-- Testing --></comment-only>\n</g1:feed>");
        });

        it('correctly spaces self-closing tags', function() {
          var result = '';

          w.on('data', function(data) {
            result += data;
          });

          w.startDocument()
           .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
             .startElementLiteral('title').addAttributeLiteral('type', 'text')
             .endElementInline()
             .startElementLiteral('inline-element')
             .endElementInline()
             .startElementLiteral('next-element')
               .addText('More Text')
             .endElement()
           .endElement()
           .endDocument();
          result.should.equal("<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\n\t" +
                              "<title type=\"text\" />\n\t<inline-element />\n\t" +
                              "<next-element>More Text</next-element>\n</g1:feed>");
        });
      });

      describe('using pre-declared nodes', function() {
        it('generates the correct XML', function() {
          var feed, ns, result, title, type;
          result = '';
          w.on('data', function(data) {
            result += data;
          });
          ns = w.declareNamespace('http://www.w3.org/2005/Atom', '');
          feed = w.declareElement(ns, 'feed');
          title = w.declareElement(ns, 'title');
          type = w.declareAttribute('type');
          w.startDocument()
            .startElement(feed)
            .startElement(title).addAttribute(type, 'text')
            .addText('Testing')
            .endElement()
            .endElement()
            .endDocument();

          result.should.equal("<feed xmlns=\"http://www.w3.org/2005/Atom\">\n\t<title type=\"text\">Testing</title>\n</feed>");
        });
      });

      describe('using a mix of literal and declared nodes', function() {
        it('generates the correct XML', function() {
          var feed, ns, result, type, uri;
          result = '';

          w.on('data', function(data) {
            result += data;
          });

          uri = 'http://www.w3.org/2005/Atom';
          ns = w.declareNamespace(uri, '');
          feed = w.declareElement(ns, 'feed');
          type = w.declareAttribute('type');

          w.startDocument()
            .startElement(feed)
            .startElementLiteral(uri, 'title').addAttribute(type, 'text')
            .addText('Testing')
            .endElement()
            .endElement()
            .endDocument();

          result.should.equal("<feed xmlns=\"http://www.w3.org/2005/Atom\">\n\t<title type=\"text\">Testing</title>\n</feed>");
        });
      });
    });

    describe('generating a prettyprinted document with custom newline and space character', function() {
      beforeEach(function () {
        w = new genx.Writer(true, "\r\n", "--");
      });

      describe('using literal nodes', function () {
        it('generates the correct XML', function () {
          var result = '';

          w.on('data', function (data) {
            result += data;
          });

          w.startDocument()
            .addComment(' Testing ')
            .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
              .startElementLiteral('title')
                .addAttributeLiteral('type', 'text')
                .addText('Testing')
              .endElement()
            .endElement()
            .endDocument();
          result.should.equal("<!-- Testing -->\r\n<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\r\n--" +
                              "<title type=\"text\">Testing</title>\r\n</g1:feed>");
        });

        it('correctly spaces trailing text nodes', function () {
          var result = '';

          w.on('data', function (data) {
            result += data;
          });

          w.startDocument()
            .addComment(' Testing ')
            .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
              .startElementLiteral('title')
                .addAttributeLiteral('type', 'text')
                .addText('Testing')
              .endElement()
              .addText('Trailing Text')
            .endElement()
            .endDocument();
          result.should.equal("<!-- Testing -->\r\n<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\r\n--" +
                              "<title type=\"text\">Testing</title>\r\n--Trailing Text\r\n</g1:feed>");
        });

        it('correctly spaces comments', function () {
          var result = '';

          w.on('data', function (data) {
            result += data;
          });

          w.startDocument()
            .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
              .startElementLiteral('title')
                .addAttributeLiteral('type', 'text')
                .addText('Testing')
              .endElement()
              .addComment(' Testing ')
              .addText('Trailing Text')
              .startElementLiteral('comment-only')
                .addComment(' Testing ')
              .endElement()
            .endElement()
            .endDocument();
          result.should.equal("<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\r\n--" +
                              "<title type=\"text\">Testing</title>\r\n--<!-- Testing -->\r\n--" +
                              "Trailing Text\r\n--<comment-only><!-- Testing --></comment-only>\r\n</g1:feed>");
        });

        it('correctly spaces self-closing tags', function () {
          var result = '';

          w.on('data', function (data) {
            result += data;
          });

          w.startDocument()
             .startElementLiteral('http://www.w3.org/2005/Atom', 'feed')
               .startElementLiteral('title')
                 .addAttributeLiteral('type', 'text')
               .endElementInline()
               .startElementLiteral('inline-element').endElementInline()
               .startElementLiteral('next-element')
                 .addText('More Text')
               .endElement()
             .endElement()
           .endDocument();
          result.should.equal("<g1:feed xmlns:g1=\"http://www.w3.org/2005/Atom\">\r\n--" +
                              "<title type=\"text\" />\r\n--<inline-element />\r\n--" +
                              "<next-element>More Text</next-element>\r\n</g1:feed>");
        });
      });
    });

    it('can generate multiple documents', function() {
      var result = '';

      w.on('data', function(data) {
        result += data;
      });

      w.startDocument().startElementLiteral('doc1').endElement().endDocument();
      result += "\n";
      w.startDocument().startElementLiteral('doc2').endElement().endDocument();

      result.should.equal("<doc1></doc1>\n<doc2></doc2>");
    });
  });
});
