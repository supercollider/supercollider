Server.default = Server.remote("belaServer", NetAddr("192.168.7.2", 57110));

fork{
	s.sync;

	SynthDef("help-DigitalOut", { arg out=0;
		DigitalOut.ar(0, SinOsc.ar(10));
	}).add;

	s.sync;
	Synth("help-DigitalOut").postln;
};
