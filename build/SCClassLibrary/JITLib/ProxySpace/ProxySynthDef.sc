ProxySynthDef : SynthDef {
	var <>rate, <>numChannels;
	
	
	*initClass {
		//clean up
		unixCmd("rm synthdefs/temp_proxy_def_*");
	}
		
	
	*new { arg name, func, lags, prependArgs, makeFadeEnv=true, channelOffset=0;
		var def, rate, output;
		name = "temp_proxy_def_" ++ name;		
		def = super.new(name, { 
			var envgen, synthGate, synthFadeTime, out, outCtl;
			envgen = if(makeFadeEnv, {
					synthGate = Control.names('#gate').kr(1.0);
					synthFadeTime = Control.names('#fadeTime').kr(0.02);
					//Linen.kr(synthGate,synthFadeTime, 1.0, synthFadeTime, 2)
					EnvGen.kr(Env.new(#[0,1,0],[1,1.25],'sin',1),synthGate,1,0,synthFadeTime,2)	
				}, { 
					1.0 
				});
			prependArgs = prependArgs.collect({ arg parg; parg.value });
			output = envgen * SynthDef.wrap(func, lags, prependArgs);
			rate = output.rate;
			
				if(rate === 'scalar', {
					output
					}, {
					outCtl = Control.names(\out).ir(0) + channelOffset;
					//if((rate === 'control') && (proxy.rate === 'audio'), 
					//{ output = K2A.ar(output) }); //change in NodeProxy-initBus
					Out.multiNewList([rate, outCtl]++output)
				})
		});
		def.rate = rate;
		def.numChannels = output.numChannels;
		^def
	}
	
	
	
	
}