Server.default = s = Server("belaServer", NetAddr("127.0.0.1", 57110));
s.initTree;
s.startAliveThread;
    
(
  SynthDef("help-AnalogInput",{ arg out=0; 
    Out.ar(out, 
      SinOsc.ar( AnalogInput.ar( 0 ).exprange( 200, 5000 ), 0, 0.1 ) 
    ) 
}).send(s);
);

Synth.new("help-AnalogInput", target: s).postln;
