{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red0\green115\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 AbstractFunction\cf0  \{\
	\
	\cf3 // function compositions\
\cf0 	\cf3 // override these in subclasses to perform different kinds of function compositions\
\cf0 	composeUnaryOp \{ \cf2 arg\cf0  aSelector;\
		^\{ \cf2 this\cf0 .value.perform(aSelector) \}\
	\}\
	composeBinaryOp \{ \cf2 arg\cf0  aSelector, function;\
		^\{ \cf2 this\cf0 .value.perform(aSelector, function.value) \}\
	\}\
	reverseComposeBinaryOp \{ \cf2 arg\cf0  aSelector, something;\
		^\{ something.value.perform(aSelector, \cf2 this\cf0 .value) \}\
	\}\
	composeNAryOp \{ \cf2 arg\cf0  aSelector, anArgList;\
		^\{ \cf2 this\cf0 .value.performList(aSelector, anArgList) \}\
	\}\
\
	\cf3 // double dispatch for mixed operations\
\cf0 	performBinaryOpOnSimpleNumber \{ \cf2 arg\cf0  aSelector, aNumber; \
		^\cf2 this\cf0 .reverseComposeBinaryOp(aSelector, aNumber) \
	\}\
	performBinaryOpOnSignal \{ \cf2 arg\cf0  aSelector, aSignal; ^\cf2 this\cf0 .reverseComposeBinaryOp(aSelector, aSignal) \}\
	performBinaryOpOnComplex \{ \cf2 arg\cf0  aSelector, aComplex; ^\cf2 this\cf0 .reverseComposeBinaryOp(aSelector, aComplex) \}\
	performBinaryOpOnSeqColl \{ \cf2 arg\cf0  aSelector, aSeqColl; ^\cf2 this\cf0 .reverseComposeBinaryOp(aSelector, aSeqColl) \}\
	\
	\cf3 // respond to math operators\
\cf0 	neg \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'neg'\cf0 ) \}\
	reciprocal \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'reciprocal'\cf0 ) \}\
	bitNot \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'bitNot'\cf0 ) \}\
	abs \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'abs'\cf0 ) \}\
	asFloat \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'asFloat'\cf0 ) \}\
	asInteger \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'asInteger'\cf0 ) \}\
	ceil \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'ceil'\cf0 ) \}\
	floor \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'floor'\cf0 ) \}\
	frac \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'frac'\cf0 ) \}\
	sign \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'sign'\cf0 ) \}\
	squared \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'squared'\cf0 ) \}\
	cubed \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'cubed'\cf0 ) \}\
	sqrt \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'sqrt'\cf0 ) \}\
	exp \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'exp'\cf0 ) \}\
	midicps \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'midicps'\cf0 ) \}\
	cpsmidi \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'cpsmidi'\cf0 ) \}\
	midiratio \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'midiratio'\cf0 ) \}\
	ratiomidi \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'ratiomidi'\cf0 ) \}\
	ampdb \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'ampdb'\cf0 ) \}\
	dbamp \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'dbamp'\cf0 ) \}	\
	octcps \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'octcps'\cf0 ) \}\
	cpsoct \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'cpsoct'\cf0 ) \}\
	log \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'log'\cf0 ) \}\
	log2 \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'log2'\cf0 ) \}\
	log10 \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'log10'\cf0 ) \}\
	sin \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'sin'\cf0 ) \}\
	cos \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'cos'\cf0 ) \}\
	tan \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'tan'\cf0 ) \}\
	asin \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'asin'\cf0 ) \}\
	acos \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'acos'\cf0 ) \}\
	atan \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'atan'\cf0 ) \}\
	sinh \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'sinh'\cf0 ) \}\
	cosh \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'cosh'\cf0 ) \}\
	tanh \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'tanh'\cf0 ) \}\
	rand \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'rand'\cf0 ) \}\
	rand2 \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'rand2'\cf0 ) \}\
	linrand \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'linrand'\cf0 ) \}\
	bilinrand \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'bilinrand'\cf0 ) \}\
	sum3rand \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'sum3rand'\cf0 ) \}\
