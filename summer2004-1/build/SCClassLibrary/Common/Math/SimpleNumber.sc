SimpleNumber : Number {
		
	isValidUGenInput { ^true }
	numChannels { ^1 }
	
	magnitude { ^this.abs }
	angle { if (this >= 0, {^0.0}, {^pi} ) }
	

	neg { _Neg; ^this.primitiveFailed }
	bitNot { _BitNot; ^this.primitiveFailed }
	abs { _Abs; ^this.primitiveFailed }
	ceil { _Ceil; ^this.primitiveFailed }
	floor { _Floor; ^this.primitiveFailed }
	frac { _Frac; ^this.primitiveFailed }
	sign { _Sign; ^this.primitiveFailed }
	squared { _Squared; ^this.primitiveFailed }
	cubed { _Cubed; ^this.primitiveFailed }
	sqrt { _Sqrt; ^this.primitiveFailed }
	exp { _Exp; ^this.primitiveFailed }
	reciprocal { _Recip; ^this.primitiveFailed }
	midicps { _MIDICPS; ^this.primitiveFailed }
	cpsmidi { _CPSMIDI; ^this.primitiveFailed }
	midiratio { _MIDIRatio; ^this.primitiveFailed }
	ratiomidi { _RatioMIDI; ^this.primitiveFailed }
	ampdb { _AmpDb; ^this.primitiveFailed }
	dbamp { _DbAmp; ^this.primitiveFailed }	
	octcps { _OctCPS; ^this.primitiveFailed }
	cpsoct { _CPSOct; ^this.primitiveFailed }
	log { _Log; ^this.primitiveFailed }
	log2 { _Log2; ^this.primitiveFailed }
	log10 { _Log10; ^this.primitiveFailed }
	sin { _Sin; ^this.primitiveFailed }
	cos { _Cos; ^this.primitiveFailed }
	tan { _Tan; ^this.primitiveFailed }
	asin { _ArcSin; ^this.primitiveFailed }
	acos { _ArcCos; ^this.primitiveFailed }
	atan { _ArcTan; ^this.primitiveFailed }
	sinh { _SinH; ^this.primitiveFailed }
	cosh { _CosH; ^this.primitiveFailed }
	tanh { _TanH; ^this.primitiveFailed }
	rand { _Rand; ^this.primitiveFailed }
	rand2 { _Rand2; ^this.primitiveFailed }
	linrand { _LinRand; ^this.primitiveFailed }
	bilinrand { _BiLinRand; ^this.primitiveFailed }
	sum3rand { _Sum3Rand; ^this.primitiveFailed }

	distort { _Distort; ^this.primitiveFailed }
	softclip { _SoftClip; ^this.primitiveFailed }
	coin { _Coin; ^this.primitiveFailed }
	isPositive { ^this >= 0 }
	isNegative { ^this < 0 }
	isStrictlyPositive { ^this > 0 }

	rectWindow { _RectWindow; ^this.primitiveFailed }
	hanWindow { _HanWindow; ^this.primitiveFailed }
	welWindow { _WelchWindow; ^this.primitiveFailed }
	triWindow { _TriWindow; ^this.primitiveFailed }

	scurve { _SCurve; ^this.primitiveFailed }
	ramp { _Ramp; ^this.primitiveFailed }
	
	+ { arg aNumber, adverb; _Add; ^aNumber.performBinaryOpOnSimpleNumber('+', this, adverb) }
	- { arg aNumber, adverb; _Sub; ^aNumber.performBinaryOpOnSimpleNumber('-', this, adverb) }
	* { arg aNumber, adverb; _Mul; ^aNumber.performBinaryOpOnSimpleNumber('*', this, adverb) }
	/ { arg aNumber, adverb; _FDiv; ^aNumber.performBinaryOpOnSimpleNumber('/', this, adverb) }
	mod { arg aNumber, adverb; _Mod; ^aNumber.performBinaryOpOnSimpleNumber('mod', this, adverb) }
	div { arg aNumber, adverb; _IDiv; ^aNumber.performBinaryOpOnSimpleNumber('div', this, adverb) }
	pow { arg aNumber, adverb; _Pow; ^aNumber.performBinaryOpOnSimpleNumber('pow', this, adverb) }
	min { arg aNumber, adverb; _Min; ^aNumber.performBinaryOpOnSimpleNumber('min', this, adverb) } 
	max { arg aNumber=0.0, adverb; _Max; ^aNumber.performBinaryOpOnSimpleNumber('max', this, adverb) }
	bitAnd { arg aNumber, adverb; _BitAnd; ^aNumber.performBinaryOpOnSimpleNumber('bitAnd', this, adverb) }
	bitOr { arg aNumber, adverb; _BitOr; ^aNumber.performBinaryOpOnSimpleNumber('bitOr', this, adverb) }
	bitXor { arg aNumber, adverb; _BitXor; ^aNumber.performBinaryOpOnSimpleNumber('bitXor', this, adverb) }	
	lcm { arg aNumber, adverb; _LCM; ^aNumber.performBinaryOpOnSimpleNumber('lcm', this, adverb) }
	gcd { arg aNumber, adverb; _GCD; ^aNumber.performBinaryOpOnSimpleNumber('gcd', this, adverb) }
	round { arg aNumber=1.0, adverb; _Round; ^aNumber.performBinaryOpOnSimpleNumber('round', this, adverb) }
	roundUp { arg aNumber=1.0, adverb; _RoundUp; ^aNumber.performBinaryOpOnSimpleNumber('roundUp', this, adverb) }
	trunc { arg aNumber=1.0, adverb; _Trunc; ^aNumber.performBinaryOpOnSimpleNumber('trunc', this, adverb) }
	atan2 { arg aNumber, adverb; _Atan2; ^aNumber.performBinaryOpOnSimpleNumber('atan2', this, adverb) }	
	hypot { arg aNumber, adverb; _Hypot; ^aNumber.performBinaryOpOnSimpleNumber('hypot', this, adverb) }
	hypotApx { arg aNumber, adverb; _HypotApx; ^aNumber.performBinaryOpOnSimpleNumber('hypotApx', this, adverb) }
	leftShift { arg aNumber=1, adverb; _ShiftLeft; ^aNumber.performBinaryOpOnSimpleNumber('leftShift', this, adverb) }
	rightShift { arg aNumber=1, adverb; _ShiftRight; ^aNumber.performBinaryOpOnSimpleNumber('rightShift', this, adverb) }
	unsignedRightShift { arg aNumber, adverb; _UnsignedShift; ^aNumber.performBinaryOpOnSimpleNumber('unsignedRightShift', this, adverb) }
	ring1 { arg aNumber, adverb; _Ring1; ^aNumber.performBinaryOpOnSimpleNumber('ring1', this, adverb) }
	ring2 { arg aNumber, adverb; _Ring2; ^aNumber.performBinaryOpOnSimpleNumber('ring2', this, adverb) }
	ring3 { arg aNumber, adverb; _Ring3; ^aNumber.performBinaryOpOnSimpleNumber('ring3', this, adverb) }
	ring4 { arg aNumber, adverb; _Ring4; ^aNumber.performBinaryOpOnSimpleNumber('ring4', this, adverb) }
	difsqr { arg aNumber, adverb; _DifSqr; ^aNumber.performBinaryOpOnSimpleNumber('difsqr', this, adverb) }
	sumsqr { arg aNumber, adverb; _SumSqr; ^aNumber.performBinaryOpOnSimpleNumber('sumsqr', this, adverb) }
	sqrsum { arg aNumber, adverb; _SqrSum; ^aNumber.performBinaryOpOnSimpleNumber('sqrsum', this, adverb) }
	sqrdif { arg aNumber, adverb; _SqrDif; ^aNumber.performBinaryOpOnSimpleNumber('sqrdif', this, adverb) }
	absdif { arg aNumber, adverb; _AbsDif; ^aNumber.performBinaryOpOnSimpleNumber('absdif', this, adverb) }
	thresh { arg aNumber, adverb; _Thresh; ^aNumber.performBinaryOpOnSimpleNumber('thresh', this, adverb) }
	amclip { arg aNumber, adverb; _AMClip; ^aNumber.performBinaryOpOnSimpleNumber('amclip', this, adverb) }
	scaleneg { arg aNumber, adverb; _ScaleNeg; ^aNumber.performBinaryOpOnSimpleNumber('scaleneg', this, adverb) }
	clip2 { arg aNumber, adverb; _Clip2; ^aNumber.performBinaryOpOnSimpleNumber('clip2', this, adverb) }
	fold2 { arg aNumber, adverb; _Fold2; ^aNumber.performBinaryOpOnSimpleNumber('fold2', this, adverb) }
	wrap2 { arg aNumber, adverb; _Wrap2; ^aNumber.performBinaryOpOnSimpleNumber('wrap2', this, adverb) }

	excess { arg aNumber, adverb; _Excess; ^aNumber.performBinaryOpOnSimpleNumber('excess', this, adverb) }
	firstArg { arg aNumber, adverb; _FirstArg; ^aNumber.performBinaryOpOnSimpleNumber('firstArg', this, adverb) }
	rrand { arg aNumber, adverb; _RandRange; ^aNumber.performBinaryOpOnSimpleNumber('rrand', this, adverb) }
	exprand { arg aNumber, adverb; _ExpRandRange; ^aNumber.performBinaryOpOnSimpleNumber('exprand', this, adverb) }

	== { arg aNumber, adverb; _EQ; ^aNumber.performBinaryOpOnSimpleNumber('==', this, adverb) }
	!= { arg aNumber, adverb; _NE; ^aNumber.performBinaryOpOnSimpleNumber('!=', this, adverb) }
	< { arg aNumber, adverb; _LT; ^aNumber.performBinaryOpOnSimpleNumber('<', this, adverb) }
	> { arg aNumber, adverb; _GT; ^aNumber.performBinaryOpOnSimpleNumber('>', this, adverb) }
	<= { arg aNumber, adverb; _LE; ^aNumber.performBinaryOpOnSimpleNumber('<=', this, adverb) }
	>= { arg aNumber, adverb; _GE; ^aNumber.performBinaryOpOnSimpleNumber('>=', this, adverb) }
	equalWithPrecision { arg that,precision=0.0001;
		^absdif(this, that) < precision
	}

	hash { _ObjectHash; ^this.primitiveFailed }
		
	asInteger { _AsInt; ^this.primitiveFailed }
	asFloat { _AsFloat; ^this.primitiveFailed }
	asComplex { ^Complex.new(this, 0.0) }
	fontID { ^this }
	
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber; ^error("Math operation failed.\n") }
	performBinaryOpOnComplex { arg aSelector, aComplex, adverb; ^aComplex.perform(aSelector, this.asComplex, adverb) }
	performBinaryOpOnSignal { arg aSelector, aSignal; ^error("Math operation failed.\n") }
	performBinaryOpOnInfinitum { arg aSelector, aInf; ^error("Math operation failed.\n") }

	linlin { arg inMin, inMax, outMin, outMax;
		// linear to linear mapping
		if (this <= inMin, { ^outMin });
		if (this >= inMax, { ^outMax });
    		^(this-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
	}
	linexp { arg inMin, inMax, outMin, outMax;
		// linear to exponential mapping
		if (this <= inMin, { ^outMin });
		if (this >= inMax, { ^outMax });
		^pow(outMax/outMin, (this-inMin)/(inMax-inMin)) * outMin
	}
	explin { arg inMin, inMax, outMin, outMax;
		// exponential to linear mapping
		if (this <= inMin, { ^outMin });
		if (this >= inMax, { ^outMax });
		^(log(this/inMin)) / (log(inMax/inMin)) * (outMax-outMin) + outMin;
	}
	expexp { arg inMin, inMax, outMin, outMax;
		// exponential to exponential mapping
		if (this <= inMin, { ^outMin });
		if (this >= inMax, { ^outMax });
		^pow(outMax/outMin, log(this/inMin)) / (log(inMax/inMin)) * outMin;
	}

	bilin { arg inCenter, inMin, inMax, outCenter, outMin, outMax;
		// triangular linear mapping
		var delta;
		if (this <= inMin, { ^outMin });
		if (this >= inMax, { ^outMax });
		^if (this >= inCenter) {
			this.linlin(inCenter, inMax, outCenter, outMax);
		} {
			this.linlin(inMin, inCenter, outMin, outCenter);
		}
	}
	
	biexp { arg inCenter, inMin, inMax, outCenter, outMin, outMax;
		// triangular exponential mapping
		var delta;
		if (this <= inMin, { ^outMin });
		if (this >= inMax, { ^outMax });
		^if (this >= inCenter) {
			this.explin(inCenter, inMax, outCenter, outMax);
		} {
			this.explin(inMin, inCenter, outMin, outCenter);
		}
	}
	 
	asPoint { ^Point.new(this, this) }

	asWarp { arg spec; ^CurveWarp.new(spec, this) }

	// scheduled Routine support
	wait { ^this.yield }
	waitUntil { ^(this - thisThread.beats).yield }
	sleep {
		var thread;
		thread = thisThread;
		thread.clock.sched(this, { thread.next; nil });
		nil.yield;
	}

	printOn { arg stream;
		stream.putAll(this.asString);
	}
	storeOn { arg stream;
		stream.putAll(this.asString);
	}
	

	rate { ^'scalar' } // scalarRate constant
	
	// support for writing synth defs
	writeInputSpec { arg file, synth;
		var constIndex;
		constIndex = synth.constants.at(this.asFloat);
		if (constIndex.isNil) {
			Error("SimpleNumber-writeInputSpec constant not found: " ++ this.asFloat).throw;		};
		//[\inpspc, this.class.name, constIndex, this].postln;
		file.putInt16(-1);
		file.putInt16(constIndex);
	}
	
	series { arg second, last;
		_SimpleNumberSeries
		^this.primitiveFailed
//		var step, size;
//		second = second ?? { if (this < last) { this + 1 } { this - 1 } };
//		step = second - this;
//		size = floor((last - this) / step + 0.001).asInteger + 1;
//		^Array.series(size, this, step)
	}
	
	degreeToKey { arg scale, stepsPerOctave=12;
		var size, scaleDegree, accidental;
		size = scale.size;
		scaleDegree = this.round.asInteger;
		accidental = ((this - scaleDegree) * 10.0) * (stepsPerOctave / 12.0);
		^(stepsPerOctave * (scaleDegree div: size)) + scale.wrapAt(scaleDegree) + accidental
	}
			
	keyToDegree { arg scale, stepsPerOctave=12;
		var key, n;
		n = this div: stepsPerOctave * scale.size;
		key = this % stepsPerOctave;
		^scale.indexInBetween(key) + n
	}
	
	nearestInList { arg list;  // collection is sorted
		^list.at(list.indexIn(this))
	}
	
	nearestInScale { arg scale, stepsPerOctave=12; // collection is sorted
		var key, root;
		root = this.trunc(stepsPerOctave);
		key = this % stepsPerOctave;
		^key.nearestInList(scale) + root
	}
	
	partition { arg parts=2, min=1;
		var n;
		// randomly partition a number into parts of at least min size :
		n = this - (min - 1 * parts);		^(1..n-1).scramble.keep(parts-1).sort.add(n).differentiate + (min - 1)
	}
	
		
}
