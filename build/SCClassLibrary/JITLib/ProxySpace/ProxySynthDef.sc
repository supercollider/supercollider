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
		var argNames, argValues, func, env;
		argNames = object.argNames;
		argValues = object.defArgs;
		
		
		func = {
			var ctl, synthGate, synthFadeTime, output, envgen;
				argValues = argNames.collect({ arg x,i; 
					argValues.at(i) ? 0.0 
				});
				ctl = Control.names(argNames).kr(argValues);
				envgen = if(proxy.autoRelease, {
					env = proxy.env ? this.class.defaultEnvelope;
					synthGate = Control.names(\synthGate).kr(1.0);
					synthFadeTime = Control.names(\synthFadeTime).kr(0.02);
					EnvGen.kr(env, synthGate, 1.0, 0.0, synthFadeTime, 2);
				}, { 
					1.0 
				});
				output = (envgen*object.valueArray(ctl)).asArray;
			//if((output.rate === 'control') && (proxy.rate === 'audio'), 
			//{ output = K2A.ar(output) });
				Out.multiNewList([proxy.rate, proxy.outbus.index]++output)
		};
		
		super.build(func);
	
	}
	
}