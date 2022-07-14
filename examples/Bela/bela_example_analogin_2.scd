Server.default = Server.remote("belaServer", NetAddr("192.168.7.2", 57110));

fork{
	s.sync;

	SynthDef("help-AnalogIn", { arg out=0;
		Out.ar(out,
			SinOsc.ar(AnalogIn.ar(DC.ar(0)).exprange(200, 5000), 0, 0.1)
		)
	}).add;

	s.sync;
	Synth("help-AnalogIn").postln;
};

s.freeAll;
