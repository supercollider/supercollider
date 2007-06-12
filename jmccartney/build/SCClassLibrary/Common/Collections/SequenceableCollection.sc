{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 SequenceableCollection\cf3  : \cf2 Collection\cf3  \{\
\
	\cf4 // synonyms\
\cf3 	|@| \{ \cf2 arg\cf3  index; ^\cf2 this\cf3 .clipAt(index) \}\
	@@ \{ \cf2 arg\cf3  index; ^\cf2 this\cf3 .wrapAt(index) \}\
	@|@ \{ \cf2 arg\cf3  index; ^\cf2 this\cf3 .foldAt(index) \}\
\
	\cf4 // fill with ramp of values\
\cf3 	*series \{ \cf2 arg\cf3  size, start=0, step=1;\
		\cf2 var\cf3  i=0, obj;\
		obj = \cf2 this\cf3 .new(size);\
		while (\{ i < size \},\{\
			obj.add(start);\
			start = start + step;\
			i = i + 1;\
		\});\
		^obj\
	\}		\
	\cf4 // fill with geometric series\
\cf3 	*geom \{ \cf2 arg\cf3  size, start, grow;\
		\cf2 var\cf3  i=0, obj;\
		obj = \cf2 this\cf3 .new(size);\
		while (\{ i < size \},\{\
			obj.add(start);\
			start = start * grow;\
			i = i + 1;\
		\});\
		^obj\
	\}		\
	*rand \{ \cf2 arg\cf3  size, minVal, maxVal;\
		\cf2 var\cf3  i=0, obj;\
		obj = \cf2 this\cf3 .new(size);\
		while (\{ i < size \},\{\
			obj.add(rrand(minVal, maxVal));\
			i = i + 1;\
		\});\
		^obj\
	\}\
	*rand2 \{ \cf2 arg\cf3  size, val;\
		\cf2 var\cf3  i=0, obj;\
		obj = \cf2 this\cf3 .new(size);\
		while (\{ i < size \},\{\
			obj.add(val.rand2);\
			i = i + 1;\
		\});\
		^obj\
	\}\
	*linrand \{ \cf2 arg\cf3  size, minVal, maxVal;\
		\cf2 var\cf3  i=0, obj, range;\
		range = maxVal - minVal;\
		obj = \cf2 this\cf3 .new(size);\
		while (\{ i < size \},\{\
			obj.add(minVal + range.linrand);\
			i = i + 1;\
		\});\
		^obj\
	\}\
\
	++ \{ \cf2 arg\cf3  aSequentialCollection; \
		\cf2 var\cf3  newlist;\
		newlist = \cf2 this\cf3 .species.new(\cf2 this\cf3 .size + aSequentialCollection.size);\
		newlist = newlist.addAll(\cf2 this\cf3 ).addAll(aSequentialCollection);\
		^newlist\
	\}\
	\
	\cf4 // select an element at random\
\cf3 	choose \{ \
		^\cf2 this\cf3 .at(\cf2 this\cf3 .size.rand) \
	\}\
	\cf4 // select an element at random using an array of weights\
\cf3 	wchoose \{ \cf2 arg\cf3  weights;\
		^\cf2 this\cf3 .at(weights.windex) \
	\}\
\
	copyRange \{ \cf2 arg\cf3  start, end;\
		\cf2 var\cf3  newColl, i;\
		i = start;\
		newColl = \cf2 this\cf3 .species.new(end - start);\
		while (\{ i < end \},\{\
			newColl.add(\cf2 this\cf3 .at(i));\
			i = i + 1;\
		\});\
		^newColl\
	\}\
	copyToEnd \{ \cf2 arg\cf3  start;\
		^\cf2 this\cf3 .copyRange(start, \cf2 this\cf3 .size - 1)\
	\}\
	copyFromStart \{ \cf2 arg\cf3  end;\
		^\cf2 this\cf3 .copyRange(0, end)\
	\}\
\
	indexOf \{ \cf2 arg\cf3  item;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  elem, i;\
			if ( item === elem, \{ ^i \})\
		\});\
		^\cf2 nil\cf3 \
	\}\
	indexOfEqual \{ \cf2 arg\cf3  item;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  elem, i;\
			if ( item == elem, \{ ^i \})\
		\});\
		^\cf2 nil\cf3 \
	\}\
	remove \{ \cf2 arg\cf3  item;\
		\cf2 var\cf3  index;\
		index = \cf2 this\cf3 .indexOf(item);\
		^if ( index.notNil, \{\
			\cf2 this\cf3 .removeAt(index);\
		\},\{\
			\cf2 nil\cf3 \
		\});\
	\}\
	take \{ \cf2 arg\cf3  item;\
		\cf2 var\cf3  index;\
		index = \cf2 this\cf3 .indexOf(item);\
		^if ( index.notNil, \{\
			\cf2 this\cf3 .takeAt(index);\
		\},\{\
			\cf2 nil\cf3 \
		\});\
	\}\
	\
	\cf4 // accessing\
