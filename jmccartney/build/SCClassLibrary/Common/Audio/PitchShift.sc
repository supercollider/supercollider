{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 PitchShift\cf0  : \cf2 UGen\cf0  \{	\
	*ar \{ \cf2 arg\cf0  in = 0.0, winSize = 0.2, pchRatio = 1.0, \
			pchDispersion = 0.0, timeDispersion = 0.0, mul = 1.0, add = 0.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, winSize, pchRatio, \
			pchDispersion, timeDispersion).madd(mul, add)\
	\}\
\}\
}