Array[slot] : ArrayedCollection {

	*with { arg ... args;
		// return an array of the arguments given
		// cool! the interpreter does it for me..
		^args
	}
	reverse {
		_ArrayReverse
		^this.primitiveFailed
	}
	scramble {
		_ArrayScramble
		^this.primitiveFailed
	}
	mirror {
		_ArrayMirror
		^this.primitiveFailed
	}
	mirror1 {
		_ArrayMirror1
		^this.primitiveFailed
	}
	mirror2 {
		_ArrayMirror2
		^this.primitiveFailed
	}
	stutter { arg n=2;
		_ArrayStutter
		^this.primitiveFailed
	}
	rotate { arg n=1;
		_ArrayRotate
		^this.primitiveFailed
	}
	pyramid { arg patternType=1; // an integer from 1-10
		_ArrayPyramid
		^this.primitiveFailed
	}
	pyramidg { arg patternType=1;
		var list = [];
		var lastIndex = this.lastIndex;
		if (patternType == 1) {
			for (0,lastIndex) {|i| list = list.add(this[0..i]) };
			^list
		};
		if (patternType == 2) {
			for (0,lastIndex) {|i| list = list.add(this[lastIndex-i..lastIndex]) };
			^list
		};
		if (patternType == 3) {
			for (lastIndex,0) {|i| list = list.add(this[0..i]) };
			^list
		};
		if (patternType == 4) {
			for (0,lastIndex) {|i| list = list.add(this[i..lastIndex]) };
			^list
		};
		if (patternType == 5) {
			for (0,lastIndex)   {|i| list = list.add(this[0..i]) };
			for (lastIndex-1,0) {|i| list = list.add(this[0..i]) };
			^list
		};
		if (patternType == 6) {
			for (0,lastIndex)   {|i| list = list.add(this[lastIndex-i..lastIndex]) };
			for (lastIndex-1,0) {|i| list = list.add(this[lastIndex-i..lastIndex]) };
			^list
		};
		if (patternType == 7) {
			for (lastIndex,0) {|i| list = list.add(this[0..i]) };
			for (1,lastIndex) {|i| list = list.add(this[0..i]) };
			^list
		};
		if (patternType == 8) {
			for (0,lastIndex)   {|i| list = list.add(this[i..lastIndex]) };
			for (lastIndex-1,0) {|i| list = list.add(this[i..lastIndex]) };
			^list
		};
		if (patternType == 9) {
			for (0,lastIndex) {|i| list = list.add(this[0..i]) };
			for (1,lastIndex) {|i| list = list.add(this[i..lastIndex]) };
			^list
		};
		if (patternType == 10) {
			for (0,lastIndex)   {|i| list = list.add(this[lastIndex-i..lastIndex]) };
			for (lastIndex-1,0) {|i| list = list.add(this[0..i]) };
			^list
		};
	}
	sputter { arg probability=0.25, maxlen = 100;
		var i=0;
		var list = Array.new;
		var size = this.size;
		probability = 1.0 - probability;
		while { (i < size) and: { list.size < maxlen }}{
			list = list.add(this[i]);
			if (probability.coin) { i = i + 1; }
		};
		^list
	}

	lace { arg length;
		_ArrayLace
		^this.primitiveFailed
	}
	permute { arg nthPermutation;
		_ArrayPermute
		^this.primitiveFailed
	}
	allTuples { arg maxTuples = 16384;
		_ArrayAllTuples
		^this.primitiveFailed
	}
	wrapExtend { arg length;
		_ArrayExtendWrap
		^this.primitiveFailed
	}
	foldExtend { arg length;
		_ArrayExtendFold
		^this.primitiveFailed
	}
	clipExtend { arg length;
		_ArrayExtendLast
		^this.primitiveFailed
	}
	slide { arg windowLength=3, stepSize=1;
		_ArraySlide
		^this.primitiveFailed
	}
	containsSeqColl {
		_ArrayContainsSeqColl
		^this.primitiveFailed
	}

	//************** inconsistent argnames, see SequenceableColllection unlace!
	unlace { arg clumpSize=2, numChan=1, clip=false;
		^if(clip) {
			super.unlace(clumpSize, numChan, true)
		} {
			this.prUnlace(clumpSize, numChan) // clip not yet implemented in primitive
		}
	}
	prUnlace { arg clumpSize=2, numChan=1;
		_ArrayUnlace
		^this.primitiveFailed;
	}
	interlace { arg clumpSize=1;
		//_ArrayInterlace
		//^this.primitiveFailed;
		Error("interlace was replaced by lace\n").throw
	}
	deinterlace { arg clumpSize=2, numChan=1;
		//_ArrayUnlace
		//^this.primitiveFailed;
		Error("deinterlace was replaced by unlace\n").throw
	}

	// multiChannelExpand and flop do the same thing.
	flop {
		_ArrayMultiChannelExpand
		^super.flop
	}
	multiChannelExpand {
		_ArrayMultiChannelExpand
		^super.flop
	}
	envirPairs {
		// given an array of symbols, this returns an array of pairs of symbol, value
		// from the current environment
		var result;
		this.do {|name|
			var value = name.envirGet;
			value !? { result = result.add(name).add(value); };
		};
		^result
	}

	shift { arg n, filler = 0.0;
		var fill = Array.fill(n.abs, filler);
		var remain = this.drop(n.neg);
		^if (n<0) { remain ++ fill } { fill ++ remain }
	}

	powerset {
		var arrSize = this.size;
		var powersize = (2 ** arrSize).asInteger;
		var powersOf2 = ({ |i| 2 ** i }).dup(arrSize);

		^Array.fill(powersize, { |i|
			var elemArr = Array.new;
			powersOf2.do { |mod, j|
				if (i div: mod % 2 != 0) {
					elemArr = elemArr.add(this[j])
				};
			};
			elemArr;
		});
	}

	// UGen support:
	source {
		// returns the source UGen from an Array of OutputProxy(s)
		var elem = this.at(0);
		if (elem.isKindOf(OutputProxy), {
			^elem.source
		},{
			Error("source: Not an Array of OutputProxy(s)\n").throw;
		});
	}
	asUGenInput { arg for; ^this.collect(_.asUGenInput(for)) }
	asControlInput { ^this.collect(_.asControlInput) }

	isValidUGenInput { ^true }
	numChannels { ^this.size }

	// multichannel UGen-poll
	poll { arg trig = 10, label, trigid = -1;
		if(label.isNil){ label = this.size.collect{|index| "UGen Array [%]".format(index) } };
		^Poll(trig, this, label, trigid)
	}
	dpoll { arg label, run = 1, trigid = -1;
		if(label.isNil){ label = this.size.collect{|index| "UGen Array [%]".format(index) } };
		^Dpoll(this, label, run, trigid)
	}

	envAt { arg time;
		_ArrayEnvAt
		^this.primitiveFailed
	}

//	// 2D array support
//	*newClear2D { arg rows=1, cols=1;
//		^super.fill(rows, { Array.newClear(cols) });
//	}
//	*new2D { arg rows=1, cols=1;
//		^this.newClear2D(rows, cols);
//	}
//	at2D { arg row, col; ^this.at(row).at(col) }
//	put2D { arg row, col, val; ^this.at(row).put(col, val) }
//	fill2D { arg val;
//		this.do({ arg row;
//			row.size.do({ arg i;
//				row.put(i, val)
//			})
//		})
//	}


	// IdentitySet support
	atIdentityHash { arg argKey;
		_Array_AtIdentityHash
		^this.primitiveFailed
	}
	// IdentityDictionary support
	atIdentityHashInPairs { arg argKey;
		_Array_AtIdentityHashInPairs
		^this.primitiveFailed
	}

	asSpec { ^ControlSpec( *this ) }

	// threads
	fork { arg join (this.size), clock, quant=0.0, stackSize=64;
		var count = 0;
		var cond = Condition({ count >= join });
		this.do({ arg func;
			Routine({ arg time;
				func.value(time);
				count = count + 1;
				cond.signal;
			}).play(clock, quant);
		});
		cond.wait;
	}

	// UGen support
	madd { arg mul = 1.0, add = 0.0;
		^MulAdd(this, mul, add);
	}

	// OSC
	asRawOSC {
		_Array_OSCBytes
		^this.primitiveFailed;
	}

	printOn { arg stream;
		if (stream.atLimit, { ^this });
		stream << "[ " ;
		this.printItemsOn(stream);
		stream << " ]" ;
	}
	storeOn { arg stream;
		if (stream.atLimit, { ^this });
		stream << "[ " ;
		this.storeItemsOn(stream);
		stream << " ]" ;
	}
	prUnarchive { arg slotArray;
		slotArray.pairsDo {|index, slots| this[index].setSlots(slots) };
		this.do {|obj| obj.initFromArchive };
		^this.first
	}
}
