Signal[float] : FloatArray {

	*sineFill { arg size, amplitudes, phases;
		^Signal.newClear(size).sineFill(amplitudes, phases).normalize
	}
	*chebyFill { arg size, amplitudes, normalize=true;
		^Signal.newClear(size).chebyFill(amplitudes, normalize); //.normalizeTransfer //shouldn't normalize by default!
	}
	*hammingWindow { arg size, pad=0;
		if (pad == 0, {
			^this.newClear(size).fill(0.5).addSine(1, 0.39, -0.5pi);
		},{
			^this.newClear(size-pad).fill(0.5).addSine(1, 0.39, -0.5pi) ++ this.newClear(pad);
		});
	}
	*hanningWindow { arg size, pad=0;
		if (pad == 0, {
			^this.newClear(size).fill(0.5).addSine(1, 0.5, -0.5pi);
		},{
			^this.newClear(size-pad).fill(0.5).addSine(1, 0.5, -0.5pi) ++ this.newClear(pad);
		});
	}
	*welchWindow { arg size, pad=0;
		if (pad == 0, {
			^this.newClear(size).addSine(0.5, 1, 0);
		},{
			^this.newClear(size-pad).addSine(0.5, 1, 0) ++ this.newClear(pad);
		});
	}
	*rectWindow { arg size, pad=0;
		if (pad == 0, {
			^this.newClear(size).fill(1.0);
		},{
			^this.newClear(size-pad).fill(1.0) ++ this.newClear(pad);
		});
	}
	*readNew { arg file;
		^file.readAllSignal;
	}

	// operations
	fill { arg val; _SignalFill ^this.primitiveFailed }
	scale { arg scale; _SignalScale ^this.primitiveFailed }
	offset { arg offset; _SignalOffset ^this.primitiveFailed }

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

		var newsig = Signal.newClear((this.size-1)*2);
		var next, cur;

		cur= this[0];
		(this.size-1).do{|i|
		var index= 2*i;
		next= this[i+1];

		newsig[index]= 2*cur -next;
		newsig[index+1]= next-cur;
		cur=next;

		};

		^newsig
	}

	peak { _SignalPeak; ^this.primitiveFailed }

	normalize { arg beginSamp=0, endSamp;
		_SignalNormalize;
		^this.primitiveFailed
	}
	normalizeTransfer {
		_SignalNormalizeTransferFn;
		^this.primitiveFailed
	}

	invert { arg beginSamp=0, endSamp;
		_SignalInvert;
		^this.primitiveFailed
	}
	reverse { arg beginSamp=0, endSamp;
		_SignalReverse;
		^this.primitiveFailed
	}
	fade { arg beginSamp=0, endSamp, beginLevel=0.0, endLevel=1.0;
		_SignalFade;
		^this.primitiveFailed
	}
	rotate { arg n=1;
		_SignalRotate
		^this.primitiveFailed
	}

	zeroPad { arg minSize;
		var size = max(minSize ? 0, this.size).nextPowerOfTwo;
		^this ++ Signal.newClear(size - this.size);
	}

	integral { _SignalIntegral; ^this.primitiveFailed }
	overDub { arg aSignal, index=0;
		_SignalOverDub
		// add a signal to myself starting at the index
		// if the other signal is too long only the first part is overdubbed
		^this.primitiveFailed
	}
	overWrite { arg aSignal, index=0;
		_SignalOverWrite
		// write a signal to myself starting at the index
		// if the other signal is too long only the first part is overwritten
		^this.primitiveFailed
	}

	play { arg loop=false, mul=0.2, numChannels=1, server;
		var buf;
		buf = Buffer.sendCollection(server ? Server.default, this, numChannels, -1, { buf.play(loop, mul); });
		^buf
	}

	waveFill { arg function, start = 0.0, end = 1.0;
		var i = 0, step, size, val, x;

		// evaluate a function for every sample over the interval from
		// start to end.
		size = this.size;
		if (size <= 0, { ^this });

		x = start;
		step = (end - start) / size;
		while ({ i < size }, {
			val = function.value(x, this.at(i), i);
			this.put(i, val);
			x = x + step;
			i = i + 1;
		});
		^this
	}
	addSine { arg harmonicNumber = 1, amplitude = 1.0, phase = 0.0;
		_SignalAddHarmonic
		^this.primitiveFailed
	}
	sineFill { arg amplitudes, phases;
		this.fill(0.0);
		if (phases.isNil, { phases = #[0]; });
		amplitudes.do({ arg amp, i; this.addSine(i+1, amp, phases @@ i) });
	}
	sineFill2 { arg list;
		this.fill(0.0);
		list.do({ arg item, i;
			var harm, amp, phase;
			# harm, amp, phase = item;
			this.addSine(harm, amp ? 1.0, phase ? 0.0);
		});
	}

	addChebyshev { arg harmonicNumber = 1, amplitude = 1.0;
		_SignalAddChebyshev
		^this.primitiveFailed
	}
	chebyFill { arg amplitudes, normalize=true;
		this.fill(0.0);
		amplitudes.do({ arg amp, i; this.addChebyshev(i+1, amp); if(i%4==1,{this.offset(1)}); if(i%4==3,{this.offset(-1)}); }); //corrections for JMC DC offsets, as per Buffer:cheby

		if(normalize,{this.normalizeTransfer}); //no automatic cheby
	}

	//old version
	chebyFill_old { arg amplitudes;
		this.fill(0.0);
		amplitudes.do({ arg amp, i; this.addChebyshev(i+1, amp) });
		this.normalizeTransfer
	}


	*fftCosTable { arg fftsize;
		^this.newClear((fftsize/4) + 1).fftCosTable
	}
	fftCosTable {
		var harm;
		harm = this.size / ((this.size - 1) * 4);
		this.addSine(harm, 1, 0.5pi);
	}

	fft { arg imag, cosTable;
		// argCosTable must contain 1/4 cycle of a cosine (use fftCosTable)
		// fftsize is the next greater power of two than the receiver's length
		_Signal_FFT
		^this.primitiveFailed
	}
	ifft { arg imag, cosTable;
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

	+ { arg aNumber; _Add; ^aNumber.performBinaryOpOnSignal('+', this) }
	- { arg aNumber; _Sub; ^aNumber.performBinaryOpOnSignal('-', this) }
	* { arg aNumber; _Mul; ^aNumber.performBinaryOpOnSignal('*', this) }
	/ { arg aNumber; _FDiv; ^aNumber.performBinaryOpOnSignal('/', this) }
	mod { arg aNumber; _Mod; ^aNumber.performBinaryOpOnSignal('mod', this) }
	div { arg aNumber; _IDiv; ^aNumber.performBinaryOpOnSignal('div', this) }
	pow { arg aNumber; _Pow; ^aNumber.performBinaryOpOnSignal('pow', this) }
	min { arg aNumber; _Min; ^aNumber.performBinaryOpOnSignal('min', this) }
	max { arg aNumber; _Max; ^aNumber.performBinaryOpOnSignal('max', this) }
	ring1 { arg aNumber; _Ring1; ^aNumber.performBinaryOpOnSignal('ring1', this) }
	ring2 { arg aNumber; _Ring2; ^aNumber.performBinaryOpOnSignal('ring2', this) }
	ring3 { arg aNumber; _Ring3; ^aNumber.performBinaryOpOnSignal('ring3', this) }
	ring4 { arg aNumber; _Ring4; ^aNumber.performBinaryOpOnSignal('ring4', this) }
	difsqr { arg aNumber; _DifSqr; ^aNumber.performBinaryOpOnSignal('difsqr', this) }
	sumsqr { arg aNumber; _SumSqr; ^aNumber.performBinaryOpOnSignal('sumsqr', this) }
	sqrsum { arg aNumber; _SqrSum; ^aNumber.performBinaryOpOnSignal('sqrsum', this) }
	sqrdif { arg aNumber; _SqrDif; ^aNumber.performBinaryOpOnSignal('sqrdif', this) }
	absdif { arg aNumber; _AbsDif; ^aNumber.performBinaryOpOnSignal('absdif', this) }
	thresh { arg aNumber; _Thresh; ^aNumber.performBinaryOpOnSignal('thresh', this) }
	amclip { arg aNumber; _AMClip; ^aNumber.performBinaryOpOnSignal('amclip', this) }
	scaleneg { arg aNumber; _ScaleNeg; ^aNumber.performBinaryOpOnSignal('scaleneg', this) }
	clip2 { arg aNumber=1; _Clip2; ^aNumber.performBinaryOpOnSignal('clip2', this) }
	fold2 { arg aNumber; _Fold2; ^aNumber.performBinaryOpOnSignal('fold2', this) }
	wrap2 { arg aNumber; _Wrap2; ^aNumber.performBinaryOpOnSignal('wrap2', this) }
	excess { arg aNumber; _Excess; ^aNumber.performBinaryOpOnSignal('excess', this) }
	firstArg { arg aNumber; _FirstArg; ^aNumber.performBinaryOpOnSignal('firstArg', this) }

	== { arg aNumber; _EQ; ^aNumber.performBinaryOpOnSignal('==', this) }
	!= { arg aNumber; _NE; ^aNumber.performBinaryOpOnSignal('!=', this) }

	clip { arg lo, hi; _ClipSignal; ^this.primitiveFailed }
	wrap { arg lo, hi; _WrapSignal; ^this.primitiveFailed }
	fold { arg lo, hi; _FoldSignal; ^this.primitiveFailed }

	asInteger { _AsInt; ^this.primitiveFailed }
	asFloat { _AsFloat; ^this.primitiveFailed }
	asComplex { ^Complex.new(this, 0.0) }
	asSignal { ^this }

	// complex support
	real { ^this }
	imag { ^0.0 }

	//PRIVATE:
	performBinaryOpOnSignal { arg aSelector, aNumber, adverb;
		BinaryOpFailureError(this, aSelector, [aNumber, adverb]).throw;
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

	*sineFill { arg size, amplitudes, phases;
		^Signal.sineFill(size, amplitudes, phases).asWavetable
	}

	//size must be N/2+1 for N power of two; N is eventual size of wavetable
	*chebyFill { arg size, amplitudes, normalize=true;

		^Signal.chebyFill(size, amplitudes, normalize).asWavetableNoWrap; //asWavetable causes wrap here, problem
	}

	*chebyFill_old { arg size, amplitudes;

		//this.deprecated(thisMethod, Buffer.findRespondingMethodFor(\cheby));

		^Signal.chebyFill(size, amplitudes).asWavetable; //asWavetable causes wrap here, problem
	}

	asSignal {
		_WavetableAsSignal
		^this.primitiveFailed
	}

	blend { arg anotherWavetable, blendFrac=0.5;
		^this.asSignal.blend(anotherWavetable.asSignal, blendFrac).asWavetable;
	}

	*readNew { arg file;
		^file.readAllSignal.asWavetable;
	}
	write { arg path;
		var file;
		file = File.new(path, "wb");
		if (file.notNil, {
			file.write(this.asSignal);
			file.close;
		});
	}
	//libMenuAction { arg names;
	//	this.plot(names.last);
	//}
}
