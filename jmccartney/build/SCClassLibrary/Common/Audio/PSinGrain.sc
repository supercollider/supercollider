{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue0;\red0\green0\blue191;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 /*\
	PSinGrain - fixed frequency sine oscillator\
	arguments :\
		freq - frequency in cycles per second. Must be a scalar.\
		dur - grain duration\
		amp - amplitude of grain\
		\
	This unit generator uses a very fast algorithm for generating a sine\
	wave at a fixed frequency.\
*/\cf3 \
\
\cf4 PSinGrain\cf3  : \cf4 UGen\cf3  \{	\
	*ar \{ \cf4 arg\cf3  freq = 440.0, dur = 0.2, amp = 1.0;\
		^\cf4 this\cf3 .multiNew(\cf5 'audio'\cf3 , freq, dur, amp)\
	\}\
\}\
}