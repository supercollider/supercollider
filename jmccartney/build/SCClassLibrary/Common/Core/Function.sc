{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Function\cf3  : \cf2 AbstractFunction\cf3  \{\
	\cf2 var\cf3  <def, context;\
	\cf4 // a Function is what you get when you write a FunctionDef in your code.\
\cf3 	\cf4 // it consists of the function's code and the variables in its defining context\
\cf3 \
	*new \{ ^\cf2 this\cf3 .shouldNotImplement(\cf2 thisMethod\cf3 ) \}\
	*newClear \{ ^\cf2 this\cf3 .shouldNotImplement(\cf2 thisMethod\cf3 ) \}\
	\
	isClosed \{ \cf2 _FunctionIsClosed\cf3  \}\
	\
	shallowCopy \{ ^\cf2 this\cf3  \}\
	\
	choose \{ ^\cf2 this\cf3 .value \}\
	\
	\
	\cf4 // evaluation\
\cf3 	value \{ \cf2 arg\cf3  ... args; \
		\cf2 _FunctionValue\cf3  \
		\cf4 // evaluate a function with args\
\cf3 		^\cf2 this\cf3 .primitiveFailed \
	\}\
	valueArray \{ \cf2 arg\cf3  ... args;\
		\cf2 _FunctionValueArray\cf3  \
		\cf4 // evaluate a function, if the last argument is an array it will be \
\cf3 		\cf4 // expanded into separate args.\
\cf3 		^\cf2 this\cf3 .primitiveFailed \
	\}\
	\
	valueEnvir \{ \cf2 arg\cf3  ... args; \
		\cf2 _FunctionValueEnvir\cf3 \
		\cf4 // evaluate a function with args. \
\cf3 		\cf4 // unsupplied argument names are looked up in the currentEnvironment\
\cf3 		^\cf2 this\cf3 .primitiveFailed \
	\}\
	valueArrayEnvir \{ \cf2 arg\cf3  ... args;\
		\cf2 _FunctionValueArrayEnvir\cf3  \
		\cf4 // evaluate a function, if the last argument is an array it will be \
\cf3 		\cf4 // expanded into separate args.\
\cf3 		\cf4 // unsupplied argument names are looked up in the currentEnvironment\
\cf3 		^\cf2 this\cf3 .primitiveFailed \
	\}\
	\
	loop \{\
		\cf4 // loop is supported magically by the compiler,\
\cf3 		\cf4 // thus it can be implemented in terms of itself\
\cf3 		loop(\{ \cf2 this\cf3 .value \});\
	\}\
	\
	block \{\
		\cf2 this\cf3 .value(\{ \cf2 arg\cf3  val; ^val \});\
	\}\
	\
	asRoutine \{\
		^\cf2 Routine\cf3 .new(\cf2 this\cf3 )\
	\}\
			\
	draw \{ \cf2 this\cf3 .value \}\
\
	dup \{ \cf2 arg\cf3  n = 2;\
		\cf2 var\cf3  array;\
		n.do(\{ \cf2 arg\cf3  i; array = array.add(\cf2 this\cf3 .value(i)) \});\
		^array\
	\}\
	\
	defer \{ \cf2 arg\cf3  delta = 0;\
		\cf2 AppClock\cf3 .sched(delta, \cf2 this\cf3 )\
	\}\
\
	\cf4 // Pattern support\
\cf3 	transformEvent \{ \cf2 arg\cf3  event;\
		^\cf2 this\cf3 .value(event)\
	\}\
\
	\cf4 // ControlView support\
\cf3 	set \{ \cf2 arg\cf3  ... args; ^\cf2 this\cf3 .valueArray(args) \}\
	get \{ \cf2 arg\cf3  prevVal; ^prevVal \}\
\}\
\
\
}