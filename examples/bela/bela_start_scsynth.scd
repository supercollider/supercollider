s = Server.default;

s.options.numAnalogInChannels = 8;
s.options.numAnalogOutChannels = 8;
s.options.numDigitalChannels = 16;

s.options.pgaGainLeft = 4;
s.options.pgaGainRight = 5;
s.options.headphoneLevel = -8;
s.options.speakerMuted = 0;
s.options.dacLevel = 5;
s.options.adcLevel = 3;
s.options.numMultiplexerChannels = 4;

s.options.blockSize = 16;
s.options.numInputBusChannels = 2;
s.options.numOutputBusChannels = 2;

s.options.postln;

s.boot;
