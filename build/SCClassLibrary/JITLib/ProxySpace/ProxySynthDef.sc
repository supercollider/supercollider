ProxySynthDef : SynthDef {
	
	var <>rate, <>numChannels; 
	var <>canReleaseSynth, <>canFreeSynth;
	
	
	*initClass {
		//clean up any written synthdefs starting with "temp__"
		unixCmd("rm synthdefs/temp__*");
	}	
	
	*new { arg name, func, lags, prependArgs, makeFadeEnv=true, channelOffset=0, chanConstraint;
		var def, rate, numChannels, output, isScalar, envgen, canFree, hasOwnGate;
		name = "temp__" ++ name;		
		def = super.new(name, { 
			var  out, outCtl, argNames;
			argNames = func.def.argNames.asArray;
			
			
			// build the controls from args
			output = SynthDef.wrap(func, lags, prependArgs);
			
			// determine rate and numChannels of ugen func
			rate = output.rate;
			isScalar = rate === 'scalar';
			numChannels = output.numChannels;
			
			// check for out key. this is used by internal control.
			if(argNames.includes(\out) && (isScalar.not), { 
				"out argument is provided internally!".error; 
				^nil 
			});
			
			//detect inner gates
			canFree = UGen.buildSynthDef.children.canFreeSynth;
			hasOwnGate = argNames.includes('gate');
			makeFadeEnv = if(hasOwnGate && canFree.not, { 
							"warning: gate does not free synth!".inform; false 
						}, {
							makeFadeEnv and: { (isScalar || canFree).not };
						});
			
			hasOwnGate = canFree && hasOwnGate; //only counts when it can actually free synth.
			
			//"gate detection:".postln;
			//[\makeFadeEnv, makeFadeEnv, \canFree, canFree, \hasOwnGate, hasOwnGate].debug;
			
			// constrain the output to the right number of channels if supplied
			// and wrap it in a fade envelope
			envgen = if(makeFadeEnv, { this.makeFadeEnv }, { 1.0 });
			if(chanConstraint.notNil and: { chanConstraint !== numChannels }, {
				output = NumChannels(output, chanConstraint, true);
				numChannels = chanConstraint;
			});
			output = output * envgen;
			
			if(isScalar, {
					output
				}, {
					outCtl = Control.names(\out).ir(0) + channelOffset;
					Out.multiNewList([rate, outCtl]++output)
			})
		});
		
		// set the synthDefs instvars, so they can be used later
		
		def.rate = rate;
		def.numChannels = numChannels;
		def.canReleaseSynth = makeFadeEnv || hasOwnGate;
		def.canFreeSynth = def.canReleaseSynth || canFree; //revisit
		//[\defcanReleaseSynth, def.canReleaseSynth, \defcanFreeSynth, def.canFreeSynth].debug;
		^def
	}
	
	*makeFadeEnv {
		var synthGate, synthFadeTime;
		synthGate = Control.names('gate').kr(1.0);
		synthFadeTime = Control.names('fadeTime').kr(0.02);
		^EnvGen.kr(Env.new(#[0,1,0],[1,1],'sin',1), synthGate, 1.0, 0, synthFadeTime, 2)	}
		
	
}