{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 /* \
	Noise Generators\
	\
	WhiteNoise.ar(mul, add)\
	BrownNoise.ar(mul, add)\
	PinkNoise.ar(mul, add)\
	Crackle.ar(chaosParam, mul, add)\
	LFNoise0.ar(freq, mul, add)\
	LFNoise1.ar(freq, mul, add)\
	LFNoise2.ar(freq, mul, add)\
	Dust.ar(density, mul, add)\
	Dust2.ar(density, mul, add)\
	\
	White, Brown, Pink generators have no modulatable parameters\
	other than multiply and add inputs.\
	\
	The chaos param for ChaosNoise should be from 1.0 to 2.0\
	\
*/\cf0 \
\
\cf3 Rand\cf0  : \cf3 UGen\cf0  \{\
	\cf2 // uniform distribution\
\cf0 	*new \{ \cf3 arg\cf0  lo = 0.0, hi = 1.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'scalar'\cf0 , lo, hi)\
	\}\
\}\
\
\cf3 IRand\cf0  : \cf3 UGen\cf0  \{\
	\cf2 // uniform distribution of integers\
\cf0 	*new \{ \cf3 arg\cf0  lo = 0, hi = 127;\
		^\cf3 this\cf0 .multiNew(\cf4 'scalar'\cf0 , lo, hi)\
	\}\
\}\
\
\cf3 LinRand\cf0  : \cf3 UGen\cf0  \{\
	\cf2 // linear distribution\
\cf0 	\cf2 // if minmax <= 0 then skewed towards lo.\
\cf0 	\cf2 // else skewed towards hi.\
\cf0 	*new \{ \cf3 arg\cf0  lo = 0.0, hi = 1.0, minmax = 0;\
		^\cf3 this\cf0 .multiNew(\cf4 'scalar'\cf0 , lo, hi, minmax)\
	\}\
\}\
\cf3 NRand\cf0  : \cf3 UGen\cf0  \{\
	\cf2 // sum of N uniform distributions.\
\cf0 	\cf2 // n = 1 : uniform distribution - same as Rand\
\cf0 	\cf2 // n = 2 : triangular distribution\
\cf0 	\cf2 // n = 3 : smooth hump\
\cf0 	\cf2 // as n increases, distribution converges towards gaussian\
\cf0 	*new \{ \cf3 arg\cf0  lo = 0.0, hi = 1.0, n = 0;\
		^\cf3 this\cf0 .multiNew(\cf4 'scalar'\cf0 , lo, hi, n)\
	\}\
\}\
\
\cf3 ExpRand : UGen\cf0  \{\
	\cf2 // exponential distribution\
\cf0 	*new \{ \cf3 arg\cf0  lo = 0.01, hi = 1.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'scalar'\cf0 , lo, hi)\
	\}\
\}\
\
\cf3 WhiteNoise\cf0  : \cf3 UGen\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 ).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 ).madd(mul, add)\
	\}\
	\
\}\
\
\cf3 BrownNoise\cf0  : \cf3 WhiteNoise\cf0  \{\
\}\
\
\cf3 PinkNoise\cf0  : \cf3 WhiteNoise\cf0  \{\
\}\
\
\cf3 PinkerNoise\cf0  : \cf3 WhiteNoise\cf0  \{\
\}\
\
\cf3 ClipNoise\cf0  : \cf3 WhiteNoise\cf0  \{\
\}\
\
\cf3 GrayNoise\cf0  : \cf3 WhiteNoise\cf0  \{\
\}\
\
\
\cf3 NoahNoise\cf0  : \cf3 WhiteNoise\cf0  \{\
\}\
\
\cf3 Crackle\cf0  : \cf3 UGen\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  chaosParam=1.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , chaosParam).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  chaosParam=1.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , chaosParam).madd(mul, add)\
	\}\
\}\
\
\cf3 Logistic\cf0  : \cf3 UGen\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  chaosParam=3.0, freq = 1000.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , chaosParam, freq).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  chaosParam=3.0, freq = 1000.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , chaosParam, freq).madd(mul, add)\
	\}\
\}\
\
\cf3 Rossler\cf0  : \cf3 UGen\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  chaosParam=1.5, dt = 0.04, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , chaosParam, dt).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  chaosParam=1.5, dt = 0.04, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , chaosParam, dt).madd(mul, add)\
	\}\
\}\
\
\cf3 LFNoise0\cf0  : \cf3 UGen\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  freq=500.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  freq=500.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , freq).madd(mul, add)\
	\}\
\}\
\
\cf3 LFNoise1\cf0  : \cf3 LFNoise0\cf0  \{\
\}\
\
\cf3 LFNoise2\cf0  : \cf3 LFNoise0\cf0  \{\
\}\
\
\cf3 LFClipNoise\cf0  : \cf3 LFNoise0\cf0  \{\
\}\
\
\
\cf3 Dust\cf0  : \cf3 UGen\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  density = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , density).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  density = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , density).madd(mul, add)\
	\}\
	\
\}\
\
\cf3 Dust2\cf0  : \cf3 Dust\cf0  \{\
\}\
\
\cf3 LinCong\cf0  : \cf3 UGen\cf0  \{\
	\cf3 var\cf0  iseed, imul, iadd, imod;\
	\
	*ar \{ \cf3 arg\cf0  iseed, imul, iadd, imod, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , iseed, imul, iadd, imod).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  iseed, imul, iadd, imod, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , iseed, imul, iadd, imod).madd(mul, add)\
	\}\
	init \{ \cf3 arg\cf0  jseed, jmul, jadd, jmod ... theInputs;\
 		inputs = theInputs;\
 		iseed = jseed;\
 		imul = jmul;\
 		iadd = jadd;\
 		imod = jmod;\
 	\}\
\}\
\
\cf3 Latoocarfian\cf0  : \cf3 UGen\cf0  \{\
	\
	*ar \{ \cf3 arg\cf0  a, b, c, d, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , a, b, c, d).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  a, b, c, d, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , a, b, c, d).madd(mul, add)\
	\}\
\}\
}