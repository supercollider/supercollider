
Env { 
	// envelope specification for an EnvGen, Env is not a UGen itself
	var <>levels;
	var <>times;
	var <>curves = 'lin';		// can also be 'exp', 'sin', 'cos', a float curve value, 
								// or an array of curve values
	var <>releaseNode;	// index of release level, if nil then ignore release;
	var <>loopNode;		// index of loop start level, if nil then does not loop;
	
	classvar shapeNames;
	
	*new { arg levels=#[0,1,0], times=#[1,1], curve='lin', releaseNode, loopNode;
		^super.newCopyArgs(levels, times, curve, releaseNode, loopNode)
	}
	*newClear { arg numSegments=8; 
		// make an envelope for filling in later.
		^this.new(Array.fill(numSegments+1,0), Array.fill(numSegments,1))
	}

	*initClass {
		shapeNames = IdentityDictionary[
			\step -> 0,
			\lin -> 1,
			\linear -> 1,
			\exp -> 2,
			\exponential -> 2,
			\sin -> 3,
			\cos -> 4,
			\sqr -> 6,
			\squared -> 6,
			\cubed -> 7
		];	
	}
	// methods to make some typical shapes :
	
	// fixed duration envelopes
	*triangle { arg dur=1.0, level=1.0;
		dur = dur * 0.5;
		^this.new(
			[0, level, 0],
			[dur, dur]
		)
	} 
	*sine { arg dur=1.0, level=1.0;
		dur = dur * 0.5;
		^this.new(
			[0, level, 0],
			[dur, dur],
			'sine'
		)
	} 
	*perc { arg attackTime=0.01, releaseTime=1.0, level=1.0, curve = -4.0;
		^this.new(
			[0, level, 0],
			[attackTime, releaseTime], 
			curve
		)
	} 
	*linen { arg attackTime=0.01, sustainTime=1.0, releaseTime=1.0, level=1.0, curve = \lin;
		^this.new(
			[0, level, level, 0],
			[attackTime, sustainTime, releaseTime], 
			curve
		)
	}
	
	
	// envelopes with sustain
	*adsr { arg attackTime=0.01, decayTime=0.3, 
			sustainLevel=0.5, releaseTime=1.0,
				peakLevel=1.0, curve = -4.0, bias = 0.0;
		^this.new(
			[0, peakLevel, peakLevel * sustainLevel, 0] + bias,
			[attackTime, decayTime, releaseTime], 
			curve,
			2
		)
	}
	
	*asr { arg attackTime=0.01, sustainLevel=1.0, releaseTime=1.0, curve = -4.0;
		^this.new(
			[0, sustainLevel, 0],
			[attackTime, releaseTime], 
			curve,
			1
		)
	}
	
	releaseTime {
		if(releaseNode.notNil,{
			^times.copyRange(releaseNode,times.size - 1).sum
		},{
			^0.0 // ?
		})
	}

	// blend two envelopes
	blend { arg argAnotherEnv, argBlendFrac=0.5;
		^this.class.new(
			levels.blend(argAnotherEnv.levels, argBlendFrac),
			times.blend(argAnotherEnv.times, argBlendFrac),
			curves.blend(argAnotherEnv.curves, argBlendFrac),
			releaseNode,
			loopNode
		)
	}
	plot {
		var timeScale;
		timeScale = 0.01 / times.sum;
		Synth.plot({ arg synth;
			synth.releaseTime = 0.005;
			EnvGen.ar(this, 1, 0, 1, 0, timeScale)
		}, 0.01)
	}
	test { arg releaseTime = 3.0;
		Synth.play({ arg synth;
			synth.releaseTime = releaseTime;
			EnvGen.ar(this, FSinOsc.ar(800, 0.3))
		})
	}
	
	shapeNumber { arg shapeName;
		^shapeNames.at(shapeName) ? 5
	}
	curveValue { arg curve;
		if (curve.isValidUGenInput, { ^curve },{ ^0 });
	}
	asArray {
		var contents, curvesArray;
		contents = [levels.at(0), times.size, 
				releaseNode ? -99, loopNode ? -99];
		curvesArray = curves.asArray;
		times.size.do({ arg i;
			contents = contents ++ [
				levels.at(i+1),
				times.at(i),
				this.shapeNumber(curvesArray.wrapAt(i)),
				this.curveValue(curvesArray.wrapAt(i))
			];
		});	
		^contents
	}
//	send { arg netAddr, bufnum;
//		var array;
//		array = this.asArray;
//		netAddr.performList(\sendMsg, "buf.setn", bufnum, 0, array.size, array);
//	}

	storeArgs { ^[levels, times, curves, releaseNode, loopNode] }

}

