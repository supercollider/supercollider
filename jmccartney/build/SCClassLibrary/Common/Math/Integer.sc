{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red0\green115\blue0;
\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Integer\cf3  : \cf2 SimpleNumber\cf3  \{\
	isInteger \{ ^\cf2 true\cf3  \}	\
		\
	+ \{ \cf2 arg\cf3  aNumber; \cf2 _AddInt\cf3 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf4 '+'\cf3 , \cf2 this\cf3 ) \}\
	- \{ \cf2 arg\cf3  aNumber; \cf2 _SubInt\cf3 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf4 '-'\cf3 , \cf2 this\cf3 ) \}\
	* \{ \cf2 arg\cf3  aNumber; \cf2 _MulInt\cf3 ; ^aNumber.performBinaryOpOnSimpleNumber(\cf4 '*'\cf3 , \cf2 this\cf3 ) \}\
	\
	clip \{ \cf2 arg\cf3  lo, hi; \cf2 _ClipInt\cf3 ; ^\cf2 this\cf3 .primitiveFailed \}\
	wrap \{ \cf2 arg\cf3  lo, hi; \cf2 _WrapInt\cf3 ; ^\cf2 this\cf3 .primitiveFailed \}\
	fold \{ \cf2 arg\cf3  lo, hi; \cf2 _FoldInt\cf3 ; ^\cf2 this\cf3 .primitiveFailed \}\cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 \
	even \{ ^(\cf5 this\cf0  & 1) == 0 \}\
	odd \{ ^(\cf5 this\cf0  & 1) == 1 \}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf3 	\
	xrand \{ \cf2 arg\cf3  exclude=0; \
		^(exclude + (\cf2 this\cf3  - 1).rand + 1) % \cf2 this\cf3 ;\
	\}\
	xrand2 \{ \cf2 arg\cf3  exclude=0; \
		\cf2 var\cf3  res;\
		res = (2 * \cf2 this\cf3 ).rand - \cf2 this\cf3 ;\
		if (res == exclude, \{ ^\cf2 this\cf3  \},\{ ^res \});\
	\}\
	\
	\cf6 // iteration\
\cf3 	do \{ \cf2 arg\cf3  function;\
		\cf6 // iterates function from 0 to this-1 \
\cf3 		\cf6 // special byte codes inserted by compiler for this method\
\cf3 		\cf2 var\cf3  i = 0;\
		while (\{ i < \cf2 this\cf3  \}, \{ function.value(i, i); i = i + 1; \});\
	\}\
\
	reverseDo \{ \cf2 arg\cf3  function;\
		\cf6 // iterates function from 0 to this-1 \
\cf3 		\cf6 // special byte codes inserted by compiler for this method\
\cf3 		\cf2 var\cf3  i=0, j=0;\
		i = \cf2 this\cf3  - 1;\
		while (\{ i >= 0 \}, \{ function.value(i, j); i = i - 1; j = j + 1; \});\
	\}\
	for \{ \cf2 arg\cf3  endval, function;\
		\cf6 // iterates function from this to endval (inclusive)\
\cf3 		\cf6 // special byte codes inserted by compiler for this method\
\cf3 		\cf2 var\cf3  i, j=0;\
		i = \cf2 this\cf3 ;\
		while (\{ i <= endval \}, \{ function.value(i, j); i = i + 1; j = j + 1; \});\
	\}\
	forBy \{ \cf2 arg\cf3  endval, stepval, function;\
		\cf6 // iterates function from this to endval (inclusive) by step\
\cf3 		\cf6 // special byte codes inserted by compiler for this method\
\cf3 		\cf2 var\cf3  i, j=0;\
		i = \cf2 this\cf3 ;\
		while (\{ i <= endval \}, \{ function.value(i, j); i = i + stepval; j = j + 1; \});\
	\}\
\
	to \{ \cf2 arg\cf3  hi, step=1;\
		^\cf2 Interval\cf3 .new(\cf2 this\cf3 , hi, step)\
	\}\
		\
	\cf6 // conversions to Char\
\cf3 	asAscii \{\
		\cf6 // must be 0 <= this <= 255\
\cf3 		\cf2 _AsAscii\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	asDigit \{\
		\cf6 // must be 0 <= this <= 35\
\cf3 		\cf2 _AsDigit\cf3 \
		^\cf2 this\cf3 .primitiveFailed\
	\}\
	\
	nextPowerOfTwo \{ \cf2 _NextPowerOfTwo\cf3  \}\
	isPowerOfTwo \{ \cf2 _IsPowerOfTwo\cf3  \}\
	leadingZeroes \{ \cf2 _CLZ\cf3  \}\
	trailingZeroes \{ \cf2 _CTZ\cf3  \}\
	numBits \{ \cf2 _NumBits\cf3  \}\
	log2Ceil \{ \cf2 _Log2Ceil\cf3  \}\
	setBit \{ \cf2 arg\cf3  bitNumber, bool = \cf2 true\cf3 ; \cf2 _SetBit\cf3  ^\cf2 this\cf3 .primitiveFailed \}\
\
	nthPrime \{ \cf2 _NthPrime\cf3  \}\
	prevPrime \{ \cf2 _PrevPrime\cf3  \}\
	nextPrime \{ \cf2 _NextPrime\cf3  \}\
	\
	\
	isPrime \{\
		\cf2 _IsPrime\cf3 \
		\cf6 /*\
		var sqrtThis;\
		if ( this <= 2, \{\
			if (this == 2, \{ ^true \}, \{ ^false \});\
		\});\
		sqrtThis = this.sqrt.asInteger;\
		256.do(\{ arg i; var p;\
			p = i.nthPrime;\
			if (this % p == 0, \{ ^false \});\
			if (p >= sqrtThis, \{ ^true \});\
		\});\
		^nil\
		*/\cf3 \
	\}\
\
	getKeys \{ \cf2 _GetKeys\cf3  \} \
	\cf6 // code gen\
\cf3 	outputType \{ ^\cf4 \\INT\cf3  \}\
	\
	\cf6 // exit the program and return the result code to unix shell\
\cf3 	exit \{ \cf2 _Exit\cf3  \}\
\}\
}