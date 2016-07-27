Server.default = s = Server("belaServer", NetAddr("127.0.0.1", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;
    	
	(	
	SynthDef("help-AnalogOutput",{ arg out=0;
	    AnalogOutput.ar( DC.ar( 0 ), SinOsc.ar( 10 ) );
	}).send(s);
	);

	s.sync;
	Synth.new("help-AnalogOutput", target: s).postln;
};