\cf3 	first \{ if (\cf2 this\cf3 .size > 0, \{ ^\cf2 this\cf3 .at(0) \}, \{ ^\cf2 nil\cf3  \}) \}\
	last \{ if (\cf2 this\cf3 .size > 0, \{ ^\cf2 this\cf3 .at(\cf2 this\cf3 .size - 1) \}, \{ ^\cf2 nil\cf3  \}) \}\
	top \{ ^\cf2 this\cf3 .last \}\
	putFirst \{ \cf2 arg\cf3  obj; if (\cf2 this\cf3 .size > 0, \{ ^\cf2 this\cf3 .put(0, obj) \}) \}\
	putLast \{ \cf2 arg\cf3  obj; if (\cf2 this\cf3 .size > 0, \{ ^\cf2 this\cf3 .put(\cf2 this\cf3 .size - 1, obj) \}) \}\
\
	\cf4 // ordering\
\cf3 	doAdjacentPairs \{ \cf2 arg\cf3  function;\
		(\cf2 this\cf3 .size - 1).do(\{ \cf2 arg\cf3  i;\
			function.value(\cf2 this\cf3 .at(i), \cf2 this\cf3 .at(i+1), i);\
		\})\
	\}\
	separate \{ \cf2 arg\cf3  function;\
		\cf2 var\cf3  list, sublist;\
		list = \cf2 Array\cf3 .new;\
		sublist = \cf2 this\cf3 .species.new;\
		\cf2 this\cf3 .doAdjacentPairs(\{ \cf2 arg\cf3  a, b, i;\
			sublist = sublist.add(a);\
			if ( function.value(a, b, i), \{\
				list = list.add(sublist);\
				sublist = \cf2 this\cf3 .species.new;\
			\});\
		\});\
		sublist = sublist.add(\cf2 this\cf3 .last);\
		list = list.add(sublist);\
		^list\
	\}\
	delimit \{ \cf2 arg\cf3  function;\
		\cf2 var\cf3  list, sublist;\
		list = \cf2 Array\cf3 .new;\
		sublist = \cf2 this\cf3 .species.new;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item, i;\
			if ( function.value(item, i), \{\
				list = list.add(sublist);\
				sublist = \cf2 this\cf3 .species.new;\
			\},\{\
				sublist = sublist.add(item);\
			\});\
		\});\
		list = list.add(sublist);\
		^list\
	\}\
	clump \{ \cf2 arg\cf3  groupSize;\
		\cf2 var\cf3  list, sublist;\
		list = \cf2 Array\cf3 .new;\
		sublist = \cf2 this\cf3 .species.new;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item;\
			sublist = sublist.add(item);\
			if (sublist.size >= groupSize, \{\
				list = list.add(sublist);\
				sublist = \cf2 this\cf3 .species.new;\
			\});\
		\});\
		if (sublist.size > 0, \{ list = list.add(sublist); \});\
		^list\
	\}\
	clumps \{ \cf2 arg\cf3  groupSizeList;\
		\cf2 var\cf3  list, sublist, i=0;\
		list = \cf2 Array\cf3 .new;\
		sublist = \cf2 this\cf3 .species.new;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item;\
			sublist = sublist.add(item);\
			if (sublist.size >= groupSizeList.wrapAt(i), \{\
				i = i + 1;\
				list = list.add(sublist);\
				sublist = \cf2 this\cf3 .species.new;\
			\});\
		\});\
		if (sublist.size > 0, \{ list = list.add(sublist); \});\
		^list\
	\}\
	curdle \{ \cf2 arg\cf3  probability;\
		^\cf2 this\cf3 .separate(\{ probability.coin \});\
	\}\
	flatten \{ \cf2 arg\cf3  numLevels=1;\
		\cf2 var\cf3  list;\
		\
		if (numLevels <= 0, \{ ^\cf2 this\cf3  \});\
		numLevels = numLevels - 1;\
		\
		list = \cf2 this\cf3 .species.new;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item;\
			if (item.respondsTo(\cf5 'flatten'\cf3 ), \{\
				list = list.addAll(item.flatten(numLevels));\
			\},\{\
				list = list.add(item);\
			\});\
		\});\
		^list\
	\}\
	\
	flat \{\
		\cf2 var\cf3  list;	\
		list = \cf2 this\cf3 .species.new;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item, i;\
			if (item.respondsTo(\cf5 'flat'\cf3 ), \{\
				list = list.addAll(item.flat);\
			\},\{\
				list = list.add(item);\
			\});\
		\});\
		^list\
	\}\
