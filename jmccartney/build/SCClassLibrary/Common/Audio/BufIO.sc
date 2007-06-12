{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red0\green112\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 /*\
	PlayBuf - sample player\
*/\cf0 \
\
\cf3 PlayBuf\cf0  : \cf3 MultiOutUGen\cf0  \{	\
	*ar \{ \cf3 arg\cf0  numChannels, bufnum=0, rate=1.0, trigger=0.0, startPos=0.0, loop = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , numChannels, bufnum, rate, trigger, startPos, loop)\
	\}\
	\
	init \{ \cf3 arg\cf0  argNumChannels ... theInputs;\
		inputs = theInputs;\
		^\cf3 this\cf0 .initOutputs(argNumChannels, rate);\
	\}\
\}\
\
\cf3 ReadBuf\cf0  : \cf3 MultiOutUGen\cf0  \{	\
	*ar \{ \cf3 arg\cf0  numChannels, bufnum=0, offset=0.0;\
		\cf2 //[\\PlayBuf_ar, \cf0 bufnum\cf2 , rate, offset, interpolationType, channelOffset, numChannels].postln;\
\cf0 		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , numChannels, bufnum, offset)\
	\}\
	\
	init \{ \cf3 arg\cf0  argNumChannels ... theInputs;\
		inputs = theInputs;\
		^\cf3 this\cf0 .initOutputs(argNumChannels, rate);\
	\}\
\}\
\
\cf3 RecordBuf\cf0  : \cf3 UGen\cf0  \{	\
	*ar \{ \cf3 arg\cf0  inputArray, bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0, run=1.0;\
		\cf3 this\cf0 .multiNewList(\cf4 'audio'\cf0 , inputArray.asArray ++ [ bufnum, offset, recLevel, preLevel, run ]);\
		^inputArray\
	\}\
	init \{ \cf3 arg\cf0  ... theInputs;\
		inputs = theInputs;\
	\}\
\}\
\
\
}