Server.default = s = Server("belaServer", NetAddr("127.0.0.1", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;
    	
	(	
	  SynthDef("help-AnalogInput",{ arg out=0; 
	    Out.ar(out, 
	      SinOsc.ar( AnalogInput.ar( DC.ar( 0 ) ).exprange( 200, 5000 ), 0, 0.1 ) 
	    ) 
	}).send(s);
	);

	s.sync;
	Synth.new("help-AnalogInput", target: s).postln;
};