\
	flop \{\
		\cf2 var\cf3  list, size, maxsize;\
		\
		size = \cf2 this\cf3 .size;\
		maxsize = 0;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  sublist;\
			\cf2 var\cf3  sz;\
			sz = if (sublist.isKindOf(\cf2 SequenceableCollection\cf3 ), \{ sublist.size \},\{ 1 \});\
			if (sz > maxsize, \{ maxsize = sz \});\
		\});\
						 \
		list = \cf2 this\cf3 .species.fill(maxsize, \{ \cf2 this\cf3 .species.new(size) \});\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  isublist, i;\
			if (isublist.isKindOf(\cf2 SequenceableCollection\cf3 ), \{\
				list.do(\{ \cf2 arg\cf3  jsublist, j;\
					jsublist.add( isublist.wrapAt(j); );\
				\});\
			\},\{\
				list.do(\{ \cf2 arg\cf3  jsublist, j;\
					jsublist.add( isublist );\
				\});\
			\});\
		\});\
		^list\
	\}\
	unlace \{ \cf2 arg\cf3  numlists;\
		\cf2 var\cf3  size, list, sublist;\
		\
		size = (\cf2 this\cf3 .size + numlists - 1) div: numlists;\
		list = \cf2 this\cf3 .species.fill(numlists, \{ \cf2 this\cf3 .species.new(size) \});\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item, i;\
			sublist = list.at(i % numlists);\
			sublist.add(item);\
		\});\
		^list\
	\}\
		\
	\cf4 // pitch operations\
\cf3 	degreeToKey \{ \cf2 arg\cf3  scale, stepsPerOctave=12;\
		\cf2 var\cf3  size;\
		size = scale.size;\
		^\cf2 this\cf3 .collect(\{ \cf2 arg\cf3  scaleDegree;\
			scaleDegree = scaleDegree.asInteger;\
			(stepsPerOctave * (scaleDegree div: size)) + scale.wrapAt(scaleDegree)\
		\});\
	\}\
	transposeKey \{ \cf2 arg\cf3  amount, octave=12;\
		^((\cf2 this\cf3  + amount) % octave).sort\
	\}\
	mode \{ \cf2 arg\cf3  degree, octave=12;\
		\cf2 var\cf3  transpose;\
		transpose = \cf2 this\cf3 .at(degree);\
		^((\cf2 this\cf3  - transpose) % octave)\
	\}\
	zxmode \{ \cf2 arg\cf3  degree, octave=12;\
		\cf2 var\cf3  transpose, i=0, j, size, newColl;\
\
		transpose = \cf2 this\cf3 .at(degree);\
		size = \cf2 this\cf3 .size;\
		newColl = \cf2 this\cf3 .class.new(size);\
		j = degree;\
		while (\{ i < size \},\{\
			newColl.add((\cf2 this\cf3 .at(j) - transpose) % octave);\
			j = j + 1;\
			if (j >= size, \{ j = 0 \});\
		\});\
		^newColl;\
	\}\
	\
	\cf4 // supports a variation of Mikael Laurson's rhythm lists.\
