ProxySynthDef : SynthDef {
	var <>rate, <>numChannels;
	
	
	*initClass {
		//clean up
		unixCmd("rm synthdefs/temp__*");
	}
		
	
	*new { arg name, func, lags, prependArgs, makeFadeEnv=true, channelOffset=0;
		var def, rate, output;
		name = "temp__" ++ name;		
		def = super.new(name, { 
			var envgen, synthGate, synthFadeTime, out, outCtl;
			
			prependArgs = prependArgs.collect({ arg parg; parg.value });
			if(func.def.argNames.asArray.includes(\out), { 
				"out argument is provided internally!".error; ^nil 
			});
			output = SynthDef.wrap(func, lags, prependArgs);
			rate = output.rate;
			
			envgen = if(makeFadeEnv, {
					synthGate = Control.names('#gate').kr(1.0);
					synthFadeTime = Control.names('#fadeTime').kr(0.02);
					//Linen.kr(synthGate,synthFadeTime, 1.0, synthFadeTime, 2)
					EnvGen.kr(Env.new(#[0,1,0],[1,1.25],'sin',1),synthGate,1,0,synthFadeTime,2)	
				}, { 
					1.0 
			});
			output = output * envgen;
			if(rate === 'scalar', {
					output
					}, {
					outCtl = Control.names(\out).ir(0) + channelOffset;
					Out.multiNewList([rate, outCtl]++output)
				})
		});
		def.rate = rate;
		def.numChannels = output.numChannels;
		^def
	}
	
	
}