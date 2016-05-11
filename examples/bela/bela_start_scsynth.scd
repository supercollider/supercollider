s = Server.default;

s.options.numAnalogIOChannels = 8;
s.options.blockSize = 16;
s.options.numInputBusChannels = 2;
s.options.numOutputBusChannels = 2;

s.options.postln;

s.boot;

