
(
SynthDef("env", { arg i_outbus=0;
	var env, envctl;
	
	// make a dummy 8 segment envelope
	env = Env.newClear(8);
	
	// create a control argument array
	envctl = Control.names([\env]).kr( env.asArray );
	
	ReplaceOut.kr(i_outbus, EnvGen.kr(envctl, doneAction: 2));
}).add;


SynthDef("sine", {
	Out.ar(0, SinOsc.ar(In.kr(0), 0, 0.2));
}).add;
)

s.sendMsg(\c_set, 0, 800);

s.sendMsg(\s_new, \sine, 1001, 1, 0);

e = Env([700,900,900,800], [1,1,1]*0.4, \exp).asArray;
s.sendBundle(nil,[\s_new, \env, 1002, 2, 1001],[\n_setn, 1002, \env, e.size] ++ e);

f = Env([1000,1000,800,1000,900,1000], [1,1,1,1,1]*0.3, \step).asArray;
s.sendBundle(nil,[\s_new, \env, 1003, 2, 1001],[\n_setn, 1003, \env, f.size] ++ f);

s.sendMsg(\n_free, 1001);
