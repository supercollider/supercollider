{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 // you must not make any change at all to the order or number of \
// instance variables in these classes! \
// You should also not muck with the contents of the instance \
// variables unless you are sure you know what you are doing.\
// You may add methods.\
\cf0 \
\cf2 // Thread inherits from Stream for the benefit of its subclass Routine which can\
// behave like a Stream. Thread itself is not used like a Stream.\
\cf0 \
\cf3 Thread\cf0  : \cf3 Stream\cf0  \{\
	\cf3 var\cf0  <state=0, func, stack, stackSize=0, method, block, frame, ip=0, sp=0;\
	\cf3 var\cf0  top=0, numpop=0, returnLevels=0, receiver, numArgsPushed=0;\
	\cf3 var\cf0  parent, terminalValue;\
	\cf3 var\cf0  <primitiveError=0, <primitiveIndex=0, <randData=0;\
	\cf3 var\cf0  <>time=0.0;\
\
	*new \{ \cf3 arg\cf0  func, stackSize=512;\
		^\cf3 super\cf0 .new.init(func, stackSize)\
	\}\
	init \{ \cf3 arg\cf0  argFunc, argStackSize=512;\
		\cf3 _Thread_Init\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
	\
	randSeed_ \{ \cf3 arg\cf0  seed;\
		\cf2 // You supply an integer seed.\
\cf0 		\cf2 // This method creates a new state vector and stores it in randData.\
\cf0 		\cf2 // A state vector is an Int32Array of three 32 bit words.\
\cf0 		\cf2 // SuperCollider uses the taus88 random number generator which has a \
\cf0 		\cf2 // period of 2**88, and passes all standard statistical tests.\
\cf0 		\cf2 // Normally Threads inherit the randData state vector from the Thread that created it.\
\cf0 		\cf3 _Thread_RandSeed\cf0 \
		^\cf3 this\cf0 .primitiveFailed;\
	\}\
	failedPrimitiveName \{ \cf3 _PrimName\cf0  \}\
	\
	\cf2 // these make Thread act like an Object not like Stream.\
\cf0 	next \{ ^\cf3 this\cf0  \}\
	value \{ ^\cf3 this\cf0  \}\
	valueArray \{ ^\cf3 this\cf0  \}\
\
	*primitiveError \{ \cf3 _PrimitiveError\cf0  \}\
	*primitiveErrorString \{ \cf3 _PrimitiveErrorString\cf0 ; \}\
\}\
\
\cf3 Routine\cf0  : \cf3 Thread\cf0  \{\
	\
	\cf2 // resume, next and value are synonyms\
\cf0 	resume \{ \cf3 arg\cf0  inval;\
		\cf3 _RoutineResume\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
	next \{ \cf3 arg\cf0  inval;\
		\cf3 _RoutineResume\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
	value \{ \cf3 arg\cf0  inval;\
		\cf3 _RoutineResume\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
	valueArray \{ \cf3 arg\cf0  inval;\
		^\cf3 this\cf0 .value(inval) \
	\}\
	\
	reset \{ \cf3 arg\cf0  inval;\
		\cf3 _RoutineReset\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
	stop \{ \cf3 arg\cf0  inval;\
		\cf3 _RoutineStop\cf0 \
		^\cf3 this\cf0 .primitiveFailed\
	\}\
	\
	\cf2 // PRIVATE\
\cf0 	awake \{ arg inTime;\
		time = inTime;\
		^\cf3 this\cf0 .next(inTime)\
	\}\
	prStart \{ \cf3 arg\cf0  inval;\
		func.value(inval);\
		\
		\cf2 // if the user's function returns then always yield nil\
\cf0 		\cf3 nil\cf0 .alwaysYield;\
	\}\
\}\
\
\cf3 Task : Routine \{\}\
\cf0 \
}