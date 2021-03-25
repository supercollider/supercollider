Server.default = Server.remote("belaServer", NetAddr("192.168.7.2", 57110));

fork{
	s.sync;

	SynthDef("help-DigitalIO",{ arg out=0;
	    DigitalIO.ar( DC.ar( 0 ), SinOsc.ar( 10 ), K2A.ar( LFPulse.kr( 0.1 ) ) ).poll;
	}).add;

	s.sync;
	Synth("help-DigitalIO").postln;
};