\cf3 	convertRhythm \{\
		\cf2 var\cf3  list, tie;\
		list = \cf2 List\cf3 .new;\
		tie = \cf2 this\cf3 .convertOneRhythm(list);\
		if (tie > 0.0, \{ list.add(tie) \});  \cf4 // check for tie at end of rhythm\
\cf3 		^list\
	\}\
	convertOneRhythm \{ \cf2 arg\cf3  list, tie = 0.0, stretch = 1.0;\
		\cf2 var\cf3  beats, divisions, repeats;\
		\
		#beats, divisions, repeats = \cf2 this\cf3 ;\
		stretch = stretch * beats / divisions.sum;\
		repeats.do(\{\
			divisions.do(\{ \cf2 arg\cf3  val;\
				if (val.isKindOf(\cf2 SequenceableCollection\cf3 ), \{\
					tie = val.convertOneRhythm(list, tie, stretch)\
				\},\{\
					val = val * stretch;\
					if (val > 0.0, \{ \
						list.add(val + tie); \
						tie = 0.0; \
					\},\{ \
						tie = tie - val \
					\});\
				\});\
			\});\
		\});\
		^tie\
	\}\
\
	\cf4 // misc math\
\cf3 	blend \{ \cf2 arg\cf3  anotherSeqColl, blendFrac = 0.5;\
		\cf4 // this blends between two collections assumed to contain numbers\
\cf3 		\cf4 // blendFrac should be from zero to one\
\cf3 		^\cf2 this\cf3 .collect(\{ \cf2 arg\cf3  item, i; item.blend(anotherSeqColl.at(i), blendFrac); \});\
	\}\
		\
	\cf4 // unary math ops\
\cf3 	neg \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'neg'\cf3 ) \}\
	bitNot \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'bitNot'\cf3 ) \}\
	abs \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'abs'\cf3 ) \}\
	ceil \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'ceil'\cf3 ) \}\
	floor \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'floor'\cf3 ) \}\
	frac \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'frac'\cf3 ) \}\
	sign \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'sign'\cf3 ) \}\
	squared \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'squared'\cf3 ) \}\
	cubed \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'cubed'\cf3 ) \}\
	sqrt \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'sqrt'\cf3 ) \}\
	exp \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'exp'\cf3 ) \}\
	reciprocal \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'reciprocal'\cf3 ) \}\
	midicps \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'midicps'\cf3 ) \}\
	cpsmidi \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'cpsmidi'\cf3 ) \}\
	midiratio \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'midiratio'\cf3 ) \}\
	ratiomidi \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'ratiomidi'\cf3 ) \}\
	ampdb \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'ampdb'\cf3 ) \}\
	dbamp \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'dbamp'\cf3 ) \}	\
	octcps \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'octcps'\cf3 ) \}\
	cpsoct \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'cpsoct'\cf3 ) \}\
	log \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'log'\cf3 ) \}\
	log2 \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'log2'\cf3 ) \}\
	log10 \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'log10'\cf3 ) \}\
	sin \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'sin'\cf3 ) \}\
	cos \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'cos'\cf3 ) \}\
	tan \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'tan'\cf3 ) \}\
	asin \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'asin'\cf3 ) \}\
	acos \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'acos'\cf3 ) \}\
	atan \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'atan'\cf3 ) \}\
	sinh \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'sinh'\cf3 ) \}\
	cosh \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'cosh'\cf3 ) \}\
	tanh \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'tanh'\cf3 ) \}\
	rand \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'rand'\cf3 ) \}\
	rand2 \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'rand2'\cf3 ) \}\
	linrand \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'linrand'\cf3 ) \}\
	bilinrand \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'bilinrand'\cf3 ) \}\
	sum3rand \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'sum3rand'\cf3 ) \}\
