{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue0;\red0\green0\blue191;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 /*\
	Osc - oscillator\
	arguments :\
		bufnum - an index to a buffer\
		freq - frequency in cycles per second\
		pm - phase modulation \
		mul - multiply by signal or scalar\
		add - add to signal or scalar\
*/\cf3 \
\
\cf4 Osc\cf3  : \cf4 UGen\cf3  \{	\
	*ar \{ \
		\cf4 arg\cf3  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , bufnum, freq, phase).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , bufnum, freq, phase).madd(mul, add)\
	\}\
\}\
\
\cf4 SinOsc\cf3  : \cf4 UGen\cf3  \{	\
	*ar \{ \
		\cf4 arg\cf3  freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , freq, phase).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , freq, phase).madd(mul, add)\
	\}\
\}\
\
\cf4 OscN\cf3  : \cf4 UGen\cf3  \{	\
	*ar \{ \
		\cf4 arg\cf3  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , bufnum, freq, phase).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  bufnum, freq=440.0, phase=0.0, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , bufnum, freq, phase).madd(mul, add)\
	\}\
\}\
\
\cf4 COsc\cf3  : \cf4 UGen\cf3  \{	\
	*ar \{ \
		\cf4 arg\cf3  bufnum, freq=440.0, beats=0.5, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , bufnum, freq, beats).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  bufnum, freq=440.0, beats=0.5, mul=1.0, add=0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , bufnum, freq, beats).madd(mul, add)\
	\}\
\}\
\
\cf4 Formant\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  fundfreq = 440.0, formfreq = 1760.0, bwfreq = 880.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , fundfreq, formfreq, bwfreq).madd(mul, add)\
	\}\
\}\
\
\cf4 LFSaw\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , freq, iphase).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , freq, iphase).madd(mul, add)\
	\}\
\}\
\
\cf4 LFTri\cf3  : \cf4 LFSaw\cf3 \
\{\
\}\
\
\
\cf4 LFPulse\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , freq, iphase, width).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  freq = 440.0, iphase = 0.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , freq, iphase, width).madd(mul, add)\
	\}\
\}\
\
\cf4 VarSaw\cf3  : \cf4 LFPulse\cf3  \{\
\}\
\
\cf4 Impulse\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , freq, iphase).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  freq = 440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , freq, iphase).madd(mul, add)\
	\}\
\}\
\
\
\cf4 SyncSaw\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  syncFreq = 440.0, sawFreq = 440.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , syncFreq, sawFreq).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  syncFreq = 440.0, sawFreq = 440.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , syncFreq, sawFreq).madd(mul, add)\
	\}\
\}\
\
\
\cf4 TPulse\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  trig = 0.0, freq = 440.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , trig, freq, width).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  trig = 0.0, freq = 440.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , trig, freq, width).madd(mul, add)\
	\}\
\}\
\
\cf4 Index\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  bufnum, in = 0.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , bufnum, in).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  bufnum, in = 0.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , bufnum, in).madd(mul, add)\
	\}\
\}\
\
\cf4 WrapIndex\cf3  : \cf4 Index\cf3  \{\
\}\
\
\cf4 Shaper\cf3  : \cf4 Index\cf3  \{\
\}\
\
\
\cf4 DegreeToKey\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  bufnum, in = 0.0, octave = 12.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , bufnum, in, octave).madd(mul, add)\
	\}\
	*kr \{\
		\cf4 arg\cf3  bufnum, in = 0.0, octave = 12.0, mul = 1.0, add = 0.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'control'\cf3 , bufnum, in, octave).madd(mul, add)\
	\}\
\}\
\
\cf4 Select\cf3  : \cf4 UGen\cf3  \{\
	*ar \{\
		\cf4 arg\cf3  which, array;\
		^\cf4 this\cf3 .multiNewList([\cf5 'audio'\cf3 , which] ++ array)\
	\}\
	*kr \{\
		\cf4 arg\cf3  which, array;\
		^\cf4 this\cf3 .multiNewList([\cf5 'control'\cf3 , which] ++ array)\
	\}\
\}\
\
}