\
	distort \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'distort'\cf0 ) \}\
	softclip \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'softclip'\cf0 ) \}\
	nyqring \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'nyqring'\cf0 ) \}\
	coin \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'coin'\cf0 ) \}\
	even \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'even'\cf0 ) \}\
	odd \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'odd'\cf0 ) \}\
\
	rectWindow \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'rectWindow'\cf0 ) \}\
	hanWindow \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'hanWindow'\cf0 ) \}\
	welWindow \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'welWindow'\cf0 ) \}\
	triWindow \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'triWindow'\cf0 ) \}\
\
	scurve \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'scurve'\cf0 ) \}\
	ramp \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'ramp'\cf0 ) \}\
\
	isPositive \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'isPositive'\cf0 ) \}\
	isNegative \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'isNegative'\cf0 ) \}\
	isStrictlyPositive \{ ^\cf2 this\cf0 .composeUnaryOp(\cf4 'isStrictlyPositive'\cf0 ) \}\
\
	\
	+ \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '+'\cf0 , function) \}\
	- \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '-'\cf0 , function) \}\
	* \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '*'\cf0 , function) \}\
	/ \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '/'\cf0 , function) \}\
	div \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'div'\cf0 , function) \}\
	mod \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'mod'\cf0 , function) \}\
	pow \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'pow'\cf0 , function) \}\
	min \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'min'\cf0 , function) \} \
	max \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'max'\cf0 , function) \}\
	\
	<  \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '<'\cf0 ,  function) \}\
	<= \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '<='\cf0 , function) \}\
	>  \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '>'\cf0 ,  function) \}\
	>= \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 '>='\cf0 , function) \}\
	\
	bitAnd \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'bitAnd'\cf0 , function) \}\
	bitOr \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'bitOr'\cf0 , function) \}\
	bitXor \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'bitXor'\cf0 , function) \}	\
	lcm \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'lcm'\cf0 , function) \}\
	gcd \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'gcd'\cf0 , function) \}\
	round \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'round'\cf0 , function) \}\
	trunc \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'trunc'\cf0 , function) \}\
	atan2 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'atan2'\cf0 , function) \}	\
	hypot \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'hypot'\cf0 , function) \}\
	hypotApx \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'hypotApx'\cf0 , function) \}\
	leftShift \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'leftShift'\cf0 , function) \}\
	rightShift \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'rightShift'\cf0 , function) \}\
	unsignedRightShift \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'unsignedRightShift'\cf0 , function) \}\
	ring1 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'ring1'\cf0 , function) \}\
	ring2 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'ring2'\cf0 , function) \}\
	ring3 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'ring3'\cf0 , function) \}\
	ring4 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'ring4'\cf0 , function) \}\
	difsqr \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'difsqr'\cf0 , function) \}\
	sumsqr \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'sumsqr'\cf0 , function) \}\
	sqrsum \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'sqrsum'\cf0 , function) \}\
	sqrdif \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'sqrdif'\cf0 , function) \}\
	absdif \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'absdif'\cf0 , function) \}\
	thresh \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'thresh'\cf0 , function) \}\
	amclip \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'amclip'\cf0 , function) \}\
	scaleneg \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'scaleneg'\cf0 , function) \}\
	clip2 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'clip2'\cf0 , function) \}\
	fold2 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'fold2'\cf0 , function) \}\
	wrap2 \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'wrap2'\cf0 , function) \}\
	excess \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'excess'\cf0 , function) \}\
	firstArg \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'firstArg'\cf0 , function) \}\
	rrand \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'rrand'\cf0 , function) \}\
	exprand \{ \cf2 arg\cf0  function; ^\cf2 this\cf0 .composeBinaryOp(\cf4 'exprand'\cf0 , function) \}\
\
	\cf3 // complex support\
\cf0 	real \{ ^\cf2 this\cf0  \}\
	imag \{ ^0.0 \}\
\
\}\
}