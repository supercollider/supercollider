{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 MIDIIn\cf0  \{\
	\cf2 var\cf0  <>port;\
	classvar <>action;	\
	*list \{\
		\cf2 _ListMIDIEndpoints\cf0 \
	\}\
	\
	*new \{ \cf2 arg\cf0  port;\
		^\cf2 super\cf0 .new.port_(port)\
	\}\
	*init \{arg inports, outports;\
		_InitMIDI;\
	\}\
	*doAction\{arg src, status, a, b, c;\
		action.value(src, status, a, b, c);\
		\}\
	*connect \{arg a, b;\
		_ConnectMIDIIn		\
	\}\
	*disposeClient\{\
	_DisposeMIDIClient\
	\}\
	*restart\{\
	_RestartMIDI\
	\}\
	\}\
	\
\cf2 MIDIOut\cf0  \{\
	\cf2 var\cf0  <>port;\
	\
	*new \{ \cf2 arg\cf0  port;\
		^\cf2 super\cf0 .new.port_(port)\
	\}\
	\
	write \{ \cf2 arg\cf0  port, len, hiStatus, chan, a=0, b=0;\
		\cf2 _MIDIOut_Write\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	\
	noteOn \{ \cf2 arg\cf0  chan, note=60, veloc=64;\
		\cf2 this\cf0 .write(port, 3, 16r90, chan, note, veloc);\
	\}\
	noteOff \{ \cf2 arg\cf0  chan, note=60, veloc=64;\
		\cf2 this\cf0 .write(port, 3, 16r80, chan, note, veloc);\
	\}\
	polyTouch \{ \cf2 arg\cf0  chan, note=60, val=64;\
		\cf2 this\cf0 .write(port, 3, 16rA0, chan, note, val);\
	\}\
	control \{ \cf2 arg\cf0  chan, ctlNum=7, val=64;\
		\cf2 this\cf0 .write(port, 3, 16rB0, chan, ctlNum, val);\
	\}\
	program \{ \cf2 arg\cf0  chan, num=1;\
		\cf2 this\cf0 .write(port, 2, 16rC0, chan, num);\
	\}\
	touch \{ \cf2 arg\cf0  chan, val=64;\
		\cf2 this\cf0 .write(port, 2, 16rD0, chan, val);\
	\}\
	bend \{ \cf2 arg\cf0  chan, val=64;\
		\cf2 this\cf0 .write(port, 3, 16rE0, chan, val, 0);\
	\}\
	allNotesOff \{ \cf2 arg\cf0  chan;\
		\cf2 this\cf0 .control(chan, 123, 0);\
	\}\
\}\
	}