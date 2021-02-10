Server.default = Server.remote("belaServer", NetAddr("192.168.7.2", 57110));

fork{
	s.sync;

	SynthDef("help-DigitalIn", { arg out=0;
		Out.ar(out,
			SinOsc.ar(500, 0, 0.1 * DigitalIn.ar(0))
		)
	}).add;

	s.sync;
	Synth("help-DigitalIn").postln;
};
