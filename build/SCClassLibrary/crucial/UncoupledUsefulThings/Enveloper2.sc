

// audio function plays continously but is gated by the envelope
Enveloper2 { 
		
	*ar { arg audio,gate,env;
		var ts;
		env ?? {env = Env.adsr};
		^audio * EnvGen.kr(env,gate,Latch.kr(gate,gate))
	}

}

