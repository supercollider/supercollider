s.options.numAnalogInChannels = 8;
s.options.numAnalogOutChannels = 8;
s.options.numDigitalChannels = 16;

s.options.pgaGainLeft = 4;
s.options.pgaGainRight = 5;
s.options.headphoneLevel = -8;
s.options.speakerMuted = 0;
s.options.dacLevel = 0;
s.options.adcLevel = 0;
s.options.numMultiplexChannels = 0;

s.options.blockSize = 16;
s.options.numInputBusChannels = 2;
s.options.numOutputBusChannels = 2;

// These lines enable access from remote hosts
s.options.bindAddress = "0.0.0.0";
s.options.maxLogins = 2;

s.boot;
