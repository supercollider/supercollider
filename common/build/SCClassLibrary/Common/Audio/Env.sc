
Env {
	// envelope specification for an EnvGen, Env is not a UGen itself
	var <levels;
	var <times;
	var <curves = 'lin';		// can also be 'exp', 'sin', 'cos', a float curve value,
								// or an array of curve values
	var <releaseNode;	// index of release level, if nil then ignore release;
	var <loopNode;		// index of loop start level, if nil then does not loop;

	var <array;

	classvar <shapeNames;

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
			\sine -> 3,
			\wel -> 4,
			\welch -> 4,
			\sqr -> 6,
			\squared -> 6,
			\cub -> 7,
			\cubed -> 7
		];
	}

	levels_ { arg z;
		levels = z;
		array = nil;
	}
	times_ { arg z;
		times = z;
		array = nil;
	}
	curves_ { arg z;
		curves = z;
		array = nil;
	}
	releaseNode_ { arg z;
		releaseNode = z;
		array = nil;
	}
	loopNode_ { arg z;
		loopNode = z;
		array = nil;
	}
	== { arg that;
		^this.compareObject(that, ['levels','times','curves','releaseNode','loopNode'])
	}
	hash {
		^this.instVarHash(['levels','times','curves','releaseNode','loopNode'])
	}

	asArray {
		if (array.isNil) { array = this.prAsArray }
		^array
	}

	at { arg time;
		var array = this.asArray;
		^if(time.isSequenceableCollection) {
			time.collect { |t| array.envAt(t) }
		} {
			array.envAt(time)
		}
	}

	asPseg {
		var c = if(curves.isSequenceableCollection.not) { curves } { Pseq(curves) };
		^Pseg(Pseq(levels), Pseq(times ++ [1.0]), c) // last time is a dummy
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
	*cutoff { arg releaseTime = 0.1, level = 1.0, curve = \lin;
		var curveNo = this.shapeNumber(curve);
		var releaseLevel = if(curveNo==2){
			-100.dbamp
		}{
			0
		};
		^this.new([level, releaseLevel], [releaseTime], curve, 0)
	}
	*dadsr { arg delayTime=0.1, attackTime=0.01, decayTime=0.3,
			sustainLevel=0.5, releaseTime=1.0,
				peakLevel=1.0, curve = -4.0, bias = 0.0;
		^this.new(
			[0, 0, peakLevel, peakLevel * sustainLevel, 0] + bias,
			[delayTime, attackTime, decayTime, releaseTime],
			curve,
			3
		)
	}
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

	// delay the onset of the envelope	delay { arg delay;
		^Env([levels[0]] ++ levels,
			[delay] ++ times,
			if (curves.isArray) {[\lin] ++ curves} {curves},
			if(releaseNode.notNil) {releaseNode = releaseNode + 1},
			if(loopNode.notNil) {loopNode = loopNode + 1}
		)
	}

	// connect releaseNode (or end) to first node of envelope
	circle { arg timeFromLastToFirst = 0.0, curve = 'lin';
		var first0Then1 = Latch.kr(1.0, Impulse.kr(0.0));
		if(releaseNode.isNil) {
			levels = [0.0]++ levels ++ 0.0;
			curves = [curve]++ curves.asArray.wrapExtend(times.size) ++ 'lin';
			times  = [first0Then1 * timeFromLastToFirst] ++ times ++ inf;
			releaseNode = levels.size - 2;
		} {
			levels = [0.0]++ levels;
			curves = [curve]++ curves.asArray.wrapExtend(times.size);
			times  = [first0Then1 * timeFromLastToFirst] ++ times;
			releaseNode = releaseNode + 1;
		};
		loopNode = 0;
	}
	
	/*
	plot {
		var timeScale;
		timeScale = 0.01 / times.sum;
		Synth.plot({ arg synth;
			synth.releaseTime = 0.005;
			EnvGen.ar(this, 1, 0, 1, 0, timeScale)
		}, 0.01)
	}
	*/

	isSustained {
		^releaseNode.notNil
	}

	shapeNumber { arg shapeName;
		this.deprecated(thisMethod, this.class.class.findMethod(\shapeNumber));
		^this.class.shapeNumber(shapeName)
	}
	*shapeNumber { arg shapeName;
		var shape;
		if (shapeName.isValidUGenInput) { ^5 };
		shape = shapeNames.at(shapeName);
		if (shape.notNil) { ^shape };
		Error("Env shape not defined.").throw;
	}
	curveValue { arg curve;
		if (curve.isValidUGenInput, { ^curve },{ ^0 });
	}

//	send { arg netAddr, bufnum;
//		var array;
//		array = this.asArray;
//		netAddr.sendMsg("buf.setn", bufnum, 0, array.size, *array);
//	}

	test { arg releaseTime = 3.0;
		var id, def, s;
		s = Server.default;
		if(s.serverRunning.not) { "Server not running.".warn; ^this };
		id = s.nextNodeID;
		fork {
			def = { arg gate=1;
				Out.ar(0,
					SinOsc.ar(800, pi/2, 0.3) * EnvGen.ar(this, gate, doneAction:2)
				)
			}.asSynthDef;
			def.send(s);
			s.sync;
			s.sendBundle(s.latency, [9, def.name, id]);
			if(s.notified) {
				OSCpathResponder(s.addr, ['/n_end', id], { |time, responder, message|
					s.sendMsg(\d_free, def.name);
					responder.remove;
				}).add;
			};
			if(this.isSustained) {
				s.sendBundle(s.latency + releaseTime, [15, id, \gate, 0])
			};
		};
	}


	storeArgs { ^[levels, times, curves, releaseNode, loopNode] }


	prAsArray {
		var contents, curvesArray;
		contents = [levels.at(0), times.size,
				releaseNode ? -99, loopNode ? -99];
		curvesArray = curves.asArray;
		times.size.do({ arg i;
			contents = contents ++ [
				levels.at(i+1),
				times.at(i),
				this.class.shapeNumber(curvesArray.wrapAt(i)),
				this.curveValue(curvesArray.wrapAt(i))
			];
		});
		^contents
	}

	discretize {arg n = 1024;
		^this.asSignal(n);
	}

	range { |lo=0, hi=1|
		^this.class.new(levels.linlin(levels.minItem, levels.maxItem, lo, hi),
			times, curves, releaseNode, loopNode)
	}

	exprange { |lo=0, hi=1|
		^this.class.new(levels.linexp(levels.minItem, levels.maxItem, lo, hi),
			times, curves, releaseNode, loopNode)
	}

}

