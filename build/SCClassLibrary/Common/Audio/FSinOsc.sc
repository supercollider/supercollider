{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 /*\
	FSinOsc - fixed frequency sine oscillator\
	arguments :\
		freq - frequency in cycles per second. Must be a scalar.\
		mul - multiply by signal or scalar\
		add - add to signal or scalar\
		\
	This unit generator uses a very fast algorithm for generating a sine\
	wave at a fixed frequency.\
*/\cf0 \
\
\cf3 FSinOsc\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \cf3 arg\cf0  freq=440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, iphase).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf0  freq=440.0, iphase = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'control'\cf0 , freq, iphase).madd(mul, add)\
	\}\
\}\
\
\
\cf3 Klang\cf0  : \cf3 UGen\cf0  \{\
	\cf3 var\cf0  freqs, amps, phases;\
	\
	*ar \{ \cf3 arg\cf0  specificationsArrayRef, freqscale = 1.0, freqoffset = 0.0;\
		\cf3 var\cf0  freqs, amps, phases;\
			\
		# freqs, amps, phases = specificationsArrayRef.value;\
		if ((freqs.size != amps.size) || (freqs.size != phases.size), \{ \
			\cf5 "Klank freqs, amps, times not same size."\cf0 .error;\
			^\cf3 this\cf0 .halt \
		\});\
\
		^\cf3 this\cf0 .multiNewList([\cf4 'audio'\cf0 , freqscale, freqoffset] ++ freqs ++ amps ++ phases )\
	\}\
\}\
\
\cf3 Klank\cf0  : \cf3 UGen\cf0  \{\
	\cf3 var\cf0  freqs, amps, times;\
	\
	*ar \{ \cf3 arg\cf0  specificationsArrayRef, input, freqscale = 1.0, freqoffset = 0.0, decayscale = 1.0;\
		\cf3 var\cf0  freqs, amps, times;\
		\
		# freqs, amps, times = specificationsArrayRef.value;\
		if ((freqs.size != amps.size) || (freqs.size != times.size), \{ \
			\cf5 "Klank freqs, amps, times not same size."\cf0 .error;\
			^\cf3 this\cf0 .halt \
		\});\
		\
		^\cf3 this\cf0 .multiNewList([\cf4 'audio'\cf0 , input, freqscale, freqoffset, decayscale] ++ freqs ++ amps ++ times )\
	\}\
\}\
\
\
\cf3 Blip\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \cf3 arg\cf0  freq=440.0, numharm = 200.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, numharm).madd(mul, add)\
	\}\
\}\
\
\cf3 Saw\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \cf3 arg\cf0  freq=440.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq).madd(mul, add)\
	\}\
\}\
\
\cf3 Pulse\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \cf3 arg\cf0  freq=440.0, width = 0.5, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , freq, width).madd(mul, add)\
	\}\
\}\
}