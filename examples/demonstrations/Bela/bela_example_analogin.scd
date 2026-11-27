s.options.numAnalogInChannels = 8;
s.options.numAnalogOutChannels = 8;
s.options.numDigitalChannels = 16;

s.options.blockSize = 16;
s.options.numInputBusChannels = 2;
s.options.numOutputBusChannels = 2;

s.waitForBoot{
	// modulate frequency of a sine oscillator
	SynthDef("help-AnalogIn", {arg out=0;
		Out.ar(out,
			SinOsc.ar( AnalogIn.ar( DC.ar( 0 ) ).exprange( 200, 5000 ), 0, 0.1 )
		)
	}).add;

	s.sync;
	Synth("help-AnalogIn");
};
