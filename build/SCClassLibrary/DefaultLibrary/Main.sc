{\rtf1\mac\ansicpg10000\cocoartf102
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Main\cf0  : \cf2 Process\cf0  \{\
	\
	startUp \{\
		\cf2 super\cf0 .startUp;\
		\cf3 // set the 's' interpreter variable to the default server.\
\cf0 		interpreter.s = \cf2 Server\cf0 .default; \cf3 \
\cf0 \
	\}\
\
	run \{\
	\}\
	stop \{\
		\cf2 SystemClock\cf0 .clear;\
		\cf2 AppClock\cf0 .clear;\
	\}\
	\
	recvOSCmessage \{ \cf2 arg\cf0  time, replyAddr, msg;\
		\cf3 msg.postln;\
\cf0 		\cf3 // this method is called when an OSC message is received.\
\cf0 		\cf2 OSCresponder\cf0 .respond(time, replyAddr, msg);\
	\}\
	\
	recvOSCbundle \{ \cf2 arg\cf0  time, replyAddr ... msgs;\
		\cf3 // this method is called when an OSC bundle is received.\
\cf0 		msgs.do(\{ \cf2 arg\cf0  msg; \
			\cf2 this\cf0 .recvOSCmessage(time, replyAddr, msg); \
		\});\
	\}\
	\
\}\
\
\
}