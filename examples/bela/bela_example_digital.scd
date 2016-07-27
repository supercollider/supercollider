Server.default = s = Server("belaServer", NetAddr("127.0.0.1", 57110));
s.initTree;
s.startAliveThread;

fork{
	s.sync;
  
	(  	
	SynthDef("help-DigitalInput",{ arg out=0;
		Out.ar(out,
		  SinOsc.ar( 500, 0, 0.1 * DigitalInput.ar( 0 ) )
		)
	}).send(s);
	);

	s.sync;
	Synth.new("help-DigitalInput", target: s).postln;
};
