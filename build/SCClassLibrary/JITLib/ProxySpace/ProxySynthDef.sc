ProxySynthDef : SynthDef {
	var <>proxy;
	
	*initClass {
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
		
		func = {
			var ctl, synthGate, synthFadeTime, output, env;
				argValues = argNames.collect({ arg x,i; 
					argValues.at(i) ? 0.0 
				});
				ctl = Control.names(argNames).kr(argValues);
				synthGate = Control.names(\synthGate).kr(1.0);
				synthFadeTime = Control.names(\synthFadeTime).kr(0.005);
				env = Linen.kr(synthGate, synthFadeTime, 1.0, synthFadeTime, 2);
			
				output = (env*object.valueArray(ctl)).asArray;
			//if((output.rate === 'control') && (proxy.rate === 'audio'), 
			//{ output = K2A.ar(output) });
				Out.multiNewList([proxy.rate, proxy.outbus.index]++output)
		};
		
		super.build(func);
	
	}
	
}