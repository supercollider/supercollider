Server.default = s = Server("belaServer", NetAddr("192.168.7.2", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;

	(
	SynthDef("help-DigitalIn",{ arg out=0;
		Out.ar(out,
		  SinOsc.ar( 500, 0, 0.1 * DigitalIn.ar( 0 ) )
		)
	}).send(s);
	);

	s.sync;
	Synth.new("help-DigitalIn", target: s).postln;
};
