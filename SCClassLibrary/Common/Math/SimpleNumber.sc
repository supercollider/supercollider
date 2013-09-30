SimpleNumber : Number {

	*new { ^this.shouldNotImplement(thisMethod) }

	isValidUGenInput { ^this.isNaN.not }
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
	isNaN { ^(this >= 0 or: { this <= 0 }).not }
	asBoolean    { ^this > 0 }
	booleanValue { ^this > 0 } // TODO in the long-run, deprecate for asBoolean
	binaryValue { ^this.sign.max(0) } // TODO in the long-run, deprecate for asInteger

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
	bitHammingDistance { arg aNumber, adverb; _HammingDistance  ^aNumber.performBinaryOpOnSimpleNumber('hammingDistance', this, adverb) }
	bitTest { arg bit; ^( (this & 1.leftShift(bit)) != 0) }
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

	== { arg aNumber, adverb; _EQ; ^aNumber.perform('==', this, adverb) }
	!= { arg aNumber, adverb; _NE; ^aNumber.perform('!=', this, adverb) }
	< { arg aNumber, adverb; _LT; ^aNumber.performBinaryOpOnSimpleNumber('<', this, adverb) }
	> { arg aNumber, adverb; _GT; ^aNumber.performBinaryOpOnSimpleNumber('>', this, adverb) }
	<= { arg aNumber, adverb; _LE; ^aNumber.performBinaryOpOnSimpleNumber('<=', this, adverb) }
	>= { arg aNumber, adverb; _GE; ^aNumber.performBinaryOpOnSimpleNumber('>=', this, adverb) }

	equalWithPrecision { arg that, precision=0.0001;
		^absdif(this, that) < precision
	}

	hash { _ObjectHash; ^this.primitiveFailed }

	asInteger { _AsInt; ^this.primitiveFailed }
	asFloat { _AsFloat; ^this.primitiveFailed }
	asComplex { ^Complex.new(this, 0.0) }
	asRect { ^Rect(this, this, this, this) }

	degrad { ^this*pi/180 }
	raddeg { ^this*180/pi }

	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		 BinaryOpFailureError(this, aSelector, [aNumber, adverb]).throw;
	}
	performBinaryOpOnComplex { arg aSelector, aComplex, adverb; ^aComplex.perform(aSelector, this.asComplex, adverb) }
	performBinaryOpOnSignal { arg aSelector, aSignal, adverb;
		BinaryOpFailureError(this, aSelector, [aSignal, adverb]).throw;
	}

	nextPowerOfTwo { ^this.nextPowerOf(2) }
	nextPowerOf { arg base; ^pow(base, ceil(log(this) / log(base))) }
	nextPowerOfThree { ^pow(3, ceil(log(this) / log(3))) }
	previousPowerOf { arg base; ^pow(base, ceil(log(this) / log(base)) - 1) }

	quantize { arg quantum = 1.0, tolerance = 0.05, strength = 1.0;
		var round = round(this, quantum);
		var diff = round - this;
		if (abs(diff) < tolerance) {
			^this + (strength * diff)
		}{
			^this
		}
	}


	linlin { arg inMin, inMax, outMin, outMax, clip=\minmax;
		// linear to linear mapping
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
    		^(this-inMin)/(inMax-inMin) * (outMax-outMin) + outMin;
	}

	linexp { arg inMin, inMax, outMin, outMax, clip=\minmax;
		// linear to exponential mapping
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
		^pow(outMax/outMin, (this-inMin)/(inMax-inMin)) * outMin
	}

	explin { arg inMin, inMax, outMin, outMax, clip=\minmax;
		// exponential to linear mapping
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
		^(log(this/inMin)) / (log(inMax/inMin)) * (outMax-outMin) + outMin;
	}

	expexp { arg inMin, inMax, outMin, outMax, clip=\minmax;
		// exponential to exponential mapping
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
		^pow(outMax/outMin, log(this/inMin) / log(inMax/inMin)) * outMin;
	}

	lincurve { arg inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax;
		var grow, a, b, scaled;
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
		if (abs(curve) < 0.001) { ^this.linlin(inMin, inMax, outMin, outMax) };

		grow = exp(curve);
		a = outMax - outMin / (1.0 - grow);
		b = outMin + a;
		scaled = (this - inMin) / (inMax - inMin);

		^b - (a * pow(grow, scaled));
	}

	curvelin { arg inMin = 0, inMax = 1, outMin = 0, outMax = 1, curve = -4, clip = \minmax;
		var grow, a, b, scaled;
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
		if (abs(curve) < 0.001) { ^this.linlin(inMin, inMax, outMin, outMax) };

		grow = exp(curve);
		a = inMax - inMin / (1.0 - grow);
		b = inMin + a;

		^log((b - this) / a) * (outMax - outMin) / curve + outMin
	}

	bilin { arg inCenter, inMin, inMax, outCenter, outMin, outMax, clip=\minmax;
		// triangular linear mapping
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
		^if (this >= inCenter) {
			this.linlin(inCenter, inMax, outCenter, outMax, \none);
		} {
			this.linlin(inMin, inCenter, outMin, outCenter, \none);
		}
	}

	biexp { arg inCenter, inMin, inMax, outCenter, outMin, outMax, clip=\minmax;
		// triangular exponential mapping
		switch(clip,
			\minmax, {
				if (this <= inMin, { ^outMin });
				if (this >= inMax, { ^outMax });
			},
			\min, {
				if (this <= inMin, { ^outMin });
			},
			\max, {
				if (this >= inMax, { ^outMax });
			}
		);
		if (this >= inCenter) {
			this.explin(inCenter, inMax, outCenter, outMax, \none);
		} {
			this.explin(inMin, inCenter, outMin, outCenter, \none);
		}
	}
	lcurve { arg a = 1.0, m = 0.0, n = 1.0, tau = 1.0;
		var rTau, x = this.neg;
		^if(tau == 1.0) {
			a * (m * exp(x) + 1) / (n * exp(x) + 1)
		} {
			rTau = tau.reciprocal;
			a * (m * exp(x) * rTau + 1) / (n * exp(x) * rTau + 1)
		}
	}
	gauss { arg standardDeviation;
		^((((-2*log(1.0.rand)).sqrt * sin(2pi.rand)) * standardDeviation) + this)
	}
	gaussCurve { arg a = 1.0, b = 0.0, c = 1.0;
		^a * (exp(squared(this - b) / (-2.0 * squared(c))))
	}

	asPoint { ^Point.new(this, this) }

	asWarp { arg spec; ^CurveWarp.new(spec, this) }

	// scheduled Routine support
	wait { ^this.yield }
	waitUntil { ^(this - thisThread.beats).max(0).yield }
	sleep {
		var thread = thisThread;
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
	asAudioRateInput { ^if(this == 0) { Silent.ar } { DC.ar(this) } }

	madd   { arg mul, add; ^(this * mul) + add; }

	lag    { ^this }
	lag2   { ^this }
	lag3   { ^this }
	lagud  { ^this }
	lag2ud { ^this }
	lag3ud { ^this }
	varlag { ^this }
	slew   { ^this }

	// support for writing synth defs
	writeInputSpec { arg file, synth;
		var constIndex = synth.constants.at(this.asFloat);
		if (constIndex.isNil) {
			Error("SimpleNumber-writeInputSpec constant not found: " ++ this.asFloat).throw;
		};
		file.putInt32(-1);
		file.putInt32(constIndex);
	}

	series { arg second, last;
		_SimpleNumberSeries
		^this.primitiveFailed
		/* var step, size;
		second = second ?? { if (this < last) { this + 1 } { this - 1 } };
		step = second - this;
		size = floor((last - this) / step + 0.001).asInteger + 1;
		^Array.series(size, this, step) */
	}
	seriesIter { arg second, last;
		var step, size;
		if (second.isNil) {
			last = last ? inf;
			step = if (this < last, 1, -1);
		}{
			last ?? { last = if (second < this, -inf, inf) };
			step = second - this;
		};
		^if (step < 0) {
			r {
				var val = this;
				while {
					val >= last;
				}{
					val.yield;
					val = val + step;
				};
			}
		}{
			r {
				var val = this;
				while {
					val <= last;
				}{
					val.yield;
					val = val + step;
				};
			}
		}
	}


	degreeToKey { arg scale, stepsPerOctave = 12;
		var scaleDegree = this.round.asInteger;
		var accidental = (this - scaleDegree) * 10.0;
		^scale.performDegreeToKey(scaleDegree, stepsPerOctave, accidental)
	}

	keyToDegree { arg scale, stepsPerOctave=12;
		^scale.performKeyToDegree(this, stepsPerOctave)
	}

	nearestInList { arg list;  // collection is sorted
		^list.performNearestInList(this);
	}

	nearestInScale { arg scale, stepsPerOctave=12; // collection is sorted
		^scale.performNearestInScale(this, stepsPerOctave);
	}

	partition { arg parts=2, min=1;
		// randomly partition a number into parts of at least min size :
		var n = this - (min - 1 * parts);
		^(1..n-1).scramble.keep(parts-1).sort.add(n).differentiate + (min - 1)
	}

	nextTimeOnGrid { arg clock;
		^clock.nextTimeOnGrid(this, 0);
	}

	playAndDelta {}

	asQuant { ^Quant(this) }

	// a clock format inspired by ISO 8601 time interval display (truncated representation)
	// receiver is a time in seconds, returns string "ddd:hh:mm:ss:ttt" where t is milliseconds
	// see String:asSecs for complement

	asTimeString { arg precision = 0.001, maxDays = 365, dropDaysIfPossible = true;
		var decimal, days, hours, minutes, seconds, mseconds;
		decimal = this.asInteger;
		days = decimal.div(86400).min(maxDays);
		days = if(dropDaysIfPossible and: { days == 0 }) {
			days = ""
		} {
			days.asString.padLeft(3, "0").add($:);
		};
		hours = (decimal.div(3600) % 24).asString.padLeft(2, "0").add($:);
		minutes = (decimal.div(60) % 60).asString.padLeft(2, "0").add($:);
		seconds = (decimal % 60).asString.padLeft(2, "0").add($:);
		mseconds = (this.frac / precision).round(precision).asInteger.asString.padLeft(3, "0");
		^days ++ hours ++ minutes ++ seconds ++ mseconds
	}

	asFraction {|denominator=100, fasterBetter=true|
		_AsFraction
		// asFraction will return a fraction that is the best approximation up to the given
		// denominator.
		// if fasterBetter is true it may find a much closer approximation and do it faster.
		^this.primitiveFailed
	}
	prSimpleNumberSeries { arg second, last;
		_SimpleNumberSeries
		^this.primitiveFailed
	}

	asBufWithValues {
		^this
	}

	schedBundleArrayOnClock { |clock, bundleArray, lag = 0, server, latency|
		clock.sched(this, {
					if (lag != 0) {
						SystemClock.sched(lag, {
							server.sendBundle(latency ? server.latency, *bundleArray)
						})
					} {
						server.sendBundle(latency ? server.latency, *bundleArray)
					}
		})
	}
}
