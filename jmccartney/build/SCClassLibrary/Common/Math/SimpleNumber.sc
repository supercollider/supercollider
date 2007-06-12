{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green115\blue0;\red96\green96\blue96;
\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 SimpleNumber\cf0  : \cf2 Number\cf0  \{\
		\
	isValidUGenInput \{ ^\cf2 true\cf0  \}\
	\
	magnitude \{ ^\cf2 this\cf0 .abs \}\
	angle \{ if (\cf2 this\cf0  >= 0, \{^0.0\}, \{^pi\} ) \}\
	degreeToKey \{ \cf2 arg\cf0  scale, stepsPerOctave=12;\
		\cf2 var\cf0  size, scaleDegree;\
		size = scale.size;\
		scaleDegree = \cf2 this\cf0 .asInteger;\
		^(stepsPerOctave * (scaleDegree div: size)) + scale.wrapAt(scaleDegree.asInteger)\
	\}\
\
	neg \{ \cf2 _Neg\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	bitNot \{ \cf2 _BitNot\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	abs \{ \cf2 _Abs\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	ceil \{ \cf2 _Ceil\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	floor \{ \cf2 _Floor\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	frac \{ \cf2 _Frac\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	sign \{ \cf2 _Sign\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	squared \{ \cf2 _Squared\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	cubed \{ \cf2 _Cubed\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	sqrt \{ \cf2 _Sqrt\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	exp \{ \cf2 _Exp\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	reciprocal \{ \cf2 _Recip\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	midicps \{ \cf2 _MIDICPS\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	cpsmidi \{ \cf2 _CPSMIDI\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	midiratio \{ \cf2 _MIDIRatio\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	ratiomidi \{ \cf2 _RatioMIDI\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	ampdb \{ \cf2 _AmpDb\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	dbamp \{ \cf2 _DbAmp\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}	\
	octcps \{ \cf2 _OctCPS\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	cpsoct \{ \cf2 _CPSOct\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	log \{ \cf2 _Log\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
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
	rand \{ \cf2 _Rand\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	rand2 \{ \cf2 _Rand2\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	linrand \{ \cf2 _LinRand\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	bilinrand \{ \cf2 _BiLinRand\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	sum3rand \{ \cf2 _Sum3Rand\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
\
	distort \{ \cf2 _Distort\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	softclip \{ \cf2 _SoftClip\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	coin \{ \cf2 _Coin\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	isPositive \{ ^\cf2 this\cf0  >= 0 \}\
	isNegative \{ ^\cf2 this\cf0  < 0 \}\
	isStrictlyPositive \{ ^\cf2 this\cf0  > 0 \}\
\
	rectWindow \{ \cf2 _RectWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	hanWindow \{ \cf2 _HanWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	welWindow \{ \cf2 _WelchWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	triWindow \{ \cf2 _TriWindow\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
\
	scurve \{ \cf2 _SCurve\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	ramp \{ \cf2 _Ramp\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	\
	+ \{ \cf2 arg\cf0  aNumber; \cf2 _Add\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '+'\cf0 , \cf2 this\cf0 ) \}\
	- \{ \cf2 arg\cf0  aNumber; \cf2 _Sub\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '-'\cf0 , \cf2 this\cf0 ) \}\
	* \{ \cf2 arg\cf0  aNumber; \cf2 _Mul\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '*'\cf0 , \cf2 this\cf0 ) \}\
	/ \{ \cf2 arg\cf0  aNumber; \cf2 _FDiv\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '/'\cf0 , \cf2 this\cf0 ) \}\
	mod \{ \cf2 arg\cf0  aNumber; \cf2 _Mod\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'mod'\cf0 , \cf2 this\cf0 ) \}\
	div \{ \cf2 arg\cf0  aNumber; \cf2 _IDiv\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'div'\cf0 , \cf2 this\cf0 ) \}\
	pow \{ \cf2 arg\cf0  aNumber; \cf2 _Pow\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'pow'\cf0 , \cf2 this\cf0 ) \}\
	min \{ \cf2 arg\cf0  aNumber; \cf2 _Min\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'min'\cf0 , \cf2 this\cf0 ) \} \
	max \{ \cf2 arg\cf0  aNumber; \cf2 _Max\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'max'\cf0 , \cf2 this\cf0 ) \}\
	bitAnd \{ \cf2 arg\cf0  aNumber; \cf2 _BitAnd\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'bitAnd'\cf0 , \cf2 this\cf0 ) \}\
	bitOr \{ \cf2 arg\cf0  aNumber; \cf2 _BitOr\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'bitOr'\cf0 , \cf2 this\cf0 ) \}\
	bitXor \{ \cf2 arg\cf0  aNumber; \cf2 _BitXor\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'bitXor'\cf0 , \cf2 this\cf0 ) \}	\
	lcm \{ \cf2 arg\cf0  aNumber; \cf2 _LCM\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'lcm'\cf0 , \cf2 this\cf0 ) \}\
	gcd \{ \cf2 arg\cf0  aNumber; \cf2 _GCD\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'gcd'\cf0 , \cf2 this\cf0 ) \}\
	round \{ \cf2 arg\cf0  aNumber; \cf2 _Round\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'round'\cf0 , \cf2 this\cf0 ) \}\
	trunc \{ \cf2 arg\cf0  aNumber; \cf2 _Trunc\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'trunc'\cf0 , \cf2 this\cf0 ) \}\
	atan2 \{ \cf2 arg\cf0  aNumber; \cf2 _Atan2\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'atan2'\cf0 , \cf2 this\cf0 ) \}	\
	hypot \{ \cf2 arg\cf0  aNumber; \cf2 _Hypot\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'hypot'\cf0 , \cf2 this\cf0 ) \}\
	hypotApx \{ \cf2 arg\cf0  aNumber; \cf2 _HypotApx\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'hypotApx'\cf0 , \cf2 this\cf0 ) \}\
	leftShift \{ \cf2 arg\cf0  aNumber; \cf2 _ShiftLeft\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'leftShift'\cf0 , \cf2 this\cf0 ) \}\
	rightShift \{ \cf2 arg\cf0  aNumber; \cf2 _ShiftRight\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'rightShift'\cf0 , \cf2 this\cf0 ) \}\
	unsignedRightShift \{ \cf2 arg\cf0  aNumber; \cf2 _UnsignedShift\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'unsignedRightShift'\cf0 , \cf2 this\cf0 ) \}\
	ring1 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring1\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'ring1'\cf0 , \cf2 this\cf0 ) \}\
	ring2 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring2\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'ring2'\cf0 , \cf2 this\cf0 ) \}\
	ring3 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring3\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'ring3'\cf0 , \cf2 this\cf0 ) \}\
	ring4 \{ \cf2 arg\cf0  aNumber; \cf2 _Ring4\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'ring4'\cf0 , \cf2 this\cf0 ) \}\
	difsqr \{ \cf2 arg\cf0  aNumber; \cf2 _DifSqr\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'difsqr'\cf0 , \cf2 this\cf0 ) \}\
	sumsqr \{ \cf2 arg\cf0  aNumber; \cf2 _SumSqr\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'sumsqr'\cf0 , \cf2 this\cf0 ) \}\
	sqrsum \{ \cf2 arg\cf0  aNumber; \cf2 _SqrSum\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'sqrsum'\cf0 , \cf2 this\cf0 ) \}\
	sqrdif \{ \cf2 arg\cf0  aNumber; \cf2 _SqrDif\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'sqrdif'\cf0 , \cf2 this\cf0 ) \}\
	absdif \{ \cf2 arg\cf0  aNumber; \cf2 _AbsDif\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'absdif'\cf0 , \cf2 this\cf0 ) \}\
	thresh \{ \cf2 arg\cf0  aNumber; \cf2 _Thresh\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'thresh'\cf0 , \cf2 this\cf0 ) \}\
	amclip \{ \cf2 arg\cf0  aNumber; \cf2 _AMClip\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'amclip'\cf0 , \cf2 this\cf0 ) \}\
	scaleneg \{ \cf2 arg\cf0  aNumber; \cf2 _ScaleNeg\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'scaleneg'\cf0 , \cf2 this\cf0 ) \}\
	clip2 \{ \cf2 arg\cf0  aNumber; \cf2 _Clip2\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'clip2'\cf0 , \cf2 this\cf0 ) \}\
	fold2 \{ \cf2 arg\cf0  aNumber; \cf2 _Fold2\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'fold2'\cf0 , \cf2 this\cf0 ) \}\
	wrap2 \{ \cf2 arg\cf0  aNumber; \cf2 _Wrap2\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'wrap2'\cf0 , \cf2 this\cf0 ) \}\
