{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Boolean\cf0  \{\
	*new \{ ^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 ) \}\
	*newClear \{ ^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 ) \}\
	xor \{ \cf2 arg\cf0  bool; ^(\cf2 this\cf0  === bool).not \}\
	if \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	not \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	&& \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	|| \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	and \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	or \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	\
	\
	keywordWarnings \{ \
		\cf3 // turn on/off warnings if a keyword argument is not found\
\cf0 		\cf2 _KeywordError\cf0  \
	\} \
	trace \{ \cf2 _Trace\cf0  \} \cf3 // this is only available in a special debugging version of the app\
\cf0 	tabletTracking \{ \cf2 _TabletTracking\cf0  \} \cf3 // Wacom tablet support\cf0 \
	\cf3 \
\cf0 	printOn \{ \cf2 arg\cf0  stream;\
		stream.putAll(\cf2 this\cf0 .asString);\
	\}\
	storeOn \{ \cf2 arg\cf0  stream;\
		stream.putAll(\cf2 this\cf0 .asCompileString);\
	\}\
\}\
\
\cf2 True\cf0  : \cf2 Boolean\cf0  \{\
	if \{ \cf2 arg\cf0  trueFunc, falseFunc; ^trueFunc.value \}\
	not \{ ^\cf2 false\cf0  \}\
	&& \{ \cf2 arg\cf0  that; ^that.value \}\
	|| \{ \cf2 arg\cf0  that; ^\cf2 this\cf0  \}\
	and \{ \cf2 arg\cf0  function; ^function.value \}\
	or \{ \cf2 arg\cf0  function; ^\cf2 this\cf0  \}\
	binaryValue \{ ^1 \}\
\}\
\
\cf2 False\cf0  : \cf2 Boolean\cf0   \{\
	if \{ \cf2 arg\cf0  trueFunc, falseFunc; ^falseFunc.value \}\
	not \{ ^\cf2 true\cf0  \}\
	&& \{ \cf2 arg\cf0  that; ^\cf2 this\cf0  \}\
	|| \{ \cf2 arg\cf0  that; ^that.value \}\
	and \{ \cf2 arg\cf0  function; ^\cf2 this\cf0  \}\
	or \{ \cf2 arg\cf0  function; ^function.value \}\
	binaryValue \{ ^0 \}\
\}\
}