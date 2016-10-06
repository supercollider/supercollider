Server.default = s = Server("belaServer", NetAddr("192.168.7.2", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;

	(
	SynthDef("help-AnalogOut",{ arg out=0;
	    AnalogOut.ar( DC.ar( 0 ), SinOsc.ar( 10, 0, 0.5, 0.5 ) );
	}).send(s);
	);

	s.sync;
	Synth.new("help-AnalogOut", target: s).postln;
};
