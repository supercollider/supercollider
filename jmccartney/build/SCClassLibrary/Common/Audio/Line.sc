{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green112\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Line\cf0  : \cf2 UGen\cf0  \{	\
	*ar \{ \cf2 arg\cf0  start=0.0, end = 1.0, dur = 1.0, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , start, end, dur).madd(mul, add)\
	\}\
	*kr \{ \cf2 arg\cf0  start=0.0, end = 1.0, dur = 1.0, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 ,  start, end, dur).madd(mul, add)\
	\}\
\}\
\
\cf2 XLine\cf0  : \cf2 UGen\cf0  \{	\
	*ar \{ \cf2 arg\cf0  start=1.0, end = 2.0, dur = 1.0, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , start, end, dur).madd(mul, add)\
	\}\
	*kr \{ \cf2 arg\cf0  start=1.0, end = 2.0, dur = 1.0, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 ,  start, end, dur).madd(mul, add)\
	\}\
\}\
\
\cf2 LinExp\cf0  : \cf2 UGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, srclo, srchi, dstlo, dsthi)\
	\}\
	*kr \{ \cf2 arg\cf0  in=0.0, srclo = 0.0, srchi = 1.0, dstlo = 1.0, dsthi = 2.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 ,  in, srclo, srchi, dstlo, dsthi)\
	\}\
\}	\
\
\cf2 K2A\cf0  : \cf2 UGen\cf0  \{ \cf4 // control rate to audio rate converter - used internally on outputs to Synth\
\cf0 	*ar \{ \cf2 arg\cf0  in = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in)\
	\}\
\}\
}