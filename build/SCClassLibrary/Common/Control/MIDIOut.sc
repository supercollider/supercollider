{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 MIDIClient\cf3  \{\
	\cf2 classvar\cf3  < initialized=\cf2 false\cf3 ;\
	*init \{\cf2 arg\cf3  inports, outports;\
		initialized = \cf2 true\cf3 ;\
		\cf2 this\cf3 .prInit(inports,outports);\
	\}\
	*prInit \{\cf2 arg\cf3  inports, outports;\
		\cf2 _InitMIDI\cf3 ;\
	\}\
	*list \{\
		\cf2 _ListMIDIEndpoints\cf3 \
	\}\
	*disposeClient\{\
		\cf2 _DisposeMIDIClient\cf3 \
	\}\
	*restart\{\
		\cf2 _RestartMIDI\cf3 \
	\}\
	\}\
\cf2 MIDIIn\cf3  \{\
	\cf2 var\cf3  <>port;\
	\cf2 classvar\cf3  <>action, \
	<> note, <> polytouch, \
	<> control, <> program, \
	<> touch, <> bend;	\
	\
	\
	*new \{ \cf2 arg\cf3  port;\
		^\cf2 super\cf3 .new.port_(port)\
	\}\
	\
	*doAction\{\cf2 arg\cf3  src, status, a, b, c;\
		action.value(src, status, a, b, c);\
		\}\
	*doNoteAction\{\cf2 arg\cf3  src, chan, num, veloc;\
		\cf4 //note.postln;\
\cf3 		note.value(src, chan, num, veloc);\
	\}\
	*doPolyTochAction\{\cf2 arg\cf3  src, chan, num, val;\
		polytouch.value(src, chan, num, val);\
	\}\
	*doControlAction\{\cf2 arg\cf3  src, chan, num, val;\
		control.value(src, chan, num, val);\
	\}\
	*doProgramAction\{\cf2 arg\cf3  src, chan, val;\
		program.value(src, chan, val);\
	\}\
	*doTouchAction\{\cf2 arg\cf3  src, chan, val;\
		touch.value(src, chan, val);\
	\}\
	*doBendAction\{\cf2 arg\cf3  src, chan, val;\
		bend.value(src, chan, val);\
	\}\
	*connect \{\cf2 arg\cf3  inport, uid;\
		\cf2 _ConnectMIDIIn\cf3 		\
	\}\
	*disconnect \{\cf2 arg\cf3  inport, uid;\
		\cf2 _DisconnectMIDIIn\cf3 		\
	\}\
	\
	\}\
	\
\cf2 MIDIOut\cf3  \{\
	\cf2 var\cf3  <>port, <> uid;\
	\
	*new \{ \cf2 arg\cf3  port, uid;\
		^\cf2 super\cf3 .newCopyArgs(port, uid);\
	\}\
		\
	write \{ \cf2 arg\cf3  len, hiStatus, chan, a=0, b=0;\
		\cf2 this\cf3 .send(port, uid, len, hiStatus, chan, a, b);\
	\}\
	\
	noteOn \{ \cf2 arg\cf3  chan, note=60, veloc=64;\
		\cf2 this\cf3 .write(3, 16r90, chan, note, veloc);\
	\}\
	noteOff \{ \cf2 arg\cf3  chan, note=60, veloc=64;\
		\cf2 this\cf3 .write(3, 16r80, chan, note, veloc);\
	\}\
	polyTouch \{ \cf2 arg\cf3  chan, note=60, val=64;\
		\cf2 this\cf3 .write(3, 16rA0, chan, note, val);\
	\}\
	control \{ \cf2 arg\cf3  chan, ctlNum=7, val=64;\
		\cf2 this\cf3 .write(3, 16rB0, chan, ctlNum, val);\
	\}\
	program \{ \cf2 arg\cf3  chan, num=1;\
		\cf2 this\cf3 .write(2, 16rC0, chan, num);\
	\}\
	touch \{ \cf2 arg\cf3  chan, val=64;\
		\cf2 this\cf3 .write(2, 16rD0, chan, val);\
	\}\
	bend \{ \cf2 arg\cf3  chan, val=64;\
		\cf2 this\cf3 .write(3, 16rE0, chan, val, 0);\
	\}\
	allNotesOff \{ \cf2 arg\cf3  chan;\
		\cf2 this\cf3 .control(chan, 123, 0);\
	\}\
	send \{\cf2 arg\cf3  outport, uid, len, stat, chan, a=0, b=0, latency=5000.0; \cf4 //in ms\
\cf3 		\cf2 _SendMIDIOut\cf3 		\
	\}\
\}\
	}