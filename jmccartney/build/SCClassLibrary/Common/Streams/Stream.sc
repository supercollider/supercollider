{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red0\green0\blue0;
\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0  \
\cf2 Stream\cf0  : \cf2 AbstractFunction\cf0  \{	\
	\cf3 // 'reset' is defined in class Object to do nothing.\
\cf0 	\cf3 // reading\
\cf0 	next \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	value \{ ^\cf2 this\cf0 .next \}\
	valueArray \{ ^\cf2 this\cf0 .next \}\
	\
	nextN \{ \cf2 arg\cf0  n;\
		^\cf2 Array\cf0 .fill(n, \{ \cf2 this\cf0 .next; \});\
	\}\
	all \{\
		\cf2 arg\cf0  list;\
		list = \cf2 List\cf0 .new;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item; list.add(item); \});\
		^list\
	\}\
	\
	\cf3 // writing\
\cf0 	put \{ \cf2 arg\cf0  item;\
		^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \
	\}\
	putN \{ \cf2 arg\cf0  n, item; \
		n.do(\{ \cf2 this\cf0 .put(item); \});\
	\}\
	putAll \{ \cf2 arg\cf0  aCollection;\
		aCollection.do(\{ \cf2 arg\cf0  item; \cf2 this\cf0 .put(item); \});\
	\}\
	\
	do \{ \cf2 arg\cf0  function;\
		\cf2 var\cf0  item;\
		while (\{ \
			item = \cf2 this\cf0 .next; \
			item.notNil \
		\},\{\
			function.value(item);\
		\});\
	\}\
	\
	\cf3 // combination\
\cf0 	collect \{ \cf2 arg\cf0  argCollectFunc;\
		\cf3 // modify a stream\
\cf0 		\cf2 var\cf0  nextFunc, resetFunc;\
\
		nextFunc = \{ \cf2 arg\cf0  inval;\
			inval = \cf2 this\cf0 .next(inval);\
			if ( inval.notNil, \{\
				argCollectFunc.value(inval) \
			\},\{ \
				\cf2 nil\cf0  \
			\})\
		\};\
		resetFunc = \{ \cf2 this\cf0 .reset \};\
		^\cf2 FuncStream\cf0 .new(nextFunc, resetFunc);\
	\}\
	reject \{ \cf2 arg\cf0  function;\
		\cf3 // reject elements from a stream\
\cf0 		\cf2 var\cf0  nextFunc, resetFunc;\
		nextFunc = \{ \cf2 arg\cf0  inval;\
			inval = \cf2 this\cf0 .next(inval);\
			while (\{ \
				inval.notNil and: \{ function.value(inval) \}\
			\},\{\
				inval = \cf2 this\cf0 .next(inval);\
			\});\
			inval\
		\};\
		resetFunc = \{ \cf2 this\cf0 .reset \};\
		^\cf2 FuncStream\cf0 .new(nextFunc, resetFunc);\
	\}\
	select \{ \cf2 arg\cf0  function;\
		\cf3 // select elements from a stream\
\cf0 		\cf2 var\cf0  nextFunc, resetFunc;\
		nextFunc = \{ \cf2 arg\cf0  inval;\
			inval = \cf2 this\cf0 .next(inval);\
			while (\{ \
				inval.notNil and: \{ function.value(inval).not \}\
			\},\{\
				inval = \cf2 this\cf0 .next(inval);\
			\});\
			inval\
		\};\
		resetFunc = \{ \cf2 this\cf0 .reset \};\
		^\cf2 FuncStream\cf0 .new(nextFunc, resetFunc);\
	\}\
	\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\sa480\ql\qnatural