\
	distort \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'distort'\cf3 ) \}\
	softclip \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'softclip'\cf3 ) \}\
	nyqring \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'nyqring'\cf3 ) \}\
	coin \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'coin'\cf3 ) \}\
	even \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'even'\cf3 ) \}\
	odd \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'odd'\cf3 ) \}\
	isPositive \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'isPositive'\cf3 ) \}\
	isNegative \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'isNegative'\cf3 ) \}\
	isStrictlyPositive \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'isStrictlyPositive'\cf3 ) \}\
\
	rectWindow \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'rectWindow'\cf3 ) \}\
	hanWindow \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'hanWindow'\cf3 ) \}\
	welWindow \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'welWindow'\cf3 ) \}\
	triWindow \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'triWindow'\cf3 ) \}\
\
	scurve \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'scurve'\cf3 ) \}\
	ramp \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'ramp'\cf3 ) \}\
\
	asFloat \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'asFloat'\cf3 ) \}\
	asInteger \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'asInteger'\cf3 ) \}\
	\
	real \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'real'\cf3 ) \}\
	imag \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'imag'\cf3 ) \}\
\
	magnitude \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'magnitude'\cf3 ) \}\
	magnitudeApx \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'magnitudeApx'\cf3 ) \}\
	phase \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'phase'\cf3 ) \}\
	angle \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'angle'\cf3 ) \}\
\
	rho \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'rho'\cf3 ) \}\
	theta \{ ^\cf2 this\cf3 .performUnaryOp(\cf5 'theta'\cf3 ) \}\
	\
	\cf4 // binary math ops\
\cf3 	+ \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '+'\cf3 , aNumber) \}\
	- \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '-'\cf3 , aNumber) \}\
	* \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '*'\cf3 , aNumber) \}\
	/ \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '/'\cf3 , aNumber) \}\
	div \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'div'\cf3 , aNumber) \}\
	mod \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'mod'\cf3 , aNumber) \}\
	pow \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'pow'\cf3 , aNumber) \}\
	min \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'min'\cf3 , aNumber) \} \
	max \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'max'\cf3 , aNumber) \}\
\
	\
	<  \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '<'\cf3 ,  aNumber) \}\
	<= \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '<='\cf3 , aNumber) \}\
	>  \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '>'\cf3 ,  aNumber) \}\
	>= \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 '>='\cf3 , aNumber) \}\
