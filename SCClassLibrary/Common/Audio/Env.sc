AbstractEnv {
	var <levels;
	var <times;
	var <curves = 'lin';		// can also be 'exp', 'sin', 'cos', a float curve value,
							// or an array of curve values
	var <array;
	classvar <shapeNames;

	*newClear { arg numSegments = 8;
		// make an envelope for filling in later.
		^this.new(Array.fill(numSegments + 1, 0), Array.fill(numSegments, 1))
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

	asArray {
		if (array.isNil) { array = this.prAsArray }
		^array
	}

	*shapeNumber { arg shapeName;
		var shape;
		if (shapeName.isValidUGenInput) { ^5 };
		shape = shapeNames.at(shapeName);
		if (shape.notNil) { ^shape };
		Error("Env shape not defined.").throw;
	}

	curveValue { arg curve;
		^if(curve.isValidUGenInput) { curve } { 0 };
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

	range { |lo = 0.0, hi = 1.0|
		^this.copy.levels_(levels.linlin(levels.minItem, levels.maxItem, lo, hi))
	}

	exprange { |lo = 0.01, hi = 1.0|
		^this.copy.levels_(levels.linexp(levels.minItem, levels.maxItem, lo, hi))
	}

	asSignal { arg length = 400;
		var duration, signal, ratio;
		duration = times.sum;
		ratio = duration/(length - 1);
		signal = Signal(length);
		length.do({ arg i; signal.add(this.at(i * ratio)) });
		^signal;
	}

	discretize {arg n = 1024;
		^this.asSignal(n);
	}

}


Env : AbstractEnv {
	// envelope specification for an EnvGen, Env is not a UGen itself

	var <releaseNode;	// index of release level, if nil then ignore release;
	var <loopNode;		// index of loop start level, if nil then does not loop;


	*new { arg levels = #[0,1,0], times = #[1,1], curve = 'lin', releaseNode, loopNode;
		^super.newCopyArgs(levels, times, curve)
			// note, we may not use newCopyArgs for these because of other instance vars
			// in the superclass
			.releaseNode_(releaseNode).loopNode_(loopNode)
	}

	releaseNode_ { arg z;
		releaseNode = z;
		array = nil;
	}

	loopNode_ { arg z;
		loopNode = z;
		array = nil;
	}

	storeArgs { ^[levels, times, curves, releaseNode, loopNode] }

	== { arg that;
		^this.compareObject(that, ['levels','times','curves','releaseNode','loopNode'])
	}

	hash {
		^this.instVarHash(['levels','times','curves','releaseNode','loopNode'])
	}

	at { arg time;
		var array = this.asArray;
		^if(time.isSequenceableCollection) {
			time.collect { |t| array.envAt(t) }
		} {
			array.envAt(time)
		}
	}

	embedInStream { arg inval;
		var startTime;
		startTime = thisThread.endBeat ? thisThread.beats;
		thisThread.endBeat = this.times.sum + startTime;
		loop {
			inval = yield(this.at(thisThread.beats - startTime));
		}
	}

	asStream {
		^Routine({ arg inval; this.embedInStream(inval) })
	}

	asPseg {
		var c = if(curves.isSequenceableCollection.not) { curves } { Pseq(curves, inf) };
		^Pseg(Pseq(levels), Pseq(times ++ [1.0]), c) // last time is a dummy
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
			^times.copyRange(releaseNode, times.size - 1).sum
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

	// delay the onset of the envelope
	delay { arg delay;
		^Env([levels[0]] ++ levels,
			[delay] ++ times,
			if (curves.isArray) { [\lin] ++ curves } { curves },
			if(releaseNode.notNil) { releaseNode = releaseNode + 1 },
			if(loopNode.notNil) { loopNode = loopNode + 1 }
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

	isSustained {
		^releaseNode.notNil
	}

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

	prAsArray {
		var contents, curvesArray, size;
		size = times.size;
		contents = Array.new((size + 1) * 4);
		contents.add(levels.at(0));
		contents.add(size);
		contents.add(releaseNode ? -99);
		contents.add(loopNode ? -99);
		curvesArray = curves.asArray;
		size.do({ arg i;
			contents.add(levels.at(i+1));
			contents.add(times.at(i));
			contents.add(this.class.shapeNumber(curvesArray.wrapAt(i)));
			contents.add(this.curveValue(curvesArray.wrapAt(i)));
		});
		^contents
	}
}
