ProxySynthDef : SynthDef {
	var <>rate, <>numChannels, <>hasGate=true, <>canFreeSynth=false;
	
	
	*initClass {
		//clean up
		unixCmd("rm synthdefs/temp__*");
	}
		
	
	*new { arg name, func, lags, prependArgs, makeFadeEnv=true, channelOffset=0;
		var def, rate, output, isScalar, envgen;
		name = "temp__" ++ name;		
		def = super.new(name, { 
			var  out, outCtl;
			
			prependArgs = prependArgs.collect({ arg parg; parg.value });
			if(func.def.argNames.asArray.includes(\out), { 
				"out argument is provided internally!".error; ^nil 
			});
			output = SynthDef.wrap(func, lags, prependArgs);
			rate = output.rate;
			isScalar = rate === 'scalar';
			envgen = if(makeFadeEnv and: { isScalar.not }, { this.makeFadeEnv }, { 1.0 });
			output = output * envgen;
			if(isScalar, {
					output
				}, {
					outCtl = Control.names(\out).ir(0) + channelOffset;
					Out.multiNewList([rate, outCtl]++output)
			})
		});
		def.rate = rate;
		def.numChannels = output.numChannels;
		if(isScalar, { def.hasGate = false });
		def.canFreeSynth = def.prCanFreeSynth(envgen);
		^def
	}
	
	*makeFadeEnv {
		var synthGate, synthFadeTime;
		synthGate = Control.names('#gate').kr(1.0);
		synthFadeTime = Control.names('#fadeTime').kr(0.02);
		^EnvGen.kr(Env.new(#[0,1,0],[1,1],'sin',1), synthGate, 1.0, 0, synthFadeTime, 2)	}
	
	//private
	
	prCanFreeSynth { arg apartFrom;
		^children.any({ arg ugen; 
			ugen.canFreeSynth 
			and: { ugen !== apartFrom } 
		})
	}
	
	
	
}