\
	bitAnd \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'bitAnd'\cf3 , aNumber) \}\
	bitOr \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'bitOr'\cf3 , aNumber) \}\
	bitXor \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'bitXor'\cf3 , aNumber) \}	\
	lcm \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'lcm'\cf3 , aNumber) \}\
	gcd \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'gcd'\cf3 , aNumber) \}\
	round \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'round'\cf3 , aNumber) \}\
	trunc \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'trunc'\cf3 , aNumber) \}\
	atan2 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'atan2'\cf3 , aNumber) \}	\
	hypot \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'hypot'\cf3 , aNumber) \}\
	hypotApx \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'hypotApx'\cf3 , aNumber) \}\
	leftShift \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'leftShift'\cf3 , aNumber) \}\
	rightShift \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'rightShift'\cf3 , aNumber) \}\
	unsignedRightShift \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'unsignedRightShift'\cf3 , aNumber) \}\
	ring1 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'ring1'\cf3 , aNumber) \}\
	ring2 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'ring2'\cf3 , aNumber) \}\
	ring3 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'ring3'\cf3 , aNumber) \}\
	ring4 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'ring4'\cf3 , aNumber) \}\
	difsqr \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'difsqr'\cf3 , aNumber) \}\
	sumsqr \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'sumsqr'\cf3 , aNumber) \}\
	sqrsum \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'sqrsum'\cf3 , aNumber) \}\
	sqrdif \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'sqrdif'\cf3 , aNumber) \}\
	absdif \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'absdif'\cf3 , aNumber) \}\
	thresh \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'thresh'\cf3 , aNumber) \}\
	amclip \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'amclip'\cf3 , aNumber) \}\
	scaleneg \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'scaleneg'\cf3 , aNumber) \}\
	clip2 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'clip2'\cf3 , aNumber) \}\
	fold2 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'fold2'\cf3 , aNumber) \}\
	wrap2 \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'wrap2'\cf3 , aNumber) \}\
	excess \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'excess'\cf3 , aNumber) \}\
	firstArg \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'firstArg'\cf3 , aNumber) \}\
	rrand \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'rrand'\cf3 , aNumber) \}\
	exprand \{ \cf2 arg\cf3  aNumber; ^\cf2 this\cf3 .performBinaryOp(\cf5 'exprand'\cf3 , aNumber) \}\
\
	\cf4 // math op dispatch support\
\cf3 	performUnaryOp \{ \cf2 arg\cf3  aSelector;\
		^\cf2 this\cf3 .collect(\{ \cf2 arg\cf3  item; item.perform(aSelector) \});\
	\}\
\
	performBinaryOp \{ \cf2 arg\cf3  aSelector, theOperand;\
		\cf2 var\cf3  i, minSize, newList;\
		^theOperand.performBinaryOpOnSeqColl(aSelector, \cf2 this\cf3 );\
	\}\
	performBinaryOpOnSeqColl \{ \cf2 arg\cf3  aSelector, theOperand;\
		\cf2 var\cf3  minSize, newList;\
		minSize = \cf2 this\cf3 .size min: theOperand.size;\
		newList = \cf2 this\cf3 .species.new(minSize);\
		minSize.do(\{ \cf2 arg\cf3  i;\
			newList.add(theOperand.at(i).perform(aSelector, \cf2 this\cf3 .at(i)));\
		\});\
		^newList\
	\}\
	performBinaryOpOnSimpleNumber \{ \cf2 arg\cf3  aSelector, aNumber; \
		^\cf2 this\cf3 .collect(\{ \cf2 arg\cf3  item; \
			aNumber.perform(aSelector, item)\
		\}) \
	\}\
	performBinaryOpOnComplex \{ \cf2 arg\cf3  aSelector, aComplex; \
		^\cf2 this\cf3 .collect(\{ \cf2 arg\cf3  item; \
			aComplex.perform(aSelector, item)\
		\}) \
	\}\
	performBinaryOpOnInfinitum \{ \cf2 arg\cf3  aSelector, aNumber; \
		^\cf2 this\cf3 .collect(\{ \cf2 arg\cf3  item; \
			aNumber.perform(aSelector, item)\
		\}) \
	\}\
	\
	\cf4 // support UGen clock division arguments.\
\cf3 	cdiv \{ ^\cf2 this\cf3 .collect(\{ \cf2 arg\cf3  item; item.cdiv; \}); \}\
	minDiv \{\
		\cf2 var\cf3  div;\
		div = \cf2 UGen\cf3 .scalarRate;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item; div = min(div, item.cdiv) \});\
		^div\
	\}\
	maxDiv \{\
		\cf2 var\cf3  div;\
		div = 1;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item; div = max(div, item.cdiv) \});\
		^div\
	\}\
	\
	\cf4 // sorting\
