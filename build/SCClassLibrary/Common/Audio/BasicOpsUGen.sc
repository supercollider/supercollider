{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red0\green115\blue0;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 // These Unit Generators are instantiated by math operations on UGens\
\cf0 \
\cf3 BasicOpUGen\cf0  : \cf3 UGen\cf0  \{\
	\cf3 var\cf0  <operator;\
\
\cf2 //	writeName \{ arg file;\
//		var name, opname;\
//		name = this.class.name.asString;\
//		opname = operator.asString;\
//		file.putInt8(name.size + opname.size + 1);\
//		file.putString(name);\
//		file.putInt8(0);\
//		file.putString(opname);\
//	\}\
\cf0 	specialIndex \{ ^operator.specialIndex \}\
\}\
\
\cf3 UnaryOpUGen\cf0  : \cf3 BasicOpUGen\cf0  \{	\
	\
	*new \{ \cf3 arg\cf0  selector, a;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , selector, a)\
	\}\
	\
	init \{ \cf3 arg\cf0  theOperator, theInput;\
		operator = theOperator;\
		rate = theInput.rate;\
		inputs = theInput.asArray;\
	\}\
\
\}\
\
\cf3 BinaryOpUGen\cf0  : \cf3 BasicOpUGen\cf0  \{		\
	*new \{ \cf3 arg\cf0  selector, a, b;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , selector, a, b)\
	\}\
	\
	determineRate \{ \cf3 arg\cf0  a, b;\
		if (a.rate == \cf4 \\audio\cf0 , \{ ^\cf4 \\audio\cf0  \});\
		if (b.rate == \cf4 \\audio\cf0 , \{ ^\cf4 \\audio\cf0  \});\
		if (a.rate == \cf4 \\control\cf0 , \{ ^\cf4 \\control\cf0  \});\
		if (b.rate == \cf4 \\control\cf0 , \{ ^\cf4 \\control\cf0  \});\
		^\cf4 \\scalar\cf0 \
	\}\
	*new1 \{ \cf3 arg\cf0  rate, selector, a, b;\
	\
		\cf2 // eliminate degenerate cases\
\cf0 		if (selector == \cf4 '*'\cf0 , \{\
			if (a == 0.0, \{ ^0.0 \});\
			if (b == 0.0, \{ ^0.0 \});\
			if (a == 1.0, \{ ^b \});\
			if (a == -1.0, \{ ^b.neg \});\
			if (b == 1.0, \{ ^a \});\
			if (b == -1.0, \{ ^a.neg \});\
		\},\{\
		if (selector == \cf4 '+'\cf0 , \{\
			if (a == 0.0, \{ ^b \});\
			if (b == 0.0, \{ ^a \});\
		\},\{\
		if (selector == \cf4 '-'\cf0 , \{\
			if (a == 0.0, \{ ^b.neg \});\
			if (b == 0.0, \{ ^a \});\
		\},\{\
		if (selector == \cf4 '/'\cf0 , \{\
			if (b == 1.0, \{ ^a \});\
			if (b == -1.0, \{ ^a.neg \});\
			if (b.rate == \cf4 'scalar'\cf0 , \{ ^a * b.reciprocal \});\
		\})\})\})\});\
		\
 		^\cf3 super\cf0 .new1(rate, selector, a, b)\
	\}\
	\
	init \{ \cf3 arg\cf0  theOperator, a, b;\
		operator = theOperator;\
		rate = \cf3 this\cf0 .determineRate(a, b);\
		inputs = [a, b];\
	\}\
\}\
\
\cf3 MulAdd\cf0  : \cf3 UGen\cf0  \{\
	*new \{ \cf3 arg\cf0  in, mul = 1.0, add = 0.0;\
		^\cf3 this\cf0 .multiNew(\cf4 'audio'\cf0 , in, mul, add)\
	\}\
	*new1 \{ \cf3 arg\cf0  rate, in, mul, add;\
		\cf3 var\cf0  minus, nomul, noadd;\
\
		\cf2 // eliminate degenerate cases\
\cf0  		if (mul == 0.0, \{ ^add \});\
		minus = mul == -1.0;\
		nomul = mul == 1.0;\
		noadd = add == 0.0;\
 		if (nomul && noadd, \{ ^in \});\
 		if (minus && noadd, \{ ^in.neg \});\
 		if (noadd, \{ ^in * mul \});\
  		if (minus, \{ ^add - in \});\
		if (nomul, \{ ^in + add \});\
 		\
 		^\cf3 super\cf0 .new1(rate, in, mul, add)\
	\}\
	init \{ \cf3 arg\cf0  in, mul, add;\
		rate = in.rate;\
		inputs = [in, mul, add];\
	\}\
\}\
\
\
}