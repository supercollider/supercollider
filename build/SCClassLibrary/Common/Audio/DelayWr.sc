{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\
\cf2 PingPong\cf0  \{\
	*ar \{ \cf2 arg\cf0  bank, index, inputs, delayTime, feedback=0.7, rotate=1, interpolationType=2;\
	\
		\cf2 var\cf0  indices, delayedSignals, outputs;\
		\
		delayedSignals = \cf2 PlayBuf\cf0 .ar(bank, index, -1, delayTime, interpolationType).postln;	\
\
		outputs = delayedSignals.rotate(rotate) * feedback + inputs;\
		\
		\cf2 RecordBuf\cf0 .ar(bank, index, outputs, run: -1);	\cf3 // feedback to buffers\
\cf0 \
		^outputs\cf3 \
\cf0 	\}\
\}\
\
\
\cf2 DelayProcess\cf0  \{\
	*ar \{ \cf2 arg\cf0  bank, firstIndex, function, inputs, delayTime, interpolationType=2;\
		\cf2 var\cf0  indices, delayedSignals, outputs;\
		//[bank, firstIndex, function, inputs, delayTime, interpolationType].postln;\
		if (delayTime.isKindO\cf2 f(Arr\cf0 ay), \{\
	\cf3 		// need separate buffers\cf0 .\
			\
			inputs = inputs.asArray;\
			indices \cf2 = Arr\cf0 ay.series(inputs.size, firstIndex, 1);\
\
	\cf3 		// tap the delay line\cf0 s\
			delayedSignals \cf2 = PlayB\cf0 uf.ar(bank, indices, -1, delayTime, interpolationType).postln;	\
\
			outputs = function.value(inputs, delayedSignals);\
			\
	\cf3 		// mix the delayed signal with the inpu\cf0 t\
\
	\cf2 		RecordB\cf0 uf.arN(bank, indices, outputs, run: -1)\cf3 ;	// feedback to buffer\cf0 s\
		\},\{\
	\cf3 		// tap the delay line\cf0 s\
			delayedSignals \cf2 = PlayB\cf0 uf.ar(bank, firstIndex, -1, delayTime, interpolationType).postln;	\
\
			outputs = function.value(inputs, delayedSignals);\
			\
	\cf3 		// mix the delayed signal with the inpu\cf0 t\
\
	\cf2 		RecordB\cf0 uf.ar(bank, firstIndex, outputs, run: -1)\cf3 ;	// feedback to buffer\cf0 s\
		\});\
		\
		^output\cf3 s	// output the mixed signal and force the DelayWr into the call grap\cf0 h\
	\}\
\}\
}