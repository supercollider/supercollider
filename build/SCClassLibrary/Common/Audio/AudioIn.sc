{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green115\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 ControlName\cf0 \
\{\
	\cf2 var\cf0  <name, <index;\
	*new \{ \cf2 arg\cf0  name, index;\
		^\cf2 super\cf0 .newCopyArgs(name, index)\
	\}\
		\
\}\
\
\cf2 Control\cf0  : \cf2 MultiOutUGen\cf0  \{\
	\cf2 var\cf0  <values, <specialIndex;\
	*names \{ arg names;\
		\cf2 var\cf0  synthDef, index;\
		synthDef = \cf2 UGen\cf0 .buildSynthDef;\
		index = synthDef.controls.size;\
		names.asArray.do(\{ \cf2 arg\cf0  name, i; \
			synthDef.controlNames = synthDef.controlNames.add(\
				\cf2 ControlName\cf0 (name.asString, index + i)\
			);\
		\});\
	\}\
	*kr \{ \cf2 arg\cf0  values;\
		^\cf2 this\cf0 .multiNewList([\cf3 'control'\cf0 ] ++ values.asArray)\
	\}\
	*ir \{ \cf2 arg\cf0  values;\
		^\cf2 this\cf0 .multiNewList([\cf3 'scalar'\cf0 ] ++ values.asArray)\
	\}\
	init \{ \cf2 arg\cf0  ... argValues;\
		values = argValues;\
		if (synthDef.notNil, \{ \
			specialIndex = synthDef.controls.size;\
			synthDef.controls = synthDef.controls.addAll(values);\
		\});\
		^\cf2 this\cf0 .initOutputs(values.size, rate)\
	\}\
\}\
\
\cf2 ControlTrig\cf0  : \cf2 UGen\cf0  \{\
	*kr \{\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 )\
	\}\
\}\
\
\cf2 In\cf0  : \cf2 MultiOutUGen\cf0  \{	\
	*ar \{ \cf2 arg\cf0  bus = 0, numChannels = 1;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , numChannels, bus)\
	\}\
	*kr \{ \cf2 arg\cf0  bus = 0, numChannels = 1;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , numChannels, bus)\
	\}\
	init \{ \cf2 arg\cf0  numChannels ... argBus;\
		inputs = argBus.asArray;\
		^\cf2 this\cf0 .initOutputs(numChannels, rate)\
	\}\
\}\
\
\cf2 InTrig\cf0  : \cf2 MultiOutUGen\cf0  \{	\
	*kr \{ \cf2 arg\cf0  bus = 0, numChannels = 1;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , numChannels, bus)\
	\}\
	init \{ \cf2 arg\cf0  numChannels ... argBus;\
		inputs = argBus.asArray;\
		^\cf2 this\cf0 .initOutputs(numChannels, rate)\
	\}\
\}\
\
\cf2 Out\cf0  : \cf2 UGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  bus, channelsArray;\
		\cf2 this\cf0 .multiNewList([\cf3 'audio'\cf0 , bus] ++ channelsArray.asArray)\
		^0.0		\cf4 // Out has no output\
\cf0 	\}\
	*kr \{ \cf2 arg\cf0  bus, channelsArray;\
		\cf2 this\cf0 .multiNewList([\cf3 'control'\cf0 , bus] ++ channelsArray.asArray)\
		^0.0		\cf4 // Out has no output\
\cf0 	\}\
	numOutputs \{ ^0 \}\
	writeOutputSpecs \{\}\
 	checkInputs \{\
 		if (rate == \cf3 'audio'\cf0 , \{\
 			for(1, inputs.size - 1, \{ \cf2 arg\cf0  i;\
 				if (inputs.at(i).rate != \cf3 'audio'\cf0 , \{ ^\cf2 false\cf0  \});\
 			\});\
 		\});\
 		^\cf2 true\cf0 \
 	\}\
\}\
\
\
\cf2 ReplaceOut\cf0  : \cf2 Out\cf0  \{\}\
\
\
\cf2 XOut\cf0  : \cf2 UGen\cf0  \{\
	*ar \{ \cf2 arg\cf0  bus, xfade, channelsArray;\
		\cf2 this\cf0 .multiNewList([\cf3 'audio'\cf0 , bus, xfade] ++ channelsArray.asArray)\
		^0.0		\cf4 // Out has no output\
\cf0 	\}\
	*kr \{ \cf2 arg\cf0  bus, xfade, channelsArray;\
		\cf2 this\cf0 .multiNewList([\cf3 'control'\cf0 , bus, xfade] ++ channelsArray.asArray)\
		^0.0		\cf4 // Out has no output\
\cf0 	\}\
	numOutputs \{ ^0 \}\
	writeOutputSpecs \{\}\
 	checkInputs \{\
 		if (rate == \cf3 'audio'\cf0 , \{\
 			for(2, inputs.size - 1, \{ \cf2 arg\cf0  i;\
 				if (inputs.at(i).rate != \cf3 'audio'\cf0 , \{ ^\cf2 false\cf0  \});\
 			\});\
 		\});\
 		^\cf2 true\cf0 \
 	\}\
\}\
\
}