ProxySynthDef : SynthDef {
	
	var <>rate, <>numChannels; 
	var <>canReleaseSynth, <>canFreeSynth;
	
	
	*initClass {
		//clean up any written synthdefs starting with "temp__"
		unixCmd("rm synthdefs/temp__*");
	}	
	
	*new { arg name, func, rates, prependArgs, makeFadeEnv=true, channelOffset=0, chanConstraint;
		var def, rate, numChannels, output, isScalar, envgen, canFree, hasOwnGate;
		var hasGateArg=false, hasOutArg=false;
		name = "temp__" ++ name;		
		def = super.new(name, { 
			var  out, outCtl;
			
			// build the controls from args
			output = SynthDef.wrap(func, rates, prependArgs);
			
			// determine rate and numChannels of ugen func
			rate = output.rate;
			isScalar = rate === 'scalar';
			numChannels = output.numChannels;
			// check for out key. this is used by internal control.
			func.def.argNames.do { arg name;
				if(name === \out) { hasOutArg = true };
				if(name === \gate) { hasGateArg = true };
			};
			
			if(isScalar.not and: hasOutArg)
			{ 
				"out argument is provided internally!".error; 
				^nil 
			};
			
			//detect inner gates
			canFree = UGen.buildSynthDef.children.canFreeSynth;
			hasOwnGate = UGen.buildSynthDef.hasGateControl;
			makeFadeEnv = if(hasOwnGate && canFree.not) { 
							"warning: gate does not free synth!".inform; false 
						} {
							makeFadeEnv and: { (isScalar || canFree).not };
						};
			
			hasOwnGate = canFree && hasOwnGate; //only counts when it can actually free synth.
			if(hasOwnGate.not && hasGateArg) {
				"supplied gate overrides inner gate.".error;
				^nil 
			};
			//"gate detection:".postln;
			//[\makeFadeEnv, makeFadeEnv, \canFree, canFree, \hasOwnGate, hasOwnGate].debug;
			
			// constrain the output to the right number of channels if supplied
			// if control rate, no channel wrapping is applied
			// and wrap it in a fade envelope
			envgen = if(makeFadeEnv, { EnvGate.new }, { 1.0 });
			if(chanConstraint.notNil  
				and: { chanConstraint < numChannels }
				and: { isScalar.not }, 
				{
					if(rate === 'audio') {
						postln(
						"wrapped channels from" + numChannels 
						+ "to" + chanConstraint + "channels");
						output = NumChannels(output, chanConstraint, true);
						numChannels = chanConstraint;
					} {
						postln("kept first" + chanConstraint + "channels from" 
						+ numChannels + "channel input");
						output = output.keep(chanConstraint);
						numChannels = chanConstraint;
					}
					
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
		def.canFreeSynth = def.canReleaseSynth || canFree;
		//[\defcanReleaseSynth, def.canReleaseSynth, \defcanFreeSynth, def.canFreeSynth].debug;
		^def
	}
	
	
}

EnvGate {
		*new { arg i_level=1, gate, fadeTime, doneAction=2;			var synthGate, synthFadeTime;
				synthGate = gate ?? { Control.names('gate').kr(1.0) };				synthFadeTime = fadeTime ?? { Control.names('fadeTime').kr(0.02) };				^EnvGen.kr(
					Env.new(#[0,1,0],#[1,1],'sin',1),
					synthGate, i_level, 0.0, synthFadeTime, doneAction
				)
               }
}

