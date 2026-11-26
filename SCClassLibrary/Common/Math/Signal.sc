Signal[float] : FloatArray {

	*sineFill { |size, amplitudes, phases|
		^Signal.newClear(size).sineFill(amplitudes, phases).normalize
	}
	*chebyFill { |size, amplitudes, normalize = true, zeroOffset = false|
		^Signal.newClear(size).chebyFill(amplitudes, normalize, zeroOffset)
	}
	*hammingWindow_old { |size, pad = 0|
		if(pad == 0) {
			^this.newClear(size).fill(0.5).addSine(1, 0.39, -0.5pi)
		} {
			^this.newClear(size - pad).fill(0.5).addSine(1, 0.39, -0.5pi) ++ this.newClear(pad)
		}
	}
	*hammingWindow { |size, pad = 0|
		if(pad == 0) {
			^this.newClear(size).fill(0.53836).addSine(1, 0.46164, -0.5pi)
		} {
			^this.newClear(size - pad).fill(0.53836).addSine(1, 0.46164, -0.5pi) ++ this.newClear(pad)
		}
	}
	*hanningWindow { |size, pad = 0|
		if(pad == 0) {
			^this.newClear(size).fill(0.5).addSine(1, 0.5, -0.5pi)
		} {
			^this.newClear(size - pad).fill(0.5).addSine(1, 0.5, -0.5pi) ++ this.newClear(pad)
		}
	}
	*welchWindow { |size, pad = 0|
		if(pad == 0) {
			^this.newClear(size).addSine(0.5, 1, 0)
		} {
			^this.newClear(size - pad).addSine(0.5, 1, 0) ++ this.newClear(pad)
		}
	}
	*rectWindow { |size, pad = 0|
		if(pad == 0) {
			^this.newClear(size).fill(1.0)
		} {
			^this.newClear(size - pad).fill(1.0) ++ this.newClear(pad)
		}
	}
	*readNew { |file|
		^file.readAllSignal
	}

	// operations
	fill { |val| _SignalFill ^this.primitiveFailed }
	scale { |scale| _SignalScale ^this.primitiveFailed }
	offset { |offset| _SignalOffset ^this.primitiveFailed }

	asWavetable {
		// Interpolating oscillators require wavetables in a special format.
		// This method returns a wavetable in that format.
		_SignalAsWavetable;
		^this.primitiveFailed
	}

	asWavetableNoWrap {
		// Shaper requires wavetables without wrap.
		// This method returns a wavetable in that format.
		//To generate size N wavetable need N/2+1 signal values rather than N/2
		//because Buffer's add_wchebyshev calculates N/2+1 values whilst
		//Signal's _SignalAddChebyshev calculates N/2!

		var newsig = Signal.newClear((this.size - 1) * 2);
		var next, cur;

		cur = this[0];
		(this.size - 1).do { |i|
			var index = 2 * i;
			next = this[i + 1];

			newsig[index] = 2 * cur - next;
			newsig[index + 1] = next - cur;
			cur = next

		};

		^newsig
	}

	peak { _SignalPeak; ^this.primitiveFailed }

	normalize { |beginSamp = 0, endSamp|
		_SignalNormalize;
		^this.primitiveFailed
	}
	normalizeTransfer {
		_SignalNormalizeTransferFn;
		^this.primitiveFailed
	}

	invert { |beginSamp = 0, endSamp|
		_SignalInvert;
		^this.primitiveFailed
	}
	reverse { |beginSamp = 0, endSamp|
		_SignalReverse;
		^this.primitiveFailed
	}
	fade { |beginSamp = 0, endSamp, beginLevel = 0.0, endLevel = 1.0|
		_SignalFade;
		^this.primitiveFailed
	}
	rotate { |n = 1|
		_SignalRotate
		^this.primitiveFailed
	}

	zeroPad { |minSize|
		var size = max(minSize ? 0, this.size).nextPowerOfTwo;
		^this ++ Signal.newClear(size - this.size)
	}

	integral { _SignalIntegral; ^this.primitiveFailed }
	overDub { |aSignal, index = 0|
		_SignalOverDub
		// add a signal to myself starting at the index
		// if the other signal is too long only the first part is overdubbed
		^this.primitiveFailed
	}
	overWrite { |aSignal, index = 0|
		_SignalOverWrite
		// write a signal to myself starting at the index
		// if the other signal is too long only the first part is overwritten
		^this.primitiveFailed
	}

	play { |loop = false, mul = 0.2, numChannels = 1, server|
		var buf;
		buf = Buffer.sendCollection(server ? Server.default, this, numChannels, -1, { buf.play(loop, mul) });
		^buf
	}

	waveFill { |function, start = 0.0, end = 1.0|
		var i = 0, step, size, val, x;

		// evaluate a function for every sample over the interval from
		// start to end.
		size = this.size;
		if(size <= 0) { ^this };

		x = start;
		step = (end - start) / size;
		while { i < size } {
			val = function.value(x, this.at(i), i);
			this.put(i, val);
			x = x + step;
			i = i + 1
		};
		^this
	}
	addSine { |harmonicNumber = 1, amplitude = 1.0, phase = 0.0|
		_SignalAddHarmonic
		^this.primitiveFailed
	}
	sineFill { |amplitudes, phases|
		this.fill(0.0);
		if(phases.isNil) { phases = #[0] };
		amplitudes.do { |amp, i| this.addSine(i + 1, amp, phases @@ i) }
	}
	sineFill2 { |list|
		this.fill(0.0);
		list.do { |item, i|
			var harm, amp, phase;
			# harm, amp, phase = item;
			this.addSine(harm, amp ? 1.0, phase ? 0.0)
		}
	}

	addChebyshev { |harmonicNumber = 1, amplitude = 1.0|
		_SignalAddChebyshev
		^this.primitiveFailed
	}
	chebyFill { |amplitudes, normalize = true, zeroOffset = false|
		this.fill(0.0);
		amplitudes.do { |amp, i|
			this.addChebyshev(i + 1, amp);
			if(zeroOffset) {
				if(i % 4 == 1) { this.offset(amp) };
				if(i % 4 == 3) { this.offset(-1 * amp) }
			} //corrections for JMC DC offsets, as per Buffer:cheby
		};

		if(normalize) {
			if(zeroOffset) {
				this.normalizeTransfer
			} {
				this.normalize
			}
		}
	}

	//old version
	chebyFill_old { |amplitudes|
		this.fill(0.0);
		amplitudes.do { |amp, i| this.addChebyshev(i + 1, amp) };
		this.normalizeTransfer
	}


	*fftCosTable { |fftsize|
		^this.newClear((fftsize / 4) + 1).fftCosTable
	}
	fftCosTable {
		var harm;
		harm = this.size / ((this.size - 1) * 4);
		this.addSine(harm, 1, 0.5pi)
	}

	fft { |imag, cosTable|
		// argCosTable must contain 1/4 cycle of a cosine (use fftCosTable)
		// fftsize is the next greater power of two than the receiver's length
		_Signal_FFT
		^this.primitiveFailed
	}
	ifft { |imag, cosTable|
		// argCosTable must contain 1/4 cycle of a cosine (use fftCosTable)
		// fftsize is the next greater power of two than the receiver's length
		_Signal_IFFT
		^this.primitiveFailed
	}

	neg { _Neg; ^this.primitiveFailed }
	abs { _Abs; ^this.primitiveFailed }
	sign { _Sign; ^this.primitiveFailed }
	squared { _Squared; ^this.primitiveFailed }
	cubed { _Cubed; ^this.primitiveFailed }
	sqrt { _Sqrt; ^this.primitiveFailed }
	exp { _Exp; ^this.primitiveFailed }
	//reciprocal { _Recip; ^this.primitiveFailed }
	//midicps { _MIDICPS; ^this.primitiveFailed }
	//cpsmidi { _CPSMIDI; ^this.primitiveFailed }
	//midiratio { _MIDIRatio; ^this.primitiveFailed }
	//ratiomidi { _RatioMIDI; ^this.primitiveFailed }
	//ampdb { _AmpDb; ^this.primitiveFailed }
	//dbamp { _DbAmp; ^this.primitiveFailed }
	//octcps { _OctCPS; ^this.primitiveFailed }
	//cpsoct { _CPSOct; ^this.primitiveFailed }
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
	distort { _Distort; ^this.primitiveFailed }
	softclip { _SoftClip; ^this.primitiveFailed }

	rectWindow { _RectWindow; ^this.primitiveFailed }
	hanWindow { _HanWindow; ^this.primitiveFailed }
	welWindow { _WelchWindow; ^this.primitiveFailed }
	triWindow { _TriWindow; ^this.primitiveFailed }

	scurve { _SCurve; ^this.primitiveFailed }
	ramp { _Ramp; ^this.primitiveFailed }

	+ { |aNumber, adverb| _Add; ^aNumber.performBinaryOpOnSignal('+', this) }
	- { |aNumber, adverb| _Sub; ^aNumber.performBinaryOpOnSignal('-', this) }
	* { |aNumber, adverb| _Mul; ^aNumber.performBinaryOpOnSignal('*', this) }
	/ { |aNumber, adverb| _FDiv; ^aNumber.performBinaryOpOnSignal('/', this) }
	mod { |aNumber, adverb| _Mod; ^aNumber.performBinaryOpOnSignal('mod', this) }
	modSeaside { |aNumber, adverb| _Mod; ^aNumber.performBinaryOpOnSignal('mod', this) }
	div { |aNumber, adverb| _IDiv; ^aNumber.performBinaryOpOnSignal('div', this) }
	pow { |aNumber, adverb| _Pow; ^aNumber.performBinaryOpOnSignal('pow', this) }
	min { |aNumber, adverb| _Min; ^aNumber.performBinaryOpOnSignal('min', this) }
	max { |aNumber, adverb| _Max; ^aNumber.performBinaryOpOnSignal('max', this) }
	ring1 { |aNumber, adverb| _Ring1; ^aNumber.performBinaryOpOnSignal('ring1', this) }
	ring2 { |aNumber, adverb| _Ring2; ^aNumber.performBinaryOpOnSignal('ring2', this) }
	ring3 { |aNumber, adverb| _Ring3; ^aNumber.performBinaryOpOnSignal('ring3', this) }
	ring4 { |aNumber, adverb| _Ring4; ^aNumber.performBinaryOpOnSignal('ring4', this) }
	difsqr { |aNumber, adverb| _DifSqr; ^aNumber.performBinaryOpOnSignal('difsqr', this) }
	sumsqr { |aNumber, adverb| _SumSqr; ^aNumber.performBinaryOpOnSignal('sumsqr', this) }
	sqrsum { |aNumber, adverb| _SqrSum; ^aNumber.performBinaryOpOnSignal('sqrsum', this) }
	sqrdif { |aNumber, adverb| _SqrDif; ^aNumber.performBinaryOpOnSignal('sqrdif', this) }
	absdif { |aNumber, adverb| _AbsDif; ^aNumber.performBinaryOpOnSignal('absdif', this) }
	thresh { |aNumber, adverb| _Thresh; ^aNumber.performBinaryOpOnSignal('thresh', this) }
	amclip { |aNumber, adverb| _AMClip; ^aNumber.performBinaryOpOnSignal('amclip', this) }
	scaleneg { |aNumber, adverb| _ScaleNeg; ^aNumber.performBinaryOpOnSignal('scaleneg', this) }
	clip2 { |aNumber = 1, adverb| _Clip2; ^aNumber.performBinaryOpOnSignal('clip2', this) }
	fold2 { |aNumber = 1, adverb| _Fold2; ^aNumber.performBinaryOpOnSignal('fold2', this) }
	wrap2 { |aNumber = 1, adverb| _Wrap2; ^aNumber.performBinaryOpOnSignal('wrap2', this) }
	excess { |aNumber = 1, adverb| _Excess; ^aNumber.performBinaryOpOnSignal('excess', this) }
	firstArg { |aNumber, adverb| _FirstArg; ^aNumber.performBinaryOpOnSignal('firstArg', this) }

	== { |aNumber, adverb| _EQ; ^aNumber.performBinaryOpOnSignal('==', this) }
	!= { |aNumber, adverb| _NE; ^aNumber.performBinaryOpOnSignal('!=', this) }

	clip { |lo, hi| _ClipSignal; ^this.primitiveFailed }
	wrap { |lo, hi| _WrapSignal; ^this.primitiveFailed }
	fold { |lo, hi| _FoldSignal; ^this.primitiveFailed }
	blend { |that, blendFrac = 0.5|
		^this + (blendFrac * (that - this))
	}

	asInteger { _AsInteger; ^this.primitiveFailed }
	asFloat { _AsFloat; ^this.primitiveFailed }
	asComplex { ^Complex.new(this, 0.0) }
	asSignal { ^this }

	// complex support
	real { ^this }
	imag { ^0.0 }

	//PRIVATE:
	performBinaryOpOnSignal { |aSelector, aNumber, adverb|
		BinaryOpFailureError(this, aSelector, [aNumber, adverb]).throw
	}
}

Wavetable[float] : FloatArray {
	// the only way to make a Wavetable is by Signal::asWavetable
	*new {
		^this.shouldNotImplement(thisMethod)
	}
	*newClear {
		^this.shouldNotImplement(thisMethod)
	}

	*sineFill { |size, amplitudes, phases|
		^Signal.sineFill(size, amplitudes, phases).asWavetable
	}

	//size must be N/2+1 for N power of two; N is eventual size of wavetable
	*chebyFill { |size, amplitudes, normalize = true, zeroOffset = false|

		^Signal.chebyFill(size, amplitudes, normalize, zeroOffset).asWavetableNoWrap //asWavetable causes wrap here, problem
	}

	*chebyFill_old { |size, amplitudes|

		//this.deprecated(thisMethod, Buffer.findRespondingMethodFor(\cheby));

		^Signal.chebyFill(size, amplitudes).asWavetable //asWavetable causes wrap here, problem
	}

	asSignal {
		_WavetableAsSignal
		^this.primitiveFailed
	}

	blend { |anotherWavetable, blendFrac = 0.5|
		^this.asSignal.blend(anotherWavetable.asSignal, blendFrac).asWavetable
	}

	*readNew { |file|
		^file.readAllSignal.asWavetable
	}
	write { |path|
		var file;
		file = File.new(path, "wb");
		if(file.notNil) {
			file.write(this.asSignal);
			file.close
		}
	}
	//libMenuAction { arg names;
	//	this.plot(names.last);
	//}
}
