GraphBuilder {
	//used to create an out ugen automatically and a fade envelope
	
	*wrapOut { arg name, func, rates, prependArgs, outClass=\Out, fadeTime;
		^SynthDef.new(name, { arg i_out=0;
			var result, rate, env;
			result = SynthDef.wrap(func, rates, prependArgs).asUGenInput;
			rate = result.rate;
			if(rate === \scalar,{
				// Out, SendTrig etc. probably a 0.0
				result
			},{
				if(fadeTime.notNil, {
						result = this.makeFadeEnv(fadeTime) * result;
				});
				outClass = outClass.asClass;
				outClass.replaceZeroesWithSilence(result.asArray);
				outClass.multiNewList([rate, i_out]++result)
			})
		})
	}

	*makeFadeEnv { arg fadeTime;
		var dt, gate;
		#dt, gate = Control.names(['fadeTime', 'gate']).kr([fadeTime, 1.0]);
		^Linen.kr(gate, dt, 1.0, dt, 2);
	
	}



}


EnvGate {
		*new { arg i_level=1, gate, fadeTime, doneAction=2, curve='sin';			var synthGate, synthFadeTime;
				if(gate.isNil and: { fadeTime.isNil }) {
					#synthGate, synthFadeTime = // optimization
						Control.names(['gate', 'fadeTime']).kr([1,0.02])
				} {
					synthGate = gate ?? { Control.names('gate').kr(1.0) };					synthFadeTime = fadeTime ?? { Control.names('fadeTime').kr(0.02) };
				};
				
				^EnvGen.kr(
					Env.new(#[0,1,0],#[1,1],curve,1),
					synthGate, i_level, 0.0, synthFadeTime, doneAction
				)
               }
}



// in the kr case, maybe leave the old value on the bus (levels: [0, 1]) and use an XOut?