\
	excess \{ \cf2 arg\cf0  aNumber; \cf2 _Excess\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'excess'\cf0 , \cf2 this\cf0 ) \}\
	firstArg \{ \cf2 arg\cf0  aNumber; \cf2 _FirstArg\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'firstArg'\cf0 , \cf2 this\cf0 ) \}\
	rrand \{ \cf2 arg\cf0  aNumber; \cf2 _RandRange\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'rrand'\cf0 , \cf2 this\cf0 ) \}\
	exprand \{ \cf2 arg\cf0  aNumber; \cf2 _ExpRandRange\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 'exprand'\cf0 , \cf2 this\cf0 ) \}\
\
	== \{ \cf2 arg\cf0  aNumber; \cf2 _EQ\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '=='\cf0 , \cf2 this\cf0 ) \}\
	!= \{ \cf2 arg\cf0  aNumber; \cf2 _NE\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '!='\cf0 , \cf2 this\cf0 ) \}\
	< \{ \cf2 arg\cf0  aNumber; \cf2 _LT\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '<'\cf0 , \cf2 this\cf0 ) \}\
	> \{ \cf2 arg\cf0  aNumber; \cf2 _GT\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '>'\cf0 , \cf2 this\cf0 ) \}\
	<= \{ \cf2 arg\cf0  aNumber; \cf2 _LE\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '<='\cf0 , \cf2 this\cf0 ) \}\
	>= \{ \cf2 arg\cf0  aNumber; \cf2 _GE\cf0 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf3 '>='\cf0 , \cf2 this\cf0 ) \}\
