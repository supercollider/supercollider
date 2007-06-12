{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green112\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 Amplitude\cf0  : \cf2 UGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, attackTime, releaseTime).madd(mul, add)\
	\}\
	*kr \{ \cf2 arg\cf0  in = 0.0, attackTime = 0.01, releaseTime = 0.01, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , in, attackTime, releaseTime).madd(mul, add)\
	\}\
\}\
\
\cf2 Compander\cf0  : \cf2 UGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  in = 0.0, control = 0.0, thresh = 0.5, slopeBelow = 1.0, slopeAbove = 1.0,\
		clampTime = 0.01, relaxTime = 0.01, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, control, thresh, slopeBelow, slopeAbove,\
			clampTime, relaxTime).madd(mul, add)\
	\}\
\}\
\
\
\cf4 // CompanderD passes the signal directly to the control input, \
// but adds a delay to the process input so that the lag in the gain \
// clamping will not lag the attacks in the input sound\
\cf0 \
\cf2 CompanderD\cf0  : \cf2 UGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  in = 0.0, thresh = 0.5, slopeBelow = 1.0, slopeAbove = 1.0,\
		clampTime = 0.01, relaxTime = 0.01, mul = 1.0, add = 0.0;\
		\
		^\cf2 Compander\cf0 .ar(\cf2 DelayN\cf0 .ar(in, clampTime, clampTime), in, thresh, \
				slopeBelow, slopeAbove, clampTime, relaxTime).madd(mul, add)\
	\}	\
\}\
\
\
\cf2 Normalizer\cf0  : \cf2 UGen\cf0  \{\
	\cf2 var\cf0  buffer;\
	*ar \{ \cf2 arg\cf0  in = 0.0, level = 1.0, dur = 0.01;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, level, dur)\
	\}\
\}\
\
\cf2 Limiter\cf0  : \cf2 Normalizer\cf0  \{\}\
}