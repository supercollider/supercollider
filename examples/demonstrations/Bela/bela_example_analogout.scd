Server.default = Server.remote("belaServer", NetAddr("192.168.7.2", 57110));

fork{
	s.sync;

	SynthDef("help-AnalogOut", { arg out=0;
		AnalogOut.ar(DC.ar(0), SinOsc.ar(10, 0, 0.5, 0.5 ));
	}).add;

	s.sync;
	Synth("help-AnalogOut").postln;
};
