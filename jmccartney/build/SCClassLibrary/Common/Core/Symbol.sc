{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Symbol\cf3  \{\
	*new \{ ^\cf2 this\cf3 .shouldNotImplement \}\
\
	\cf4 // conversion\
\cf3 	asSymbol \{ ^\cf2 this\cf3  \}\
	asInteger \{ \
		\cf2 _Symbol_AsInteger\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	asFloat \{ \
		\cf2 _Symbol_AsFloat\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	\cf4 //asCompileString \{ ^super.asString \}\
\cf3 	asClass \{ \
		\cf2 _SymbolClass\cf3  \
		\cf4 // if Symbol represents a class name then return the class, else return nil.\
\cf3 	\}\
	asSetter \{\
		\cf2 _SymbolAsSetter\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	asGetter \{\
		\cf2 _SymbolAsGetter\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	asSpec \{ ^\cf2 Spec\cf3 .specs.at(\cf2 this\cf3 ) \}\
	asWarp \{ \cf2 arg\cf3  spec; ^\cf2 Warp\cf3 .warps.at(\cf2 this\cf3 ).new(spec) \}\
	\
	\cf4 // testing\
\cf3 	isSetter \{\
		\cf4 // returns true if last character of symbol is an underscore\
\cf3 		\cf2 _SymbolIsSetter\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	isClassName \{\
		\cf2 _SymbolIsClassName\cf3 \
		\cf4 // returns true if first character of symbol is a capital letter\
\cf3 	\}\
	isMetaClassName \{\
		\cf2 _SymbolIsMetaClassName\cf3 \
		\cf4 // returns true if there is a meta class by this name\
\cf3 	\}\
\
	openTextFile \{ \cf2 arg\cf3  selectionStart=0, selectionLength=0;\
		^\cf2 this\cf3 .asString.openTextFile(selectionStart, selectionLength)\
	\}\
	\
	\cf4 // Environment support\
\cf3 	\cf4 // The compiler translates use of an Environment variable like ~myvar \
\cf3 	\cf4 // to a call to one of these methods, for example:\
\cf3 	\cf4 // 			~myvar = 5;  translates to:  'myvar'.envirPut(5);\
\cf3 	envirGet \{ ^currentEnvironment.at(\cf2 this\cf3 ) \}\
	envirPut \{ \cf2 arg\cf3  aValue; ^currentEnvironment.put(\cf2 this\cf3 , aValue) \}\
\
	draw \{ \
		\cf2 _Symbol_Draw\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	fontID \{\
		\cf2 _Symbol_GetFontID\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	blend \{ \cf4 // Envelopes may call this on the curves inst var.\
\cf3 		^\cf2 this\cf3 \
	\}\
\
	\cf4 // support for math on symbols\
\cf3 \
	\cf4 // unary ops\
\cf3 	neg \{ ^\cf2 this\cf3  \}\
	bitNot \{ ^\cf2 this\cf3  \}\
	abs \{ ^\cf2 this\cf3  \}\
	ceil \{ ^\cf2 this\cf3  \}\
	floor \{ ^\cf2 this\cf3  \}\
	frac \{ ^\cf2 this\cf3  \}\
	sign \{ ^\cf2 this\cf3  \}\
	sqrt \{ ^\cf2 this\cf3  \}\
	exp \{ ^\cf2 this\cf3  \}\
	midicps \{ ^\cf2 this\cf3  \}\
	cpsmidi \{ ^\cf2 this\cf3  \}\
	midiratio \{ ^\cf2 this\cf3  \}\
	ratiomidi \{ ^\cf2 this\cf3  \}\
	ampdb \{ ^\cf2 this\cf3  \}\
	dbamp \{ ^\cf2 this\cf3  \}	\
	octcps \{ ^\cf2 this\cf3  \}\
	cpsoct \{ ^\cf2 this\cf3  \}\
	log \{ ^\cf2 this\cf3  \}\
	log2 \{ ^\cf2 this\cf3  \}\
	log10 \{ ^\cf2 this\cf3  \}\
	sin \{ ^\cf2 this\cf3  \}\
	cos \{ ^\cf2 this\cf3  \}\
	tan \{ ^\cf2 this\cf3  \}\
	asin \{ ^\cf2 this\cf3  \}\
	acos \{ ^\cf2 this\cf3  \}\
	atan \{ ^\cf2 this\cf3  \}\
	sinh \{ ^\cf2 this\cf3  \}\
	cosh \{ ^\cf2 this\cf3  \}\
	tanh \{ ^\cf2 this\cf3  \}\
	rand \{ ^\cf2 this\cf3  \}\
	rand2 \{ ^\cf2 this\cf3  \}\
	linrand \{ ^\cf2 this\cf3  \}\
	bilinrand \{ ^\cf2 this\cf3  \}\
	sum3rand \{ ^\cf2 this\cf3  \}\
\
	distort \{ ^\cf2 this\cf3  \}\
	softclip \{ ^\cf2 this\cf3  \}\
	nyqring \{ ^\cf2 this\cf3  \}\
	coin \{ ^\cf2 this\cf3  \}\
	even \{ ^\cf2 this\cf3  \}\
	odd \{ ^\cf2 this\cf3  \}\
\
	rectWindow \{ ^\cf2 this\cf3  \}\
	hanWindow \{ ^\cf2 this\cf3  \}\
	welWindow \{ ^\cf2 this\cf3  \}\
	triWindow \{ ^\cf2 this\cf3  \}\
\
	scurve \{ ^\cf2 this\cf3  \}\
	ramp \{ ^\cf2 this\cf3  \}\
\
	\cf4 // binary ops\
\cf3 	+ \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}		\
	- \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \} \
	* \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \} \
	/ \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \} \
	mod \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	min \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \} \
	max \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	bitAnd \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	bitOr \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	bitXor \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}	\
	lcm \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	gcd \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	round \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	trunc \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	atan2 \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}	\
	hypot \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	hypotApx \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	pow \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	leftShift \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	rightShift \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	unsignedRightShift \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	rrand \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	exprand \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	\
	degreeToKey \{ ^\cf2 this\cf3  \}\
	\
	doNumberOp \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	doComplexOp \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	doSignalOp \{ \cf2 arg\cf3  x; ^\cf2 this\cf3  \}\
	doListOp \{ \cf2 arg\cf3  aSelector, aList; \
		aList.collect(\{ \cf2 arg\cf3  item; \
			item.perform(aSelector, \cf2 this\cf3 )\
		\}) \
	\}\
\
	specialIndex \{ \
		\cf4 // used by BasicOpUGens to get an ID number for the operator\
\cf3 		\cf2 _Symbol_SpecialIndex\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
\
	printOn \{ \cf2 arg\cf3  stream;\
		stream.putAll(\cf2 this\cf3 .asString);\
	\}\
	storeOn \{ \cf2 arg\cf3  stream;\
		stream.putAll(\cf2 this\cf3 .asCompileString);\
	\}\
	\
	\cf4 // code gen\
\cf3 	codegen_UGenCtorArg \{ \cf2 arg\cf3  stream; \
		\cf2 this\cf3 .asString.codegen_UGenCtorArg(stream);\
	\}\
\}\
}