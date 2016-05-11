s = Server.default;

s.options.numAnalogIOChannels = 8;
s.options.blockSize = 16;
s.options.numInputBusChannels = 2;
s.options.numOutputBusChannels = 2;

s.options.postln;

s.waitForBoot({
  // modulate frequency of a sine oscillator
  (	
  SynthDef("help-AnalogInput",{ arg out=0;
	  Out.ar(out,
	    SinOsc.ar( AnalogInput.ar( 0 ).exprange( 200, 5000 ), 0, 0.1 )
	  )
  }).play;
  );
});