{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green115\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 Pan2\cf0  : \cf2 MultiOutUGen\cf0  \{\
	\
	*ar \{ \cf2 arg\cf0  in, pos = 0.0, level = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, pos, level )\
	\}\
	init \{ \cf2 arg\cf0  ... theInputs;\
		inputs = theInputs;		\
		channels = [ \
			\cf2 OutputProxy\cf0 (rate, \cf2 this\cf0 , 0), \
			\cf2 OutputProxy\cf0 (rate, \cf2 this\cf0 , 1) \
		];\
		^channels\
	\}\
\}\
\
\cf2 LinPan2\cf0  : \cf2 Pan2\cf0  \{\}\
\
\cf2 Pan4\cf0  : \cf2 MultiOutUGen\cf0  \{\
	\
	*ar \{ \cf2 arg\cf0  in, xpos = 0.0, ypos = 0.0, level = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, xpos, ypos, level )\
	\}\
	init \{ \cf2 arg\cf0  ... theInputs;\
		inputs = theInputs;		\
		channels = [ \cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 , 0), \cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 , 1),\
					\cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 , 2), \cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 , 3) ];\
		^channels\
	\}\
\}\
\
\
\cf2 PanB\cf0  : \cf2 UGen\cf0  \{\
	\cf2 var\cf0  channels;\
	\
	*ar \{ \cf2 arg\cf0  in, azimuth, elevation, gain;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , in, azimuth, elevation, gain )\
	\}\
	init \{ \cf2 arg\cf0  ... theInputs;\
		inputs = theInputs;		\
		channels = [ \cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 ), \cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 ),\
					\cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 ), \cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 ) ];\
		^channels\
	\}\
\}\
\
\cf2 PanAz\cf0  : \cf2 MultiOutUGen\cf0  \{\
	\
	*ar \{ \cf2 arg\cf0  numChans, in, pos = 0.0, level = 1.0, width = 2.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , numChans, in, pos, level, width )\
	\}\
	init \{ \cf2 arg\cf0  numChans ... theInputs;\
		inputs = theInputs;		\
		channels = \cf2 Array\cf0 .fill(numChans, \{ \cf2 arg\cf0  i; \cf2 OutputProxy\cf0 .ar1(\cf2 this\cf0 , i) \});\
		^channels\
	\}\
\}\
\
\cf2 XFade2\cf0  : \cf2 UGen\cf0  \{\
	\cf4 // equal power two channel cross fade\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	*ar \{ \cf2 arg\cf0  inA, inB = 0.0, pan = 0.0, level = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , inA, inB, pan, level)\
	\}\
	*kr \{ \cf2 arg\cf0  inA, inB = 0.0, pan = 0.0, level = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , inA, inB, pan, level)\
	\}\
\}\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 LinXFade2\cf0  : \cf2 UGen\cf0  \{\
	\cf4 // linear two channel cross fade\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	*ar \{ \cf2 arg\cf0  inA, inB = 0.0, pan = 0.0, level = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'audio'\cf0 , inA, inB, pan, level)\
	\}\
	*kr \{ \cf2 arg\cf0  inA, inB = 0.0, pan = 0.0, level = 1.0;\
		^\cf2 this\cf0 .multiNew(\cf3 'control'\cf0 , inA, inB, pan, level)\
	\}\
\}\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf4 /*\
Pan8 : MultiOutUGen \{\
	\
	*ar \{ arg in, xpos = 0.0, ypos = 0.0, zpos = 0.0, level = 1.0;\
		^this.multiNew('audio', in, xpos, ypos, zpos, level )\
	\}\
	init \{ arg ... theInputs;\
		inputs = theInputs;		\
		channels = [ OutputProxy.ar1(this, 0), OutputProxy.ar1(this, 1),\
					OutputProxy.ar1(this, 2), OutputProxy.ar1(this, 3),\
					OutputProxy.ar1(this, 4), OutputProxy.ar1(this, 5),\
					OutputProxy.ar1(this, 6), OutputProxy.ar1(this, 7) ];\
		^channels\
	\}\
\}\
*/\cf0 \
}