\cf4 	dot \{ \cf5 arg\cf4  function, stream;\
		\cf6 // combine item by item with another stream\
\cf4 		^\cf5 FuncStream\cf4 .new(\
			\{ \cf5 arg\cf4  inval;\
				\cf5 var\cf4  x, y;\
				x = \cf5 this\cf4 .next(inval);\
				y = stream.next(inval);\
				if ( x.notNil and: \{ y.notNil \}, \{\
					function.value(x, y)\
				\});\
			\},\
			\{ \cf5 this\cf4 .reset; stream.reset; \}\
		);\
	\}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	interlace \{ \cf2 arg\cf0  function, stream;\
		\cf3 // interlace with another stream\
\cf0 		\cf2 var\cf0  nextx, nexty;\
		\
		nextx = \cf2 this\cf0 .next;\
		nexty = stream.next;\
		^\cf2 FuncStream\cf0 .new(\{\
			\cf2 var\cf0  val;\
			if ( nextx.isNil, \{\
				if ( nexty.isNil, \{\cf2 nil\cf0 \},\{ val=nexty; nexty = stream.next; val \});\
			\},\{\
				if ( nexty.isNil or: \{ function.value(nextx,nexty) \}, \
					\{ val=nextx; nextx = \cf2 this\cf0 .next; val \},\
					\{ val=nexty; nexty = stream.next; val \}\
				);					\
			\});\
		\},\
		\{ \
			\cf2 this\cf0 .reset; stream.reset; \
			nextx = \cf2 this\cf0 .next;\
			nexty = stream.next;\
		\});\
	\}\
	++ \{ \cf2 arg\cf0  stream;\
		\cf3 // concatenate two streams\
\cf0 		^\cf2 this\cf0 .interlace(\cf2 true\cf0 , stream);\
	\}\
	collate \{ \cf2 arg\cf0  stream;\
		\cf3 // ascending order merge of two streams\
\cf0 		^\cf2 this\cf0 .interlace(\{ \cf2 arg\cf0  x, y; x < y \}, stream);\
	\}\
			\
\
	\cf3 // function composition\
\cf0 	composeUnaryOp \{ \cf2 arg\cf0  argSelector;\
		^\cf2 UnaryOpStream\cf0 .new(argSelector, \cf2 this\cf0 )\
	\}\
	composeBinaryOp \{ \cf2 arg\cf0  argSelector, argStream;\
		^\cf2 BinaryOpStream\cf0 .new(argSelector, \cf2 this\cf0 , argStream)\
	\}\
	reverseComposeBinaryOp \{ \cf2 arg\cf0  argSelector, argStream;\
		^\cf2 BinaryOpStream\cf0 .new(argSelector, argStream, \cf2 this\cf0 )\
	\}\
	composeNAryOp \{ \cf2 arg\cf0  argSelector, anArgList;\
		^\cf2 this\cf0 .notYetImplemented\
	\}\
	\
	spawn \{ \cf2 arg\cf0  spawn, eventCount, synth;\
		\cf3 // this method assumes that the stream returns something that responds to spawn\
\cf0 		\cf2 this\cf0 .next.spawn(spawn, eventCount, synth);	\
	\}\
\
	embedInStream \{ \cf2 arg\cf0  inval;\
		\cf2 var\cf0  outval;\
		while (\{\
			outval = \cf2 this\cf0 .value(inval);\
			outval.notNil\
		\},\{\
			inval = outval.yield;\
		\});\
		^inval\
	\}\
	\
	play \{ arg clock;\
		clock = clock ? \cf2 SystemClock;\cf0 \
		clock.play(this);\
	\}\
\}\
\
\cf2 FuncStream\cf0  : \cf2 Stream\cf0  \{\
	\cf2 var\cf0  <>nextFunc; \cf3 // Func is evaluated for each next state\
\cf0 	\cf2 var\cf0  <>resetFunc; \cf3 // Func is evaluated for each next state\
\cf0 	*new \{ \cf2 arg\cf0  nextFunc, resetFunc;	\
		^\cf2 super\cf0 .new.nextFunc_(nextFunc).resetFunc_(resetFunc)\
	\}\
	next \{ \cf2 arg\cf0  inval;\
		^nextFunc.value(inval) \
	\}\
	reset \{ \
		^resetFunc.value\
	\}\
\}\
\
}