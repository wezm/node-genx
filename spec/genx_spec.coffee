genx = require '../build/default/genx'

describe 'genx', ->
  
  describe 'Writer', ->
    w = null
  
    beforeEach ->
      w = new genx.Writer()

    describe 'declareNamespace', ->
      it 'raises an exception if there is the wrong number of arguments', ->
        expect(-> w.declareNamespace()).toThrow('Wrong number of arguments to declareNamespace')
        expect(-> w.declareNamespace(true, true, true)).toThrow('Wrong number of arguments to declareNamespace')

      describe 'with a prefix', ->

        it 'raises an exception if the prefix is not a string', ->
          expect(-> w.declareNamespace(1, 2)).toThrow('First argument to declareNamespace must be a string')

        it 'raises an exception if the uri is not a string', ->
          expect(-> w.declareNamespace('prefix', 1)).toThrow('Second argument to declareNamespace must be a string')

      describe 'without a prefix', ->


    describe 'declareElement', ->
      it 'raises an exception if there is the wrong number of arguments', ->
        expect(-> w.declareElement()).toThrow('Wrong number of arguments to declareElement')
        expect(-> w.declareElement(true, true, true)).toThrow('Wrong number of arguments to declareElement')

      describe 'with one argument', ->

        it 'raises an exception if it isn\'t a string', ->
          expect(-> w.declareElement(1)).toThrow('First argument must be a string')

        it 'returns an element', ->
          expect(w.declareElement 'test').toBeDefined()

      describe 'with two arguments', ->

