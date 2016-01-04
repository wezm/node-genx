
// Check if we're in debug mode
var genxDir = !!process.env.GENX_DEBUG ? '../build/Debug/genx' : '../build/Release/genx',
    EventEmitter = require('events').EventEmitter,
    genx         = require(genxDir);
genx.Writer.prototype.__proto__ = EventEmitter.prototype;

module.exports = genx;
