{\rtf1\mac\ansicpg10000\cocoartf102
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 /*\
	Osc - oscillator\
	arguments :\
		bufnum - an index to a buffer\
		freq - frequency in cycles per second\
		pm - phase modulation \
		mul - multiply by signal or scalar\
		add - add to signal or scalar\
*/\cf0 \
\
\cf3 Osc\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \
		\cf3 arg\cf0  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , bufnum, freq, phase).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , bufnum, freq, phase).madd(mul, add)\
	\}\
\}\
\
\cf3 SinOsc\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \
		\cf3 arg\cf0  freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, phase).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , freq, phase).madd(mul, add)\
	\}\
\}\
\
\cf3 OscN\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \
		\cf3 arg\cf0  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , bufnum, freq, phase).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , bufnum, freq, phase).madd(mul, add)\
	\}\
\}\
\
\cf3 COsc\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \
		\cf3 arg\cf0  bufnum, freq=440.0, beats=0.5, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , bufnum, freq, beats).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  bufnum, freq=440.0, beats=0.5, mul=1.0, add=0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , bufnum, freq, beats).madd(mul, add)\
	\}\
\}\
\
\cf3 Formant\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  fundfreq = 440.0, formfreq = 1760.0, bwfreq = 880.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , fundfreq, formfreq, bwfreq).madd(mul, add)\
	\}\
\}\
\
\cf3 LFSaw\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, iphase).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , freq, iphase).madd(mul, add)\
	\}\
\}\
\
\cf3 LFTri\cf0  : \cf3 LFSaw\cf0 \
\{\
\}\
\
\
\cf3 LFPulse\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, iphase, width).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , freq, iphase, width).madd(mul, add)\
	\}\
	signalRange \{ ^\\unipolar \}\
\}\
\
\cf3 VarSaw\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, iphase, width).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , freq, iphase, width).madd(mul, add)\
	\}\
\}\
\
\cf3 Impulse\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, iphase).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , freq, iphase).madd(mul, add)\
	\}\
	signalRange \{ ^\\unipolar \}\
\}\
\
\
\cf3 SyncSaw\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  syncFreq = 440.0, sawFreq = 440.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , syncFreq, sawFreq).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  syncFreq = 440.0, sawFreq = 440.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , syncFreq, sawFreq).madd(mul, add)\
	\}\
\}\
\
\
\cf3 TPulse\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  trig = 0.0, freq = 440.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , trig, freq, width).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  trig = 0.0, freq = 440.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , trig, freq, width).madd(mul, add)\
	\}\
	signalRange \{ ^\\unipolar \}\
\}\
\
\cf3 Index\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  bufnum, in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , bufnum, in).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  bufnum, in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , bufnum, in).madd(mul, add)\
	\}\
\}\
\
\cf3 WrapIndex\cf0  : \cf3 Index\cf0  \{\
\}\
\
\cf3 Shaper\cf0  : \cf3 Index\cf0  \{\
\}\
\
\
\cf3 DegreeToKey\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  bufnum, in = 0.0, octave = 12.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , bufnum, in, octave).madd(mul, add)\
	\}\
	*kr \{\
		\cf3 arg\cf0  bufnum, in = 0.0, octave = 12.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , bufnum, in, octave).madd(mul, add)\
	\}\
\}\
\
\cf3 Select\cf0  : \cf3 UGen\cf0  \{\
	*ar \{\
		\cf3 arg\cf0  which, array;\
		^\cf3 this\cf0 .multiNewList([\cf4 'audio'\cf0 , which] ++ array)\
	\}\
	*kr \{\
		\cf3 arg\cf0  which, array;\
		^\cf3 this\cf0 .multiNewList([\cf4 'control'\cf0 , which] ++ array)\
	\}\
\}\
\
}