ProxySynthDef : SynthDef {
	var <>proxy;
	classvar <defaultEnvelope;
	
	*initClass {
		defaultEnvelope = Env.linen(1.0, 1.0, 1.0);
		//clean up
		unixCmd("rm synthdefs/temp_proxy_def_*");
	}
	
	*new { arg proxy, object;
		var name;
		name = "temp_proxy_def_" 
		++ proxy.identityHash.abs ++ proxy.synthDefs.size;
		^super.prNew(name).proxy_(proxy).build(object);
	}
	
	build { arg object;
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
				
				envgen = if(proxy.autoRelease, {
					synthGate = Control.names(\synthGate).kr(1.0);
					synthFadeTime = Control.names(\synthFadeTime).kr(0.02);
					EnvGen.kr(proxy.env ? this.class.defaultEnvelope, 
						synthGate, 1.0, 0.0, synthFadeTime, 2);
				}, { 
					1.0 
				});
				output = envgen*output;
				if(rate === 'scalar', {
					output
					}, {
					//if((rate === 'control') && (proxy.rate === 'audio'), 
					//{ output = K2A.ar(output) }); //change in NodeProxy-initBus
					Out.multiNewList([rate, proxy.outbus.index]++output)
				})
		};
		
		super.build(func);
	
	}
	
	
}