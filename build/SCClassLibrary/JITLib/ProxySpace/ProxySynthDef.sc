ProxySynthDef : SynthDef {
	var <>proxy;
	classvar <env;
	
	*initClass {
		//clean up
		env =  Env.linen(1.0, 1.0, 1.0);
		unixCmd("rm synthdefs/temp_proxy_def_*");
	}
	
	*new { arg proxy, object, channelOffset=0;
		var name;
		name = "temp_proxy_def_" ++ proxy.generateUniqueName;
		^super.prNew(name).proxy_(proxy).build(object,channelOffset);
	}
	
	build { arg object,channelOffset;
		var argNames, argValues, func;
		argNames = object.argNames;
		argValues = object.defArgs;
		argValues = argNames.collect({ arg x,i; 
					argValues.at(i) ? 0.0 
		});
		func = {
			var synthGate, synthFadeTime, envgen, output, ctl, ok, rate;
				ctl = Control.names(argNames).kr(argValues);
				output = object.valueArray(ctl).asArray;
				rate = output.rate;
				ok = proxy.initBus(rate, output.size);
				if(ok.not, { ^nil });
				
				envgen = if(proxy.freeSelf, {
					synthGate = Control.names(\synthGate).kr(1.0);
					synthFadeTime = Control.names(\synthFadeTime).kr(0.02);
					Linen.kr(synthGate,synthFadeTime, 1.0, synthFadeTime, 2)	
				}, { 
					1.0 
				});
				output = envgen*output;
				if(rate === 'scalar', {
					output
					}, {
					//if((rate === 'control') && (proxy.rate === 'audio'), 
					//{ output = K2A.ar(output) }); //change in NodeProxy-initBus
					Out.multiNewList([rate, proxy.outbus.index+channelOffset]++output)
				})
		};
		
		super.build(func);
	
	}
	
	
}