Server.default = s = Server("belaServer", NetAddr("127.0.0.1", 57110));
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
