genx = require '../build/default/genx'

describe 'genx', ->

  beforeEach ->
    this.addMatchers
      toBeInstanceOf: (expected) ->
        this.actual instanceof expected

  describe 'Writer', ->
    w = null

    beforeEach ->
      w = new genx.Writer()

    describe 'declareNamespace', ->
      it 'raises an exception if there is the wrong number of arguments', ->
        expect(-> w.declareNamespace()).toThrow('Wrong number of arguments to declareNamespace')
        expect(-> w.declareNamespace(true, true, true)).toThrow('Wrong number of arguments to declareNamespace')

      describe 'with a prefix', ->

        it 'raises an exception if the uri is not a string', ->
          expect(-> w.declareNamespace(1, 2)).toThrow('First argument to declareNamespace must be a string')

        it 'raises an exception if the prefix is not a string', ->
          expect(-> w.declareNamespace('http://example.com/', 1)).toThrow('Second argument to declareNamespace must be a string')

        it 'raises an exception if the prefix is not valid', ->
          expect(-> w.declareNamespace('http://example.com/', 'invalid/prefix')).toThrow('Bad NAME')

        it "returns a Namespace object", ->
          expect(w.declareNamespace 'http://example.com/', 'test').toBeDefined()

      describe 'without a prefix', ->

        it "returns a Namespace object", ->
          expect(w.declareNamespace 'http://example.com/').toBeDefined()

    describe 'declareElement', ->
      it 'raises an exception if there is the wrong number of arguments', ->
        expect(-> w.declareElement()).toThrow('Wrong number of arguments to declareElement')
        expect(-> w.declareElement(true, true, true)).toThrow('Wrong number of arguments to declareElement')

      it 'raises an exception if the element name is invalid', ->
        w.startDocument()
        expect(-> w.declareElement('test>')).toThrow('Bad NAME')

      describe 'without a namespace', ->

        it 'raises an exception if it isn\'t a string', ->
          expect(-> w.declareElement(1)).toThrow('First argument must be a string')

        it 'returns an Element object', ->
          expect(w.declareElement 'test').toBeDefined()

      describe 'with a namespace', ->
        ns = null

        beforeEach ->
          ns = w.declareNamespace('test')

        it 'returns an Element object', ->
          expect(w.declareElement ns, 'test').toBeDefined()
