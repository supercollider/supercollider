{\rtf1\mac\ansicpg10000\cocoartf102
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Boolean\cf3  \{\
	*new \{ ^\cf2 this\cf3 .shouldNotImplement(\cf2 thisMethod\cf3 ) \}\
	*newClear \{ ^\cf2 this\cf3 .shouldNotImplement(\cf2 thisMethod\cf3 ) \}\
	xor \{ \cf2 arg\cf3  bool; ^(\cf2 this\cf3  === bool).not \}\
	if \{ ^\cf2 this\cf3 .subclassResponsibility(\cf2 thisMethod\cf3 ) \}\
	not \{ ^\cf2 this\cf3 .subclassResponsibility(\cf2 thisMethod\cf3 ) \}\
	&& \{ ^\cf2 this\cf3 .subclassResponsibility(\cf2 thisMethod\cf3 ) \}\
	|| \{ ^\cf2 this\cf3 .subclassResponsibility(\cf2 thisMethod\cf3 ) \}\
	and \{ ^\cf2 this\cf3 .subclassResponsibility(\cf2 thisMethod\cf3 ) \}\
	or \{ ^\cf2 this\cf3 .subclassResponsibility(\cf2 thisMethod\cf3 ) \}\
	\
	\
	keywordWarnings \{ \
		\cf4 // turn on/off warnings if a keyword argument is not found\
\cf3 		\cf2 _KeywordError\cf3  \
	\} \
	trace \{ \cf2 _Trace\cf3  \} \cf4 // this is only available in a special debugging version of the app\
\cf3 	tabletTracking \{ \cf2 _TabletTracking\cf3  \} \cf4 // Wacom tablet support\
\cf3 	\
	printOn \{ \cf2 arg\cf3  stream;\
		stream.putAll(\cf2 this\cf3 .asString);\
	\}\
	storeOn \{ \cf2 arg\cf3  stream;\
		stream.putAll(\cf2 this\cf3 .asCompileString);\
	\}\
\}\
\
\cf2 True\cf3  : \cf2 Boolean\cf3  \{\
	if \{ \cf2 arg\cf3  trueFunc, falseFunc; ^trueFunc.value \}\
	not \{ ^\cf2 false\cf3  \}\
	&& \{ \cf2 arg\cf3  that; ^that.value \}\
	|| \{ \cf2 arg\cf3  that; ^\cf2 this\cf3  \}\
	and \{ \cf2 arg\cf3  that; ^that.value \}\
	or \{ \cf2 arg\cf3  that; ^\cf2 this\cf3  \}\
	binaryValue \{ ^1 \}\
\}\
\
\cf2 False\cf3  : \cf2 Boolean\cf3   \{\
	if \{ \cf2 arg\cf3  trueFunc, falseFunc; ^falseFunc.value \}\
	not \{ ^\cf2 true\cf3  \}\
	&& \{ \cf2 arg\cf3  that; ^\cf2 this\cf3  \}\
	|| \{ \cf2 arg\cf3  that; ^that.value \}\
	and \{ \cf2 arg\cf3  that; ^\cf2 this\cf3  \}\
	or \{ \cf2 arg\cf3  that; ^that.value \}\
	binaryValue \{ ^0 \}\
\}\
}