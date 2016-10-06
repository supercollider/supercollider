Server.default = s = Server("belaServer", NetAddr("192.168.7.2", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;

	(
	SynthDef("help-DigitalOut",{ arg out=0;
		DigitalOut.ar( 0, SinOsc.ar( 10 ) );
	}).send(s);
	);

	s.sync;
	Synth.new("help-DigitalOut", target: s).postln;
};
