{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red96\green96\blue96;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Signal\cf0 [float] : \cf2 FloatArray\cf0  \{\
	\
	*sineFill \{ \cf2 arg\cf0  size, amplitudes, phases;\
		^\cf2 Signal\cf0 .newClear(size).sineFill(amplitudes, phases).normalize\
	\}\
	*chebyFill \{ \cf2 arg\cf0  size, amplitudes;\
		^\cf2 Signal\cf0 .newClear(size).chebyFill(amplitudes).normalizeTransfer\
	\}\
	*hammingWindow \{ \cf2 arg\cf0  size, pad=0;\
		if (pad == 0, \{\
			^\cf2 this\cf0 .newClear(size).fill(0.5).addSine(1, 0.39, -0.5pi);\
		\},\{\
			^\cf2 this\cf0 .newClear(size-pad).fill(0.5).addSine(1, 0.39, -0.5pi) ++ \cf2 this\cf0 .newClear(pad);\
		\});\
	\}\
	*hanningWindow \{ \cf2 arg\cf0  size, pad=0;\
		if (pad == 0, \{\
			^\cf2 this\cf0 .newClear(size).fill(0.5).addSine(1, 0.5, -0.5pi);\
		\},\{\
			^\cf2 this\cf0 .newClear(size-pad).fill(0.5).addSine(1, 0.5, -0.5pi) ++ \cf2 this\cf0 .newClear(pad);\
		\});\
	\}\
	*welchWindow \{ \cf2 arg\cf0  size, pad=0;\
		if (pad == 0, \{\
			^\cf2 this\cf0 .newClear(size).addSine(0.5, 1, 0);\
		\},\{\
			^\cf2 this\cf0 .newClear(size-pad).addSine(0.5, 1, 0) ++ \cf2 this\cf0 .newClear(pad);\
		\});\
	\}\
	*rectWindow \{ \cf2 arg\cf0  size, pad=0;\
		if (pad == 0, \{\
			^\cf2 this\cf0 .newClear(size).fill(1.0);\
		\},\{\
			^\cf2 this\cf0 .newClear(size-pad).fill(1.0) ++ \cf2 this\cf0 .newClear(pad);\
		\});\
	\}\
	*readNew \{ \cf2 arg\cf0  file;\
		^file.readAllSignal;\
	\}\
\
	\cf3 // operations\
\cf0 	fill \{ \cf2 arg\cf0  val; \cf2 _SignalFill\cf0  ^\cf2 this\cf0 .primitiveFailed \}\
	scale \{ \cf2 arg\cf0  scale; \cf2 _SignalScale\cf0  ^\cf2 this\cf0 .primitiveFailed \}\
	offset \{ \cf2 arg\cf0  offset; \cf2 _SignalOffset\cf0  ^\cf2 this\cf0 .primitiveFailed \}\
	\
	asWavetable \{ \
		\cf3 // Interpolating oscillators require wavetables in a special format.\
\cf0 		\cf3 // This method returns a wavetable in that format.\
\cf0 		\cf2 _SignalAsWavetable\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	\
	peak \{ \cf2 _SignalPeak\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	\
	normalize \{ \cf2 arg\cf0  beginSamp=0, endSamp;\
		\cf2 _SignalNormalize\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	normalizeTransfer \{\
		\cf2 _SignalNormalizeTransferFn\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	\
	invert \{ \cf2 arg\cf0  beginSamp=0, endSamp;\
		\cf2 _SignalInvert\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	reverse \{ \cf2 arg\cf0  beginSamp=0, endSamp;\
		\cf2 _SignalReverse\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	fade \{ \cf2 arg\cf0  beginSamp=0, endSamp, beginLevel=0.0, endLevel=1.0; \
		\cf2 _SignalFade\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	rotate \{ \cf2 arg\cf0  n=1;\
		\cf2 _SignalRotate\cf0  \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	zeroPad \{\
		\cf2 var\cf0  size;\
		size = (\cf2 this\cf0 .size.nextPowerOfTwo * 2);\
		^\cf2 this\cf0  ++ \cf2 Signal\cf0 .newClear(size - \cf2 this\cf0 .size);\
	\}\
	\
	integral \{ \cf2 _SignalIntegral\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	overDub \{ \cf2 arg\cf0  aSignal, index; \
		\cf2 _SignalOverDub\cf0  \
		\cf3 // add a signal to myself starting at the index\
\cf0 		\cf3 // if the other signal is too long only the first part is overdubbed\
\cf0 		^\cf2 this\cf0 .primitiveFailed \
	\}\
	overWrite \{ \cf2 arg\cf0  aSignal, index; \
		\cf2 _SignalOverWrite\cf0  \
		\cf3 // write a signal to myself starting at the index\
\cf0 		\cf3 // if the other signal is too long only the first part is overwritten\
\cf0 		^\cf2 this\cf0 .primitiveFailed \
	\}\
\cf3 //	asciiPlot \{\
//		// draw the waveform down the page as asterisks\
//		var lo, hi, scale, pt;\
//		lo = this.minItem; \
//		hi = this.maxItem;\
//		scale = 72 / (hi - lo);\
//		this.size.do(\{ arg i;\
//			pt = ((this.at(i) - lo) * scale).asInteger;\
//			pt.do(\{ " ".post; \});\
//			"*\\n".post;\
//		\});\
//	\}\
\cf0 	plot \{ \cf2 arg\cf0  name, bounds;\
		\cf3 // open a window\
\cf0 		name = name ? \cf4 "signal"\cf0 ;\
		bounds = bounds ? \cf2 Rect\cf0 .new(40,80,600,380);\
		\cf2 SignalWindow\cf0 .new(name, bounds, \cf2 this\cf0 );\
	\}\
	play \{ \cf2 arg\cf0  sampleRate, name=\cf5 \\Signal\cf0 , loop = \cf2 true\cf0 ;\
		\cf2 var\cf0  playbackRate = 1.0;\
		if (sampleRate.isNil, \{ \
			sampleRate = \cf2 Synth\cf0 .sampleRate \
		\},\{\
			playbackRate = sampleRate / \cf2 Synth\cf0 .sampleRate;\
		\});\
		\cf2 Instrument\cf0 .play(name.asSymbol, \{ \cf2 arg\cf0  rate = 1.0;\
			if (loop, \{\
				\cf2 PlayBuf\cf0 .ar(\cf2 this\cf0 , sampleRate, playbackRate * rate, 0, 0, \cf2 this\cf0 .size-2)\
			\},\{\
				\cf2 PlayBuf\cf0 .ar(\cf2 this\cf0 , sampleRate, playbackRate * rate, 0, \cf2 this\cf0 .size-2, \cf2 this\cf0 .size-2)\
			\}); \
		\}, 1.0);\
	\}\
	\
	waveFill \{ \cf2 arg\cf0  function, start = 0.0, end = 1.0;\
		\cf2 var\cf0  i = 0, step, size, val, x;\
		\
		\cf3 // evaluate a function for every sample over the interval from\
\cf0 		\cf3 // start to end. \
\cf0 		size = \cf2 this\cf0 .size;\
		if (size <= 0, \{ ^\cf2 this\cf0  \});\
		\
		x = start;\
		step = (end - start) / size;\
		while (\{ i < size \}, \{\
			val = function.value(x, \cf2 this\cf0 .at(i), i);\
			\cf2 this\cf0 .put(i, val);\
			x = x + step;\
			i = i + 1;\
		\});\
		^\cf2 this\cf0 \
	\}\
	addSine \{ \cf2 arg\cf0  harmonicNumber = 1, amplitude = 1.0, phase = 0.0;\
		\cf2 _SignalAddHarmonic\cf0 \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	sineFill \{ \cf2 arg\cf0  amplitudes, phases;\
		\cf2 this\cf0 .fill(0.0);\
		if (phases.isNil, \{ phases = #[0]; \});\
		amplitudes.do(\{ \cf2 arg\cf0  amp, i; \cf2 this\cf0 .addSine(i+1, amp, phases @@ i) \});\
	\}\
	sineFill2 \{ \cf2 arg\cf0  list;\
		\cf2 this\cf0 .fill(0.0);\
		list.do(\{ \cf2 arg\cf0  item, i; \
			\cf2 var\cf0  harm, amp, phase;\
			# harm, amp, phase = item;\
			\cf2 this\cf0 .addSine(harm, amp, phase);\
		\});\
	\}\
	\
	addChebyshev \{ \cf2 arg\cf0  harmonicNumber = 1, amplitude = 1.0;\
		\cf2 _SignalAddChebyshev\cf0 \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	chebyFill \{ \cf2 arg\cf0  amplitudes;\
		\cf2 this\cf0 .fill(0.0);\
		amplitudes.do(\{ \cf2 arg\cf0  amp, i; \cf2 this\cf0 .addChebyshev(i+1, amp) \});\
		\cf2 this\cf0 .normalizeTransfer\
	\}\
\
	\
	*fftCosTable \{ \cf2 arg\cf0  fftsize;\
		^\cf2 this\cf0 .newClear((fftsize/4) + 1).fftCosTable\
	\}\
	fftCosTable \{\
		\cf2 var\cf0  harm;\
		harm = \cf2 this\cf0 .size / ((\cf2 this\cf0 .size - 1) * 4);\
		\cf2 this\cf0 .addSine(harm, 1, 0.5pi);\
	\}\
	\
	fft \{ \cf2 arg\cf0  imag, cosTable; \
		\cf3 // argCosTable must contain 1/4 cycle of a cosine (use fftCosTable)\
\cf0 		\cf3 // fftsize is the next greater power of two than the receiver's length\
\cf0 		\cf2 _Signal_FFT\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	ifft \{ \cf2 arg\cf0  imag, cosTable;\
		\cf3 // argCosTable must contain 1/4 cycle of a cosine (use fftCosTable)\
\cf0 		\cf3 // fftsize is the next greater power of two than the receiver's length\
\cf0 		\cf2 _Signal_IFFT\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	\
	neg \{ \cf2 _Neg\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	abs \{ \cf2 _Abs\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	sign \{ \cf2 _Sign\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	squared \{ \cf2 _Squared\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	cubed \{ \cf2 _Cubed\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	sqrt \{ \cf2 _Sqrt\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	exp \{ \cf2 _Exp\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	\cf3 //reciprocal \{ _Recip; ^this.primitiveFailed \}\
\cf0 	\cf3 //midicps \{ _MIDICPS; ^this.primitiveFailed \}\
\cf0 	\cf3 //cpsmidi \{ _CPSMIDI; ^this.primitiveFailed \}\
\cf0 	\cf3 //midiratio \{ _MIDIRatio; ^this.primitiveFailed \}\
\cf0 	\cf3 //ratiomidi \{ _RatioMIDI; ^this.primitiveFailed \}\
\cf0 	\cf3 //ampdb \{ _AmpDb; ^this.primitiveFailed \}\
\cf0 	\cf3 //dbamp \{ _DbAmp; ^this.primitiveFailed \}	\
\cf0 	\cf3 //octcps \{ _OctCPS; ^this.primitiveFailed \}\
\cf0 	\cf3 //cpsoct \{ _CPSOct; ^this.primitiveFailed \}\
\cf0 	log \{ \cf2 _Log\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	log2 \{ \cf2 _Log2\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	log10 \{ \cf2 _Log10\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	sin \{ \cf2 _Sin\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	cos \{ \cf2 _Cos\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	tan \{ \cf2 _Tan\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	asin \{ \cf2 _ArcSin\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	acos \{ \cf2 _ArcCos\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	atan \{ \cf2 _ArcTan\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	sinh \{ \cf2 _SinH\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	cosh \{ \cf2 _CosH\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	tanh \{ \cf2 _TanH\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	distort \{ \cf2 _Distort\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	softclip \{ \cf2 _SoftClip\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	nyqring \{ \cf2 _NyqRing\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	isPositive \{ \cf2 _IsPositive\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	isNegative \{ \cf2 _IsNegative\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	isStrictlyPositive \{ \cf2 _IsStrictlyPos\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}	\
\
	rectWindow \{ \cf2 _RectWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	hanWindow \{ \cf2 _HanWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	welWindow \{ \cf2 _WelchWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	triWindow \{ \cf2 _TriWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
\
	scurve \{ \cf2 _SCurve\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	ramp \{ \cf2 _Ramp\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	\
	+ \{ \cf2 arg\cf0  aNumber; \cf2 _Add\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 '+'\cf0 , \cf2 this\cf0 ) \}\
	- \{ \cf2 arg\cf0  aNumber; \cf2 _Sub\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 '-'\cf0 , \cf2 this\cf0 ) \}\
	* \{ \cf2 arg\cf0  aNumber; \cf2 _Mul\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 '*'\cf0 , \cf2 this\cf0 ) \}\
	/ \{ \cf2 arg\cf0  aNumber; \cf2 _FDiv\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 '/'\cf0 , \cf2 this\cf0 ) \}\
	mod \{ \cf2 arg\cf0  aNumber; \cf2 _Mod\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'mod'\cf0 , \cf2 this\cf0 ) \}\
	div \{ \cf2 arg\cf0  aNumber; \cf2 _IDiv\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'div'\cf0 , \cf2 this\cf0 ) \}\
	pow \{ \cf2 arg\cf0  aNumber; \cf2 _Pow\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'pow'\cf0 , \cf2 this\cf0 ) \}\
	min \{ \cf2 arg\cf0  aNumber; \cf2 _Min\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'min'\cf0 , \cf2 this\cf0 ) \} \
	max \{ \cf2 arg\cf0  aNumber; \cf2 _Max\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'max'\cf0 , \cf2 this\cf0 ) \}\
	ring1 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring1\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'ring1'\cf0 , \cf2 this\cf0 ) \}\
	ring2 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring2\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'ring2'\cf0 , \cf2 this\cf0 ) \}\
	ring3 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring3\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'ring3'\cf0 , \cf2 this\cf0 ) \}\
	ring4 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring4\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'ring4'\cf0 , \cf2 this\cf0 ) \}\
	difsqr \{ \cf2 arg\cf0  aNumber; \cf2 _DifSqr\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'difsqr'\cf0 , \cf2 this\cf0 ) \}\
	sumsqr \{ \cf2 arg\cf0  aNumber; \cf2 _SumSqr\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'sumsqr'\cf0 , \cf2 this\cf0 ) \}\
	sqrsum \{ \cf2 arg\cf0  aNumber; \cf2 _SqrSum\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'sqrsum'\cf0 , \cf2 this\cf0 ) \}\
	sqrdif \{ \cf2 arg\cf0  aNumber; \cf2 _SqrDif\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'sqrdif'\cf0 , \cf2 this\cf0 ) \}\
	absdif \{ \cf2 arg\cf0  aNumber; \cf2 _AbsDif\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'absdif'\cf0 , \cf2 this\cf0 ) \}\
	thresh \{ \cf2 arg\cf0  aNumber; \cf2 _Thresh\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'thresh'\cf0 , \cf2 this\cf0 ) \}\
	amclip \{ \cf2 arg\cf0  aNumber; \cf2 _AMClip\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'amclip'\cf0 , \cf2 this\cf0 ) \}\
	scaleneg \{ \cf2 arg\cf0  aNumber; \cf2 _ScaleNeg\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'scaleneg'\cf0 , \cf2 this\cf0 ) \}\
	clip2 \{ \cf2 arg\cf0  aNumber; \cf2 _Clip2\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'clip2'\cf0 , \cf2 this\cf0 ) \}\
	fold2 \{ \cf2 arg\cf0  aNumber; \cf2 _Fold2\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'fold2'\cf0 , \cf2 this\cf0 ) \}\
	wrap2 \{ \cf2 arg\cf0  aNumber; \cf2 _Wrap2\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'wrap2'\cf0 , \cf2 this\cf0 ) \}\
	excess \{ \cf2 arg\cf0  aNumber; \cf2 _Excess\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'excess'\cf0 , \cf2 this\cf0 ) \}\
	firstArg \{ \cf2 arg\cf0  aNumber; \cf2 _FirstArg\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 'firstArg'\cf0 , \cf2 this\cf0 ) \}\
\
	== \{ \cf2 arg\cf0  aNumber; \cf2 _EQ\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 '=='\cf0 , \cf2 this\cf0 ) \}\
	!= \{ \cf2 arg\cf0  aNumber; \cf2 _NE\cf0 ; ^aNumber.performBinaryOpOnSignal(\cf5 '!='\cf0 , \cf2 this\cf0 ) \}\
	\
	clip \{ \cf2 arg\cf0  lo, hi; \cf2 _ClipSignal\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	wrap \{ \cf2 arg\cf0  lo, hi; \cf2 _WrapSignal\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	fold \{ \cf2 arg\cf0  lo, hi; \cf2 _FoldSignal\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
		\
	asInteger \{ \cf2 _AsInt\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	asFloat \{ \cf2 _AsFloat\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	asComplex \{ ^\cf2 Complex\cf0 .new(\cf2 this\cf0 , 0.0) \}\
	asSignal \{ ^\cf2 this\cf0  \}\
	\
	\cf3 // complex support\
\cf0 	real \{ ^\cf2 this\cf0  \}\
	imag \{ ^0.0 \}	\
	\
	\cf3 //PRIVATE: \
\cf0 	performBinaryOpOnSignal \{ \cf2 arg\cf0  aSelector, aNumber; ^error(\cf4 "Math operation failed.\\n"\cf0 ) \}\
	performBinaryOpOnComplex \{ \cf2 arg\cf0  aSelector, aComplex; ^aComplex.perform(aSelector, \cf2 this\cf0 .asComplex) \}\
	performBinaryOpOnSimpleNumber \{ \cf2 arg\cf0  aSelector, aSimpleNumber; ^aSimpleNumber.perform(aSelector, \cf2 this\cf0 ) \}\
\}\
\
\cf2 Wavetable\cf0 [float] : \cf2 FloatArray\cf0  \{\
	\cf3 // the only way to make a Wavetable is by Signal::asWavetable\
\cf0 	*new \{ \
		^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 )\
	\}\
	*newClear \{ \
		^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 )\
	\}\
	\
	*sineFill \{ \cf2 arg\cf0  size, amplitudes, phases;\
		^\cf2 Signal\cf0 .sineFill(size, amplitudes, phases).asWavetable\
	\}\
	*chebyFill \{ \cf2 arg\cf0  size, amplitudes;\
		^\cf2 Signal\cf0 .chebyFill(size, amplitudes).asWavetable\
	\}\
\
	\
	asSignal \{\
		\cf2 _WavetableAsSignal\cf0 \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	plot \{ \cf2 arg\cf0  name, bounds;\
	\}\
\
	blend \{ \cf2 arg\cf0  anotherWavetable, blendFrac=0.5;\
		^\cf2 this\cf0 .asSignal.blend(anotherWavetable.asSignal, blendFrac).asWavetable;\
	\}\
	\
	*readNew \{ \cf2 arg\cf0  file;\
		^file.readAllSignal.asWavetable;\
	\}\
	write \{ \cf2 arg\cf0  path;\
		\cf2 var\cf0  file;\
		file = \cf2 File\cf0 .new(path, \cf4 "wb"\cf0 );\
		if (file.notNil, \{\
			file.write(\cf2 this\cf0 .asSignal);\
			file.close;\
		\});\
	\}\
	libMenuAction \{ \cf2 arg\cf0  names;\
		\cf2 this\cf0 .plot(names.last);\
	\}\
\}\
\
}