{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 Condition\cf0  \{\
	\cf2 var\cf0  <>func, waitingThreads;\
	\
	*new \{ \cf2 arg\cf0  func;\
		^\cf2 super\cf0 .newCopyArgs(func)\
	\}\
	wait \{ \cf2 arg\cf0  value;\
		if (func.value.not, \{\
			waitingThreads = waitingThreads.add(\cf2 thisThread\cf0 );\
			value.yield;\
		\});\
	\}\
	signal \{\
		var tempWaitingThreads, time;\
		if (func.value, \{		\
			time = thisThread.time;\
			tempWaitingThreads = waitingThreads;\
			waitingThreads = nil;\
			tempWaitingThreads.do(\{ arg thread; \
				thread.time = time;\
				thread.resume;\
			\});\
		\});\
	\}\
\}\
\
\
\cf2 Wait\cf0  \{\
	\cf2 var\cf0  waitingThreads;\
	\
	wait \{ \cf2 arg\cf0  value;\
		waitingThreads = waitingThreads.add(\cf2 thisThread\cf0 );\
		value.yield;\
	\}\
	signal \{\
		var tempWaitingThreads, time;\
		time = thisThread.time;\
		tempWaitingThreads = waitingThreads;\
		waitingThreads = nil;\
		tempWaitingThreads.do(\{ arg thread; \
			thread.time = time;\
			thread.resume;\
		\});\
	\}\
\}}