Env {
	var <levels, <times, <curves;
	var <releaseNode;	// index of release level, if nil then ignore release
	var <loopNode;	// index of loop start level, if nil then does not loop
	var <offset; 		// an offset to all time values (only works in IEnvGen)

	var <array;		// cache for osc-conform data
	classvar <shapeNames;


	*new { arg levels = #[0,1,0], times = #[1,1], curve = \lin, releaseNode, loopNode, offset = 0;
		times = times.asArray.wrapExtend(levels.size - 1);
		^super.newCopyArgs(levels, times, curve ? \lin, releaseNode, loopNode, offset)
	}

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
		shapeNames.freeze;
	}

	kr { arg doneAction = 0, gate = 1.0, timeScale = 1.0, mul = 1.0, add = 0.0;
		^EnvGen.kr(this, gate, mul, add, timeScale, doneAction)
	}

	ar { arg doneAction = 0, gate = 1.0, timeScale = 1.0, mul = 1.0, add = 0.0;
		^EnvGen.ar(this, gate, mul, add, timeScale, doneAction)
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
	offset_ { arg z;
		offset = z;
		array = nil;
	}

	duration_ { arg dur;
		times = times * this.totalDuration.reciprocal * dur
	}

	duration {
		^times.sum
	}

	totalDuration {
		var duration = times.sum;
		^duration.asArray.maxItem;
	}

	range { arg lo = 0.0, hi = 1.0;
		^this.copy.levels_(levels.linlin(levels.minItem, levels.maxItem, lo, hi))
	}

	exprange { arg lo = 0.01, hi = 1.0;
		^this.copy.levels_(levels.linexp(levels.minItem, levels.maxItem, lo, hi))
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
			\sine
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

	*xyc { arg xyc;
		var times, levels, curves, offset, order;
		#times, levels, curves = xyc.flop;
		if(times.containsSeqColl.not) { // sort triplets, if possible.
			order = times.order;
			times = times[order];
			levels = levels[order];
			curves = curves[order];
		};
		offset = times[0];
		times = times.differentiate.drop(1);
		curves.asArray.drop(-1);
		^this.new(levels, times, curves, offset: offset);
	}

	*pairs { arg pairs, curve;
		if(curve.isNil) { ^this.xyc(pairs) };
		^this.xyc(pairs +++ curve);
	}

	// envelopes with sustain

	*cutoff { arg releaseTime = 0.1, level = 1.0, curve = \lin;
		var curveNo = this.shapeNumber(curve);
		var releaseLevel = if(curveNo == 2) { -100.dbamp } { 0 };
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

	*circle { arg levels, times, curve = \lin;
		times = times.asArray.wrapExtend(levels.size);
		curve = curve.asArray.wrapExtend(levels.size);
		^this.new(levels, times.drop(-1), curve.drop(-1)).circle(times.last, curve.last);
	}

	releaseTime {
		if(releaseNode.isNil) { ^0.0 };
		^times.copyRange(releaseNode, times.size - 1).sum
	}

	isSustained {
		^releaseNode.notNil
	}

	asMultichannelSignal { arg length = 400, class = (Signal);
		var multiChannelArray = this.asMultichannelArray;
		var channelCount = multiChannelArray.size;

		var totalDur = this.totalDuration;

		length = max(length, levels.size);

		^multiChannelArray.collect { |chan|
			var signal, ratio;
			ratio = totalDur / (length - 1);
			signal = class.new(length);
			length.do { arg i; signal.add(chan.envAt(i * ratio)) };
			signal
		}
	}

	asSignal { arg length = 400;
		^this.asMultichannelSignal(length).unbubble
	}

	discretize { arg n = 1024;
		^this.asSignal(n);
	}

	storeArgs { ^[levels, times, curves, releaseNode, loopNode] }

	== { arg that;
		^this.compareObject(that, [\levels, \times, \curves, \releaseNode, \loopNode, \offset])
	}

	hash {
		^this.instVarHash([\levels, \times, \curves, \releaseNode, \loopNode, \offset])
	}

	at { arg time;
		var data = this.asMultichannelArray;
		time = (time - offset).max(0);
		^if(time.isSequenceableCollection) {
			if(data.size <= 1) {
				data = data[0];
				time.collect { |t| data.envAt(t) }
			} {
				time.collect { |t|
					data.collect { |channel| channel.envAt(t) }
				}
			}
		} {
			if(data.size <= 1) {
				data[0].envAt(time)
			} {
				data.collect { |channel| channel.envAt(time) }
			}
		}
	}

	embedInStream { arg inval;
		var startTime = thisThread.endBeat ? thisThread.beats;
		thisThread.endBeat = this.duration + startTime;
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
	circle { arg timeFromLastToFirst = 0.0, curve = \lin;
		var first0Then1;
		if(UGen.buildSynthDef.isNil) { ^this };
		first0Then1 = Latch.kr(1.0, Impulse.kr(0.0));
		if(releaseNode.isNil) {
			levels = [0.0] ++ levels ++ 0.0;
			curves = [curve] ++ curves.asArray.wrapExtend(times.size) ++ \lin;
			times  = [first0Then1 * timeFromLastToFirst] ++ times ++ inf;
			releaseNode = levels.size - 2;
		} {
			levels = [0.0] ++ levels;
			curves = [curve] ++ curves.asArray.wrapExtend(times.size);
			times  = [first0Then1 * timeFromLastToFirst] ++ times;
			releaseNode = releaseNode + 1;
		};
		loopNode = 0;
	}

	test { arg releaseTime = 3.0;
		var s = Server.default;
		if(s.serverRunning.not) { "Server not running.".warn; ^this };
		fork {
			var synth = { arg gate=1;
				SinOsc.ar(800, pi/2, 0.3) * EnvGen.ar(this, gate, doneAction:2)
			}.play;
			if(this.isSustained) {
				s.sendBundle(s.latency + releaseTime, [15, synth.nodeID, \gate, 0])
			};
		};
	}

	*shapeNumber { arg shapeName;
		^shapeName.asArray.collect { |name|
			var shape;
			if(name.isValidUGenInput) { 5 } {
				shape = shapeNames.at(name);
				if(shape.isNil) { Error("Env shape not defined.").throw };
				shape
			}
		}.unbubble
	}

	curveValue { arg curve;
		^if(curve.isSequenceableCollection) {
			curve.collect { |x|
				if(x.isValidUGenInput) { x } { 0 }
			}
		} {
			if(curve.isValidUGenInput) { curve } { 0 }
		}
	}

	asArray {
		if (array.isNil) { array = this.prAsArray }
		^array.unbubble // keep backward compatibility
	}

	asMultichannelArray {
		if (array.isNil) { array = this.prAsArray }
		^array
	}

	// this version is for IEnvGen and has a special format.
	// don't cache this version for now, but instead return it directly.
	asArrayForInterpolation {
		var contents, size;
		var levelArray = levels.asUGenInput;
		var timeArray = times.asUGenInput;
		var curvesArray = curves.asArray.asUGenInput;

		size = timeArray.size;
		contents = Array.new((size + 1) * 4);
		contents.add(offset.asUGenInput ? 0);
		contents.add(levelArray.at(0));
		contents.add(size);
		contents.add(timeArray.sum);
		curvesArray = curves.asArray;
		times.size.do { arg i;
			contents.add(timeArray[i]);
			contents.add(this.class.shapeNumber(curvesArray.wrapAt(i)));
			contents.add(this.curveValue(curvesArray.wrapAt(i)));
			contents.add(levelArray[i+1]);
		};

		^contents.flop
	}

	prAsArray {
		var contents, size;
		var levelArray = levels.asUGenInput;
		var timeArray = times.asUGenInput;
		var curvesArray = curves.asArray.asUGenInput;

		size = times.size;
		contents = Array.new((size + 1) * 4);
		contents.add(levelArray.at(0));
		contents.add(size);
		contents.add(releaseNode.asUGenInput ? -99);
		contents.add(loopNode.asUGenInput ? -99);

		size.do { arg i;
			contents.add(levelArray.at(i+1));
			contents.add(timeArray.at(i));
			contents.add(this.class.shapeNumber(curvesArray.wrapAt(i)));
			contents.add(this.curveValue(curvesArray.wrapAt(i)));
		};

		^contents.flop;
	}


}
