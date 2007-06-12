{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 Trig1\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  in = 0.0, dur = 0.1;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, dur)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, dur = 0.1;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, dur)\
	\}\
	\
\}\
\
\cf3 Trig\cf2  : \cf3 Trig1\cf2  \{\
\}\
\
\
\cf3 SendTrig\cf2  : \cf3 UGen\cf2  \{\
	*ar \{ \cf3 arg\cf2  in = 0.0, id = 0, value = 0.0;\
		\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, id, value);\
		^0.0		\cf5 // SendTrig has no output\
\cf2 	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, id = 0, value = 0.0;\
		\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, id, value);\
		^0.0		\cf5 // SendTrig has no output\
\cf2 	\}\
 	checkInputs \{\
 		if (rate == \cf4 'audio'\cf2 , \{\
 			if (inputs.at(0).rate != \cf4 'audio'\cf2 , \{ ^\cf3 false\cf2  \});\
 		\});\
 		^\cf3 true\cf2 \
 	\}\
	numOutputs \{ ^0 \}\
	writeOutputSpecs \{\}\
\}\
\
\cf3 TDelay\cf2  : \cf3 Trig1\cf2  \{\
\}\
\
\cf3 Latch\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  in = 0.0, trig = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, trig)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, trig = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, trig)\
	\}\
	\
\}\
\
\cf3 Gate\cf2  : \cf3 Latch\cf2  \{	\
\}\
\
\cf3 PulseCount\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  trig = 0.0, reset = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , trig, reset)\
	\}\
	*kr \{ \cf3 arg\cf2  trig = 0.0, reset = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , trig, reset)\
	\}\
\}\
\
\cf3 Peak\cf2  : \cf3 PulseCount\cf2  \{\
\}\
\
\cf3 PulseDivider\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  trig = 0.0, div = 2.0, start = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , trig, div, start)\
	\}\
	*kr \{ \cf3 arg\cf2  trig = 0.0, div = 2.0, start = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , trig, div, start)\
	\}\
	\
\}\
\
\cf3 SetResetFF\cf2  : \cf3 PulseCount\cf2  \{\
\}\
\
\cf3 ToggleFF\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  trig = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , trig)\
	\}\
	*kr \{ \cf3 arg\cf2  trig = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , trig)\
	\}\
\}\
\
\
\cf3 ZeroCrossing\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  in = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in)\
	\}\
\}\
\
\cf3 PeakFollower\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  in = 0.0, decay = 0.999;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, decay)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, decay = 0.999;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, decay)\
	\}\
\}\
\
\cf3 Pitch\cf2  : \cf3 MultiOutUGen\cf2  \{\
	\
	*kr \{ \cf3 arg\cf2  in = 0.0, initFreq = 440.0, minFreq = 60.0, maxFreq = 4000.0, execFreq = 100.0,\
		maxBinsPerOctave = 16, median = 1, ampThreshold = 0.01, peakThreshold = 0.5, downSample = 1;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, initFreq, minFreq, maxFreq, execFreq,\
				maxBinsPerOctave, median, ampThreshold, peakThreshold, downSample)\
	\}\
\}\
\
\cf3 InRange\cf2  : \cf3 UGen\cf2 \
\{\
	*ar \{ \cf3 arg\cf2  in = 0.0, lo = 0.0, hi = 1.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, lo, hi)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, lo = 0.0, hi = 1.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, lo, hi)\
	\}\
\}\
\
\cf3 InRect\cf2  : \cf3 UGen\cf2 \
\{\
	*ar \{ \cf3 arg\cf2  x = 0.0, y = 0.0, rect;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , x, y, rect)\
	\}\
	*kr \{ \cf3 arg\cf2  x = 0.0, y = 0.0, rect;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , x, y, rect)\
	\}\
\}\
\
\cf3 Trapezoid\cf2  : \cf3 UGen\cf2 \
\{\
	*ar \{ \cf3 arg\cf2  in = 0.0, a = 0.2, b = 0.4, c = 0.6, d = 0.8;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, a, b, c, d)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, a = 0.2, b = 0.4, c = 0.6, d = 0.8;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, a, b, c, d)\
	\}\
\}\
\
\cf3 Fold\cf2  : \cf3 InRange\cf2  \{\}\
\cf3 Clip\cf2  : \cf3 InRange\cf2  \{\}\
\cf3 Wrap\cf2  : \cf3 InRange\cf2  \{\}\
\cf3 Schmidt\cf2  : \cf3 InRange\cf2  \{\}\
\
\cf3 MostChange\cf2  : \cf3 UGen\cf2 \
\{\
	*ar \{ \cf3 arg\cf2  a = 0.0, b = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , a, b)\
	\}\
	*kr \{ \cf3 arg\cf2  a = 0.0, b = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , a, b)\
	\}\
\}\
\
\cf3 LeastChange\cf2  : \cf3 MostChange\cf2  \{\}\
}