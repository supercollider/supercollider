ProxySynthDef : SynthDef {
	var <>rate, <>numChannels, <>hasGate=true, <>canFreeSynth=false;
	
	
	*initClass {
		//clean up any written synthdefs starting with "temp__"
		unixCmd("rm synthdefs/temp__*");
	}
		
	
	*new { arg name, func, lags, prependArgs, makeFadeEnv=true, channelOffset=0, chanConstraint;
		var def, rate, numChannels, output, isScalar, envgen;
		name = "temp__" ++ name;		
		def = super.new(name, { 
			var  out, outCtl;
			
			// check for out key. this is used by internal control.
			if(func.def.argNames.asArray.includes(\out), { 
				"out argument is provided internally!".error; ^nil 
			});
			
			// build the controls from args
			output = SynthDef.wrap(func, lags, prependArgs);
			
			// determine rate and numChannels of ugen func
			rate = output.rate;
			isScalar = rate === 'scalar';
			numChannels = output.numChannels;
			
			// constrain the output to the right number of channels if supplied
			// and wrap it in a fade envelope
			envgen = if(makeFadeEnv and: { isScalar.not }, { this.makeFadeEnv }, { 1.0 });
			if(chanConstraint.notNil and: { chanConstraint !== numChannels }, {
				output = NumChannels(output, chanConstraint, true);
			});
			output = output * envgen;
			
			if(isScalar, {
					output
				}, {
					outCtl = Control.names(\out).ir(0) + channelOffset;
					Out.multiNewList([rate, outCtl]++output)
			})
		});
		// set the synthDefs instvars, so they can be checked later
		
		def.rate = rate;
		def.numChannels = numChannels;
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