\cf3 	sort \{ \cf2 arg\cf3  function; \
		if (function.isNil, \{ function = \{ \cf2 arg\cf3  a, b; a < b \}; \}); \
		\cf2 this\cf3 .sortRange(0, \cf2 this\cf3 .size - 1, function) \
	\}\
	\
	swap \{ \cf2 arg\cf3  i, j;\
		\cf2 var\cf3  temp;\
		temp = \cf2 this\cf3 .at(i);\
		\cf2 this\cf3 .put(i, \cf2 this\cf3 .at(j));\
		\cf2 this\cf3 .put(j, temp);\
	\}\
	\
	sortRange \{ \cf2 arg\cf3  i, j, function;\
		\cf4 //Sort elements i through j of this to be nondescending according to\
\cf3 		\cf4 // function.\
\cf3 		\cf2 var\cf3  di, dij, dj, tt, ij, k, l, n;\
				\
		\cf4 // The prefix d means the data at that index.\
\cf3 		if ((n = j + 1  - i) <= 1, \{ ^\cf2 this\cf3  \});	\cf4 // Nothing to sort.\
\cf3 		\
		\cf4 //Sort di,dj.\
\cf3 		di = \cf2 this\cf3 .at(i);\
		dj = \cf2 this\cf3 .at(j);\
		if (function.value(di, dj).not, \{ \cf4 // i.e., should di precede dj?\
\cf3 			\cf2 this\cf3 .swap(i,j);\
				 tt = di;\
				 di = dj;\
				 dj = tt;\
		\});\
		if ( n > 2, \{ \cf4 // More than two elements.\
\cf3 			ij = (i + j) div: 2;  \cf4 // ij is the midpoint of i and j.\
\cf3 			dij = \cf2 this\cf3 .at(ij);  \cf4 // Sort di,dij,dj.  Make dij be their median.\
\cf3 			if (function.value(di,  dij), \{  \cf4 // i.e. should di precede dij?\
\cf3 				if (function.value(dij, dj).not, \{  \cf4 // i.e., should dij precede dj?\
\cf3 					\cf2 this\cf3 .swap(j, ij);\
					dij = dj;\
				\})\
			\},\{ \cf4 // i.e. di should come after dij"\
\cf3 				\cf2 this\cf3 .swap(i, ij);\
				dij = di;\
			\});\
			if ( n > 3, \{ \cf4 // More than three elements.\
\cf3 				\cf4 // Find k>i and l<j such that dk,dij,dl are in reverse order.\
\cf3 				\cf4 // Swap k and l.  Repeat this procedure until k and l pass each other.\
\cf3 				k = i;\
				l = j;\
				while (\{\
				 	while (\{\
				 		l = l - 1;  \
				 		k <= l and: \{ function.value(dij, \cf2 this\cf3 .at(l)) \}\
				 	\}); \cf4 // i.e. while dl succeeds dij\
\cf3 				 	while (\{\
				 		k = k + 1;  \
				 		k <= l and: \{ function.value(\cf2 this\cf3 .at(k), dij) \};\
				 	\}); \cf4 // i.e. while dij succeeds dk\
\cf3 				 	k <= l\
				\},\{\
					\cf2 this\cf3 .swap(k, l); \
				\});\
		\cf4 // Now l<k (either 1 or 2 less), and di through dl are all less than or equal to dk\
\cf3 		\cf4 // through dj.  Sort those two segments.\
\cf3 				\cf2 this\cf3 .sortRange(i, l, function);\
				\cf2 this\cf3 .sortRange(k, j, function);\
			\});\
		\});\
	\}\
\
	\cf4 // streaming\
\cf3 	*streamContents \{ \cf2 arg\cf3  function;\
		\cf2 var\cf3  stream;\
		stream = \cf2 CollStream\cf3 .on(\cf2 this\cf3 .new(100));\
		function.value(stream);\
		^stream.contents\
	\}\
	*streamContentsLimit \{ \cf2 arg\cf3  function, limit=2000;\
		\cf2 var\cf3  stream;\
		stream = \cf2 LimitedWriteStream\cf3 .on(\cf2 this\cf3 .new(100 min: limit));\
		stream.limit_(limit).limitFunc_(\{ ^stream.contents \});\
		function.value(stream);\
		^stream.contents\
	\}\
	\
\}\
}