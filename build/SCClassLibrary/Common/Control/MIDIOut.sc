{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 MIDIIn\cf3  \{\
	\cf2 var\cf3  <>port;\
	\
	*list \{\
		\cf2 _ListMIDIEndpoints\cf3 \
	\}\
	\
	*new \{ \cf2 arg\cf3  port;\
		^\cf2 super\cf3 .new.port_(port)\
	\}\
\}\
\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 MIDIOut\cf3  \{\
	\cf2 var\cf3  <>port;\
	\
	*new \{ \cf2 arg\cf3  port;\
		^\cf2 super\cf3 .new.port_(port)\
	\}\
	\
	write \{ \cf2 arg\cf3  port, len, hiStatus, chan, a=0, b=0;\
		\cf2 _MIDIOut_Write\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	\
	noteOn \{ \cf2 arg\cf3  chan, note=60, veloc=64;\
		\cf2 this\cf3 .write(port, 3, 16r90, chan, note, veloc);\
	\}\
	noteOff \{ \cf2 arg\cf3  chan, note=60, veloc=64;\
		\cf2 this\cf3 .write(port, 3, 16r80, chan, note, veloc);\
	\}\
	polyTouch \{ \cf2 arg\cf3  chan, note=60, val=64;\
		\cf2 this\cf3 .write(port, 3, 16rA0, chan, note, val);\
	\}\
	control \{ \cf2 arg\cf3  chan, ctlNum=7, val=64;\
		\cf2 this\cf3 .write(port, 3, 16rB0, chan, ctlNum, val);\
	\}\
	program \{ \cf2 arg\cf3  chan, num=1;\
		\cf2 this\cf3 .write(port, 2, 16rC0, chan, num);\
	\}\
	touch \{ \cf2 arg\cf3  chan, val=64;\
		\cf2 this\cf3 .write(port, 2, 16rD0, chan, val);\
	\}\
	bend \{ \cf2 arg\cf3  chan, val=64;\
		\cf2 this\cf3 .write(port, 3, 16rE0, chan, val, 0);\
	\}\
	allNotesOff \{ \cf2 arg\cf3  chan;\
		\cf2 this\cf3 .control(chan, 123, 0);\
	\}\
\}\
	}