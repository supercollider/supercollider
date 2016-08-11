Server.default = s = Server("belaServer", NetAddr("192.168.7.2", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;

	(
	  SynthDef("help-AnalogIn",{ arg out=0;
	    Out.ar(out,
	      SinOsc.ar( AnalogIn.ar( DC.ar( 0 ) ).exprange( 200, 5000 ), 0, 0.1 )
	    )
	}).send(s);
	);

	s.sync;
	Synth.new("help-AnalogIn", target: s).postln;
};

s.freeAll;