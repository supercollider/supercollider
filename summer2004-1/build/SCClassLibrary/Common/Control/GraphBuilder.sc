GraphBuilder {
	//used to create an out ugen automatically and a fade envelope
	
	*wrapOut { arg name, func, rates, prependArgs, outClass=\Out, fadeTime;
		^SynthDef.new(name, { arg i_out=0;
			var result, rate, env;
			result = SynthDef.wrap(func, rates, prependArgs);
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