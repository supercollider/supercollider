{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
\red191\green0\blue0;\red0\green0\blue191;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 Delay1\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in).madd(mul, add)\
	\}\
\}\
\
\cf3 Delay2\cf2  : \cf3 Delay1\cf2  \{ \}\
\
\cf5 ///////////////////////////////////////\
\cf2 \
\cf5 // these delays use real time allocated memory.\
\cf2 \
\cf3 DelayN\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, maxdelaytime, delaytime).madd(mul, add)\
	\}\
\cf0 	*kr \{ \cf6 arg\cf0  in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, mul = 1.0, add = 0.0;\
\cf2 		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, maxdelaytime, delaytime).madd(mul, add)\
	\}\
\}\
\
\cf3 DelayL\cf2  : \cf3 DelayN\cf2  \{ \}\
\cf3 DelayC\cf2  : \cf3 DelayN\cf2  \{ \}\
\
\
\cf3 CombN\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , in, maxdelaytime, delaytime, decaytime).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf2  in = 0.0, maxdelaytime = 0.2, delaytime = 0.2, decaytime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , in, maxdelaytime, delaytime, decaytime).madd(mul, add)\
	\}\
\}\
\
\cf3 CombL\cf2  : \cf3 CombN\cf2  \{ \}\
\cf3 CombC\cf2  : \cf3 CombN\cf2  \{ \}\
\
\cf3 AllpassN\cf2  : \cf3 CombN\cf2  \{ \}\
\cf3 AllpassL\cf2  : \cf3 CombN\cf2  \{ \}\
\cf3 AllpassC\cf2  : \cf3 CombN\cf2  \{ \}\
\
\cf5 ///////////////////////////////////////\
\cf2 \
\cf5 // these delays use shared buffers.\
\cf2 \
\cf3 BufDelayN\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  buf = 0, in = 0.0, delaytime = 0.2, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , buf, in, delaytime).madd(mul, add)\
	\}\
	*kr \{ \cf3 arg\cf2  buf = 0, in = 0.0, delaytime = 0.2, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'control'\cf2 , buf, in, delaytime).madd(mul, add)\
	\}\
\}\
\
\cf3 BufDelayL\cf2  : \cf3 BufDelayN\cf2  \{ \}\
\cf3 BufDelayC\cf2  : \cf3 BufDelayN\cf2  \{ \}\
\
\
\cf3 BufCombN\cf2  : \cf3 UGen\cf2  \{\
	\
	*ar \{ \cf3 arg\cf2  buf = 0, in = 0.0, delaytime = 0.2, decaytime = 1.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , buf, in, delaytime, decaytime).madd(mul, add)\
	\}\
\}\
\
\cf3 BufCombL\cf2  : \cf3 BufCombN\cf2  \{ \}\
\cf3 BufCombC\cf2  : \cf3 BufCombN\cf2  \{ \}\
\
\cf3 BufAllpassN\cf2  : \cf3 BufCombN\cf2  \{ \}\
\cf3 BufAllpassL\cf2  : \cf3 BufCombN\cf2  \{ \}\
\cf3 BufAllpassC\cf2  : \cf3 BufCombN\cf2  \{ \}\
\
\cf5 ///////////////////////////////////////\
\cf2 \
\cf3 GrainTap\cf2  : \cf3 MultiOutUGen\cf2  \{	\
	*ar \{ \cf3 arg\cf2  grainDur = 0.2, pchRatio = 1.0, \
			pchDispersion = 0.0, timeDispersion = 0.0, overlap = 2.0, mul = 1.0, add = 0.0;\
		^\cf3 this\cf2 .multiNew(\cf4 'audio'\cf2 , grainDur, pchRatio, \
			pchDispersion, timeDispersion, overlap).madd(mul, add)\
	\}\
\}\
\
}