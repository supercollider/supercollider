{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Main\cf3  : \cf2 Process\cf3  \{\
	\
	startUp \{\
		\cf2 super\cf3 .startUp;\
		\cf4 // set the 's' interpreter variable to the default server.\
\cf3 		interpreter.s = \cf2 Server\cf3 .default; \cf4 \
\cf3 \
	\}\
\
	run \{\
	\}\
	stop \{\
		\cf2 SystemClock\cf3 .clear;\
		\cf2 AppClock\cf3 .clear;\
	\}\
	\
	recvOSCmessage \{ \cf2 arg\cf3  time, replyAddr, msg;\
		\cf4 //msg.postln;\
\cf3 		\cf4 // this method is called when an OSC message is received.\
\cf3 		\cf2 OSCresponder\cf3 .respond(time, replyAddr, msg);\
	\}\
	\
	recvOSCbundle \{ \cf2 arg\cf3  time, replyAddr ... msgs;\
		\cf4 // this method is called when an OSC bundle is received.\
\cf3 		msgs.do(\{ \cf2 arg\cf3  msg; \
			\cf2 this\cf3 .recvOSCmessage(time, replyAddr, msg); \
		\});\
	\}\
	\
\}\
\
\
}