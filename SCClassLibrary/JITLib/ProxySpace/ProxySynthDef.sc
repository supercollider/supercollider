ProxySynthDef : SynthDef {

	var <>rate, <>numChannels;
	var <>canReleaseSynth, <>canFreeSynth;
	classvar <>sampleAccurate=false;


	*new { | name, func, rates, prependArgs, makeFadeEnv = true, channelOffset = 0,
		chanConstraint, rateConstraint |
		var def, rate, numChannels, output, isScalar, envgen, canFree, hasOwnGate;
		var hasGateArg=false, hasOutArg=false;
		var outerBuildSynthDef = UGen.buildSynthDef;
		def = super.new(name, {
			var  out, outCtl;

			// build the controls from args
			output = SynthDef.wrap(func, rates, prependArgs);
			output = output.asUGenInput;

			// protect from user error
			if(output.isKindOf(UGen) and: { output.synthDef != UGen.buildSynthDef }) {
				Error("Cannot share UGens between NodeProxies:" + output).throw
			};
			// protect from accidentally wrong array shapes
			if(output.containsSeqColl) {
				"Synth output should be a flat array.\n%\nFlattened to: %\nSee NodeProxy helpfile:routing\n\n".format(output, output.flat).warn;
				output = output.flat;
			};

			output = output ? 0.0;

			// determine rate and numChannels of ugen func
			numChannels = output.numChannels;
			rate = output.rate;
			isScalar = rate === 'scalar';

			// check for out key. this is used by internal control.
			func.def.argNames.do { arg name;
				if(name === \out) { hasOutArg = true };
				if(name === \gate) { hasGateArg = true };
			};

			if(isScalar.not and: hasOutArg)
			{
				"out argument is provided internally!".error; // avoid overriding generated out
				^nil
			};

			// rate is only scalar if output was nil or if it was directly produced by an out ugen
			// this allows us to conveniently write constant numbers to a bus from the synth
			// if you want the synth to write nothing, return nil from the UGen function.

			if(isScalar and: { output.notNil } and: { UGen.buildSynthDef.children.last.isKindOf(AbstractOut).not }) {
				rate = 'control';
				isScalar = false;
			};

			//detect inner gates
			canFree = UGen.buildSynthDef.children.canFreeSynth;
			hasOwnGate = UGen.buildSynthDef.hasGateControl;
			makeFadeEnv = if(hasOwnGate and: { canFree.not }) {
				"The gate control should be able to free the synth!\n%".format(func).warn; false
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
			envgen = if(makeFadeEnv) {
				EnvGate(1, nil, nil, 2, if(rate === 'audio') { 'sin' } { 'lin' })
			} { 1.0 };

			if(chanConstraint.notNil
				and: { chanConstraint < numChannels }
				and: { isScalar.not },
				{
					if(rate === 'audio') {
						postf("%: wrapped channels from % to % channels\n", NodeProxy.buildProxy, numChannels, chanConstraint);
						output = NumChannels.ar(output, chanConstraint, true);
						numChannels = chanConstraint;
					} {
						postf("%: kept first % channels from % channel input\n", NodeProxy.buildProxy, chanConstraint, numChannels);
						output = output.keep(chanConstraint);
						numChannels = chanConstraint;
					}

			});
			output = output * envgen;

			//"passed in rate: % output rate: %\n".postf(rateConstraint, rate);

			if(isScalar, {
				output
			}, {
				// rate adaption. \scalar proxy means neutral
				if(rateConstraint.notNil and: { rateConstraint != \scalar and: { rateConstraint !== rate }}) {
					if(rate === 'audio') {
						output = A2K.kr(output);
						rate = 'control';
						postf("%: adopted proxy input to control rate\n", NodeProxy.buildProxy);
					} {
						if(rateConstraint === 'audio') {
							output = K2A.ar(output);
							rate = 'audio';
							postf("%: adopted proxy input to audio rate\n", NodeProxy.buildProxy);
						}
					}
				};
				outCtl = Control.names(\out).ir(0) + channelOffset;
				(if(rate === \audio and: { sampleAccurate }) { OffsetOut } { Out }).multiNewList([rate, outCtl] ++ output)
			})
		});

		UGen.buildSynthDef = outerBuildSynthDef;

		// set the synthDefs instvars, so they can be used later

		def.rate = rate;
		def.numChannels = numChannels;
		def.canReleaseSynth = makeFadeEnv || hasOwnGate;
		def.canFreeSynth = def.canReleaseSynth || canFree;
		//[\defcanReleaseSynth, def.canReleaseSynth, \defcanFreeSynth, def.canFreeSynth].debug;
		^def
	}


}
