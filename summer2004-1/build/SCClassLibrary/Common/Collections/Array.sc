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
		var list, lastIndex;
		list = [];
		lastIndex = this.lastIndex;
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
		var list, i=0, size;
		list = Array.new;
		size = this.size;
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
	
	// multiChannelExpand and flop do the same thing.
	flop {
		_ArrayMultiChannelExpand 
		^super.flop 
	}
	multiChannelExpand {
		_ArrayMultiChannelExpand 
		^super.flop 
	}

	shift { arg n;
		var fill, remain;
		fill = Array.fill(n, 0.0);
		remain = this.drop(n.neg);
		^if (n<0) { remain ++ fill } { fill ++ remain }
	}
	
	// UGen support:
	source {
		var elem;
		// returns the source UGen from an Array of OutputProxy(s)
		elem = this.at(0);
		if (elem.isKindOf(OutputProxy), {
			^elem.source
		},{
			Error("source: Not an Array of OutputProxy(s)\n").throw;
		});
	}
	isValidUGenInput { ^true }
	numChannels { ^this.size }
	
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

	// threads
	fork { arg join, clock, quant;
		var count = 0, cond;
		join = join ? this.size;
		cond = Condition({ count >= join });
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
}

