Env {
	var <levels, <times, <curves;
	var <releaseNode;	// index of release level, if nil then ignore release
	var <loopNode;	// index of loop start level, if nil then does not loop
	var <offset; 		// an offset to all time values (only works in IEnvGen)
	
	var <array;		// cache for osc-conform data
	classvar <shapeNames;

	
	*new { arg levels = #[0,1,0], times = #[1,1], curve = 'lin', releaseNode, loopNode, offset;
		times = times.asArray.clipExtend(levels.size - 1);
		^super.newCopyArgs(levels, times, curve ? 'lin', releaseNode, loopNode, offset)
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
	*xyc { arg xyc;
		var times, levels, curves, offset;
		var timeOf = { |x|
			var time = x[0];
			if(time.isSequenceableCollection) { time.minItem } { time }
		};
		xyc.sort { |a, b| timeOf.(a) < timeOf.(b) };
		#times, levels, curves = xyc.flop;
		offset = times[0];
		times = times.differentiate.drop(1);
		curves.asArray.drop(-1);
		^this.new(levels, times, curves, offset: offset);
	}
	*pairs { arg pairs, curve;
		if(curve.isNil) { ^this.xyc(pairs) };
		^this.xyc(pairs +++ curve);
	}

	range { |lo = 0.0, hi = 1.0|
		^this.copy.levels_(levels.linlin(levels.minItem, levels.maxItem, lo, hi))
	}

	exprange { |lo = 0.01, hi = 1.0|
		^this.copy.levels_(levels.linexp(levels.minItem, levels.maxItem, lo, hi))
	}

	asSignal { arg length = 400;
		^this.asMultichannelArray.collect { |chan|
			var duration, signal, ratio;
			duration = chan[5, 9 ..].sum;
			ratio = duration / (length - 1);
			signal = Signal(length);
			length.do { arg i; signal.add(chan.envAt(i * ratio)) };
			signal
		}.unbubble
	}

	discretize { arg n = 1024;
		^this.asSignal(n);
	}

	storeArgs { ^[levels, times, curves, releaseNode, loopNode] }

	== { arg that;
		^this.compareObject(that, ['levels','times','curves','releaseNode','loopNode','offset'])
	}

	hash {
		^this.instVarHash(['levels','times','curves','releaseNode','loopNode','offset'])
	}

	at { arg time;
		var array = this.asMultichannelArray;
		^if(time.isSequenceableCollection) {
			time.collect { |t, i| array.wrapAt(i).envAt(t) }
		} {
			if(array.size <= 1) { 
				array[0].envAt(time)
			} {
				array.collect(_.envAt(time))
			}
		}
	}

	// doesn't work for multichannel expanison
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
 // todo: multichannel expansion
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

	// don't cache this version for now, but instead return it directly.
	asArrayForInterpolation {
		var contents, curvesArray;
		contents = [offset ? 0, levels.at(0), times.size, times.sum];
		curvesArray = curves.asArray;
		times.size.do({ arg i;
			contents = contents ++ [
				times[i],
				this.class.shapeNumber(curvesArray.wrapAt(i)),
				this.curveValue(curvesArray.wrapAt(i)),
				levels[i+1]
			];
		});
		contents = contents.flop;
		^contents
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
		contents = contents.flop;
		^contents
	}
	
	
}