\
	hash \{ \cf2 _ObjectHash\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
		\
	asInteger \{ \cf2 _AsInt\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	asFloat \{ \cf2 _AsFloat\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	asComplex \{ ^\cf2 Complex\cf0 .new(\cf2 this\cf0 , 0.0) \}\
	asSignal \{ \cf2 _AsSignal\cf0 ; \cf2 this\cf0 .primitiveFailed \}\
	fontID \{ ^\cf2 this\cf0  \}\
	\
	performBinaryOpOnSimpleNumber \{ \cf2 arg\cf0  aSelector, aNumber; ^error(\cf4 "Math operation failed.\\n"\cf0 ) \}\
	performBinaryOpOnComplex \{ \cf2 arg\cf0  aSelector, aComplex; ^aComplex.perform(aSelector, \cf2 this\cf0 .asComplex) \}\
	performBinaryOpOnSignal \{ \cf2 arg\cf0  aSelector, aSignal; ^error(\cf4 "Math operation failed.\\n"\cf0 ) \}\
	performBinaryOpOnInfinitum \{ \cf2 arg\cf0  aSelector, aInf; ^error(\cf4 "Math operation failed.\\n"\cf0 ) \}\
\
	linlin \{ \cf2 arg\cf0  inMin, inMax, outMin, outMax;\
		\cf5 // linear to linear mapping\
\cf0 		if (\cf2 this\cf0  <= inMin, \{ ^outMin \});\
		if (\cf2 this\cf0  >= inMax, \{ ^outMax \});\
    		^(\cf2 this\cf0 -inMin)/(inMax-inMin) * (outMax-outMin) + outMin;\
	\}\
	linexp \{ \cf2 arg\cf0  inMin, inMax, outMin, outMax;\
		\cf5 // linear to exponential mapping\
\cf0 		if (\cf2 this\cf0  <= inMin, \{ ^outMin \});\
		if (\cf2 this\cf0  >= inMax, \{ ^outMax \});\
		^pow(outMax/outMin, (\cf2 this\cf0 -inMin)/(inMax-inMin)) * outMin\
	\}\
	explin \{ \cf2 arg\cf0  inMin, inMax, outMin, outMax;\
		\cf5 // exponential to linear mapping\
\cf0 		if (\cf2 this\cf0  <= inMin, \{ ^outMin \});\
		if (\cf2 this\cf0  >= inMax, \{ ^outMax \});\
		^(log(\cf2 this\cf0 /inMin)) / (log(inMax/inMin)) * (outMax-outMin) + outMin;\
	\}\
	expexp \{ \cf2 arg\cf0  inMin, inMax, outMin, outMax;\
		\cf5 // exponential to exponential mapping\
\cf0 		if (\cf2 this\cf0  <= inMin, \{ ^outMin \});\
		if (\cf2 this\cf0  >= inMax, \{ ^outMax \});\
		pow(outMax/outMin, log(\cf2 this\cf0 /inMin)) / (log(inMax/inMin)) * outMin;\
	\}\
\
	asPoint \{ ^\cf2 Point\cf0 .new(\cf2 this\cf0 , \cf2 this\cf0 ) \}\
\
	asWarp \{ \cf2 arg\cf0  spec; ^\cf2 CurveWarp\cf0 .new(spec, \cf2 this\cf0 ) \}\
\
	\cf5 // scheduled Routine support\
\cf0 	wait \{ ^\cf2 this\cf0 .yield \}\
	waitUntil \{ ^(\cf2 this\cf0  - \cf2 thisThread\cf0 .time).yield \}\
	\
	printOn \{ \cf2 arg\cf0  stream;\
		stream.putAll(\cf2 this\cf0 .asString);\
	\}\
	storeOn \{ \cf2 arg\cf0  stream;\
		stream.putAll(\cf2 this\cf0 .asString);\
	\}\
	\
\
	rate \{ ^\cf3 'scalar'\cf0  \} \cf5 // scalarRate constant\
\cf0 	\
	\cf5 // support for writing synth defs\
\cf0 	writeInputSpec \{ \cf2 arg\cf0  file, synth;\
		\cf2 var\cf0  constIndex;\
		constIndex = synth.constants.at(\cf2 this\cf0 .asFloat);\
		\cf5 //[\\inpspc, this.class.name, constIndex, this].postln;\
\cf0 		file.putInt16(-1);\
		file.putInt16(constIndex);\
	\}\
\
\}\
}