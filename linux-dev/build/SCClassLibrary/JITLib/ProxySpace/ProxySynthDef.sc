ProxySynthDef : SynthDef {
	var <rate, <numChannels;
	
	*initClass {
		//clean up
		unixCmd("rm synthdefs/temp_proxy_def_*");
	}
	
	*new { arg name, object, makeFadeEnv=true, channelOffset=0;
		name = "temp_proxy_def_" ++ name;
		^super.prNew(name).build(object,makeFadeEnv,channelOffset);
	}
	
	build { arg object,makeFadeEnv,channelOffset;
		var argNames, argValues, func;
		argNames = object.argNames;
		argValues = object.defArgs.asArray.extend(argNames.size);
		argValues = argValues.collect({ arg x; x ? 0.0 }); 
		
		func = {
			var synthGate, synthFadeTime, envgen, output, ctl, ok, outCtl;
				ctl = Control.names(argNames).kr(argValues);
				output = object.valueArray(ctl).asArray;
				rate = output.rate;
				numChannels = output.size;
				
				envgen = if(makeFadeEnv, {
					synthGate = Control.names('__synthGate__').kr(1.0);
					synthFadeTime = Control.names('__synthFadeTime__').kr(0.02);
					Linen.kr(synthGate,synthFadeTime, 1.0, synthFadeTime, 2)	
				}, { 
					1.0 
				});
				output = envgen*output;
				if(rate === 'scalar', {
					output
					}, {
					outCtl = Control.names(\out).ir(0) + channelOffset;
					//if((rate === 'control') && (proxy.rate === 'audio'), 
					//{ output = K2A.ar(output) }); //change in NodeProxy-initBus
					Out.multiNewList([rate, outCtl]++output)
				})
		};
		
		super.build(func);
	
	}
	
	
}