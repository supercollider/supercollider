SimpleNumber : Number {
		
	isValidUGenInput { ^true }
	numChannels { ^1 }
	
	magnitude { ^this.abs }
	angle { if (this >= 0, {^0.0}, {^pi} ) }
	degreeToKey { arg scale, stepsPerOctave=12;
		var size, scaleDegree;
		size = scale.size;
		scaleDegree = this.asInteger;
		^(stepsPerOctave * (scaleDegree div: size)) + scale.wrapAt(scaleDegree)
	}

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
	
	+ { arg aNumber; _Add; ^aNumber.performBinaryOpOnSimpleNumber('+', this) }
	- { arg aNumber; _Sub; ^aNumber.performBinaryOpOnSimpleNumber('-', this) }
	* { arg aNumber; _Mul; ^aNumber.performBinaryOpOnSimpleNumber('*', this) }
	/ { arg aNumber; _FDiv; ^aNumber.performBinaryOpOnSimpleNumber('/', this) }
	mod { arg aNumber; _Mod; ^aNumber.performBinaryOpOnSimpleNumber('mod', this) }
	div { arg aNumber; _IDiv; ^aNumber.performBinaryOpOnSimpleNumber('div', this) }
	pow { arg aNumber; _Pow; ^aNumber.performBinaryOpOnSimpleNumber('pow', this) }
	min { arg aNumber; _Min; ^aNumber.performBinaryOpOnSimpleNumber('min', this) } 
	max { arg aNumber=0.0; _Max; ^aNumber.performBinaryOpOnSimpleNumber('max', this) }
	bitAnd { arg aNumber; _BitAnd; ^aNumber.performBinaryOpOnSimpleNumber('bitAnd', this) }
	bitOr { arg aNumber; _BitOr; ^aNumber.performBinaryOpOnSimpleNumber('bitOr', this) }
	bitXor { arg aNumber; _BitXor; ^aNumber.performBinaryOpOnSimpleNumber('bitXor', this) }	
	lcm { arg aNumber; _LCM; ^aNumber.performBinaryOpOnSimpleNumber('lcm', this) }
	gcd { arg aNumber; _GCD; ^aNumber.performBinaryOpOnSimpleNumber('gcd', this) }
	round { arg aNumber=1.0; _Round; ^aNumber.performBinaryOpOnSimpleNumber('round', this) }
	roundUp { arg aNumber=1.0; _RoundUp; ^aNumber.performBinaryOpOnSimpleNumber('roundUp', this) }
	trunc { arg aNumber=1.0; _Trunc; ^aNumber.performBinaryOpOnSimpleNumber('trunc', this) }
	atan2 { arg aNumber; _Atan2; ^aNumber.performBinaryOpOnSimpleNumber('atan2', this) }	
	hypot { arg aNumber; _Hypot; ^aNumber.performBinaryOpOnSimpleNumber('hypot', this) }
	hypotApx { arg aNumber; _HypotApx; ^aNumber.performBinaryOpOnSimpleNumber('hypotApx', this) }
	leftShift { arg aNumber=1; _ShiftLeft; ^aNumber.performBinaryOpOnSimpleNumber('leftShift', this) }
	rightShift { arg aNumber=1; _ShiftRight; ^aNumber.performBinaryOpOnSimpleNumber('rightShift', this) }
	unsignedRightShift { arg aNumber; _UnsignedShift; ^aNumber.performBinaryOpOnSimpleNumber('unsignedRightShift', this) }
	ring1 { arg aNumber; _Ring1; ^aNumber.performBinaryOpOnSimpleNumber('ring1', this) }
	ring2 { arg aNumber; _Ring2; ^aNumber.performBinaryOpOnSimpleNumber('ring2', this) }
	ring3 { arg aNumber; _Ring3; ^aNumber.performBinaryOpOnSimpleNumber('ring3', this) }
	ring4 { arg aNumber; _Ring4; ^aNumber.performBinaryOpOnSimpleNumber('ring4', this) }
	difsqr { arg aNumber; _DifSqr; ^aNumber.performBinaryOpOnSimpleNumber('difsqr', this) }
	sumsqr { arg aNumber; _SumSqr; ^aNumber.performBinaryOpOnSimpleNumber('sumsqr', this) }
	sqrsum { arg aNumber; _SqrSum; ^aNumber.performBinaryOpOnSimpleNumber('sqrsum', this) }
	sqrdif { arg aNumber; _SqrDif; ^aNumber.performBinaryOpOnSimpleNumber('sqrdif', this) }
	absdif { arg aNumber; _AbsDif; ^aNumber.performBinaryOpOnSimpleNumber('absdif', this) }
	thresh { arg aNumber; _Thresh; ^aNumber.performBinaryOpOnSimpleNumber('thresh', this) }
	amclip { arg aNumber; _AMClip; ^aNumber.performBinaryOpOnSimpleNumber('amclip', this) }
	scaleneg { arg aNumber; _ScaleNeg; ^aNumber.performBinaryOpOnSimpleNumber('scaleneg', this) }
	clip2 { arg aNumber; _Clip2; ^aNumber.performBinaryOpOnSimpleNumber('clip2', this) }
	fold2 { arg aNumber; _Fold2; ^aNumber.performBinaryOpOnSimpleNumber('fold2', this) }
	wrap2 { arg aNumber; _Wrap2; ^aNumber.performBinaryOpOnSimpleNumber('wrap2', this) }

	excess { arg aNumber; _Excess; ^aNumber.performBinaryOpOnSimpleNumber('excess', this) }
	firstArg { arg aNumber; _FirstArg; ^aNumber.performBinaryOpOnSimpleNumber('firstArg', this) }
	rrand { arg aNumber; _RandRange; ^aNumber.performBinaryOpOnSimpleNumber('rrand', this) }
	exprand { arg aNumber; _ExpRandRange; ^aNumber.performBinaryOpOnSimpleNumber('exprand', this) }

	== { arg aNumber; _EQ; ^aNumber.performBinaryOpOnSimpleNumber('==', this) }
	!= { arg aNumber; _NE; ^aNumber.performBinaryOpOnSimpleNumber('!=', this) }
	< { arg aNumber; _LT; ^aNumber.performBinaryOpOnSimpleNumber('<', this) }
	> { arg aNumber; _GT; ^aNumber.performBinaryOpOnSimpleNumber('>', this) }
	<= { arg aNumber; _LE; ^aNumber.performBinaryOpOnSimpleNumber('<=', this) }
	>= { arg aNumber; _GE; ^aNumber.performBinaryOpOnSimpleNumber('>=', this) }
	equalWithPrecision { arg that,precision=0.0001;
		^absdif(this, that) < precision
	}

	hash { _ObjectHash; ^this.primitiveFailed }
		
	asInteger { _AsInt; ^this.primitiveFailed }
	asFloat { _AsFloat; ^this.primitiveFailed }
	asComplex { ^Complex.new(this, 0.0) }
	asSignal { _AsSignal; this.primitiveFailed }
	fontID { ^this }
	
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber; ^error("Math operation failed.\n") }
	performBinaryOpOnComplex { arg aSelector, aComplex; ^aComplex.perform(aSelector, this.asComplex) }
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
		pow(outMax/outMin, log(this/inMin)) / (log(inMax/inMin)) * outMin;
	}

	asPoint { ^Point.new(this, this) }

	asWarp { arg spec; ^CurveWarp.new(spec, this) }

	// scheduled Routine support
	wait { ^this.yield }
	waitUntil { ^(this - thisThread.beats).yield }
	
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
		constIndex = synth.constants.at(this.asFloat ?? { 
			"SimpleNumber-writeInputSpec constant not found".die(synth.constants);
		});
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
	
	
	asIndex { arg list; // list is sorted
		var i, j, a, b;
		j = list.detectIndex { |item| item > this };
		if(j.isNil) { ^list.size - 1 };
		if(j == 0) { ^j };
		i = j - 1;
		^if((this - list[i]) < (list[j] - this)) { i } { j }
	}
	
	asIndex2 { arg list; // list is sorted
		var i, a, b;
		i = list.detectIndex { |item| item > this };
		if(i.isNil) { ^list.size - 1 };
		if(i == 0) { ^i };
		a = list[i-1]; b = list[i];
		^((this - a) / (b - a)) + i - 1
	}
	
	roundToList { arg list; // list is sorted
		^list.at(this.asIndex(list))
	}
	
	keyToDegree { arg scale, stepsPerOctave=12;
		var key;
		key = this % stepsPerOctave;
		^key.asIndex2(scale)
	}
	
	roundToScale { arg scale, stepsPerOctave=12;
		var key, root;
		root = this.trunc(stepsPerOctave);
		key = this % stepsPerOctave;
		^key.roundToList(scale) + root
	}
	
	
}
