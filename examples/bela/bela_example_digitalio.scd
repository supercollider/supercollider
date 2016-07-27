Server.default = s = Server("belaServer", NetAddr("127.0.0.1", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;
    	
	(	
	SynthDef("help-DigitalIO",{ arg out=0;
	    DigitalIO.ar( DC.ar( 0 ), SinOsc.ar( 10 ), K2A.ar( LFPulse.kr( 0.1 ) ) ).poll;
	}).send(s);
	);

	s.sync;
	Synth.new("help-DigitalIO", target: s).postln;
};
