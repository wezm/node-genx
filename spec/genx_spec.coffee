genx = require '../build/default/genx'

describe 'genx', ->
  
  describe 'Writer', ->
    w = null
  
    beforeEach ->
      w = new genx.Writer()
    
    describe 'declareElement', ->
      it 'raises an exception if there is the wrong number of arguments', ->
        expect(-> w.declareElement()).toThrow('Wrong number of arguments to declareElement')
        expect(-> w.declareElement(true, true, true)).toThrow('Wrong number of arguments to declareElement')
