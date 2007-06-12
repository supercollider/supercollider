{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;\red191\green0\blue0;
\red0\green115\blue0;\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 Pattern\cf2  : \cf3 AbstractFunction\cf2  \{\
	\
	\
	\cf4 // concatenate Patterns\
\cf2 	++ \{ \cf3 arg\cf2  aPattern;\
		^\cf3 Pseq\cf2 .new([\cf3 this\cf2 , aPattern])\
	\}	\
\
	play \{ \cf3 arg\cf2  protoEvent, clock;\
		\cf3 Pevent\cf2 (\cf3 this\cf2 , protoEvent).play(clock)\
	\}\
	\
	asStream \{\
		^\cf3 this\cf2 .subclassResponsibility(\cf3 thisMethod\cf2 )\
	\}\
	\
	embedInStream \{ \cf3 arg\cf2  inval;\
		^\cf3 this\cf2 .asStream.embedInStream(inval);\
	\}\
	\
	\cf4 // filtering operations\
\cf2 	collect \{ \cf3 arg\cf2  function;\
		^\cf3 Pcollect\cf2 .new(function, \cf3 this\cf2 )\
	\}\
	select \{ \cf3 arg\cf2  function;\
		^\cf3 Pselect\cf2 .new(function, \cf3 this\cf2 )\
	\}\
	reject \{ \cf3 arg\cf2  function;\
		^\cf3 Preject\cf2 .new(function, \cf3 this\cf2 )\
	\}\
	\
	\cf4 // function composition\
\cf2 	composeUnaryOp \{ \cf3 arg\cf2  operator;\
		^\cf3 Punop\cf2 .new(operator, \cf3 this\cf2 )\
	\}\
	composeBinaryOp \{ \cf3 arg\cf2  operator, pattern;\
		^\cf3 Pbinop\cf2 .new(operator, \cf3 this\cf2 , pattern)\
	\}\
	reverseComposeBinaryOp \{ \cf3 arg\cf2  operator, pattern;\
		^\cf3 Pbinop\cf2 .new(operator, pattern, \cf3 this\cf2 )\
	\}\
	composeNAryOp \{ \cf3 arg\cf2  selector, argList;\
		^\cf3 this\cf2 .notYetImplemented\
	\}\
\}\
\
\cf3 Pevent\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>pattern, <>event;\
	*new \{ \cf3 arg\cf2  pattern, event;\
		^\cf3 super\cf2 .newCopyArgs(pattern, event);\
	\}\
	asStream \{\
		^\cf3 EventStream\cf2 (pattern.asStream, event);\
	\}\
	play \{ \cf3 arg\cf2  clock;\
		\cf5 \\Pevent_play\cf2 .postln;\
		\cf3 this\cf2 .asStream.play(clock ? \cf3 SystemClock\cf2 )\
	\}\
\}\
\
\cf3 EventStream\cf2  : \cf3 Stream\cf2  \{\
	\cf3 var\cf2  <>stream, <>protoEvent;\
	*new \{ \cf3 arg\cf2  stream, protoEvent;\
		^\cf3 super\cf2 .newCopyArgs(stream, protoEvent);\
	\}\
	next \{\
		\cf3 var\cf2  event;\
		event = stream.next( protoEvent.copy );\
		if (event.isNil, \{ ^\cf3 nil\cf2  \});\
		\
		^event.play;\
	\}\
\}\
\
\
\cf3 Pfunc\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>nextFunc; \cf4 // Func is evaluated for each next state\
\cf2 	\cf3 var\cf2  <>resetFunc; \cf4 // Func is evaluated for each next state\
\cf2 	*new \{ \cf3 arg\cf2  nextFunc, resetFunc;	\
		^\cf3 super\cf2 .newCopyArgs(nextFunc, resetFunc)\
	\}\
	asStream \{\
		^\cf3 FuncStream\cf2 .new(nextFunc, resetFunc)\
	\}\
\}\
\
\cf3 Prout\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>routineFunc;\
	*new \{ \cf3 arg\cf2  routineFunc;\
		^\cf3 super\cf2 .newCopyArgs(routineFunc)\
	\}\
	asStream \{\
		^\cf3 Routine\cf2 .new(routineFunc)\
	\}\
\}\
\
\cf3 Pfuncn\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>func, <>repeats;\
	*new \{ \cf3 arg\cf2  func, repeats = 1;\
		^\cf3 super\cf2 .newCopyArgs(func, repeats)\
	\}\
	asStream \{ \
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inval;\
			repeats.value.do(\{\
				inval = func.value(inval).yield;\
			\});\
		\});\
	\}\
\}	\
\
\cf4 // Punop and Pbinop are used to create patterns in response to math operations\
\cf3 Punop\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>operator, <>a;\
	*new \{ \cf3 arg\cf2  operator, a;\
		^\cf3 super\cf2 .newCopyArgs(operator, a)\
	\}\
	asStream \{\
		\cf3 var\cf2  stream;\
		stream = a.asStream;\
		^\cf3 UnaryOpStream\cf2 .new(operator, stream);\
	\}\
\}\
\
\cf3 Pbinop\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>operator, <>a, <>b;\
	*new \{ \cf3 arg\cf2  operator, a, b;\
		^\cf3 super\cf2 .newCopyArgs(operator, a, b)\
	\}\
	asStream \{\
		\cf3 var\cf2  streamA, streamB;\
		streamA = a.asStream;\
		streamB = b.asStream;\
		^\cf3 BinaryOpStream\cf2 .new(operator, streamA, streamB);\
	\}\
\}\
\
\
\cf3 Pbind\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>patternpairs;\
	*new \{ \cf3 arg\cf2  ... pairs;\
		if (pairs.size.odd, \{ \cf6 "Pbind should have even number of args.\\n"\cf2 .error; \cf3 this\cf2 .halt \});\
		^\cf3 super\cf2 .new.patternpairs_(pairs)\
	\}\
	asStream \{\
		\cf3 var\cf2  streampairs, endval;\
		\
		streampairs = patternpairs.copy;\
		endval = streampairs.size - 1;\
		forBy (1, endval, 2, \{ \cf3 arg\cf2  i;\
			streampairs.put(i, streampairs.at(i).asStream);\
		\});\
		\
		^\cf3 FuncStream\cf2 .new(\{ \cf3 arg\cf2  inval;\
			\cf3 var\cf2  sawNil = \cf3 false\cf2 ;\
			inval = inval.copy;\
			forBy (0, endval, 2, \{ \cf3 arg\cf2  i;\
				\cf3 var\cf2  name, stream, streamout;\
				name = streampairs.at(i);\
				stream = streampairs.at(i+1);\
				\
				streamout = stream.next(inval);\
				\
				if (streamout.isNil, \{\
					sawNil = \cf3 true\cf2 ;\
				\},\{\
					if (name.isKindOf(\cf3 SequenceableCollection\cf2 ), \{					\
						streamout.do(\{ \cf3 arg\cf2  val, i;\
							inval.put(name.at(i), val);\
						\});\
					\},\{\
						inval.put(name, streamout);\
					\});\
				\});\
			\});\
			if (sawNil, \{ \cf3 nil\cf2  \},\{ \
				inval \
			\});\
		\},\{\
			streampairs = patternpairs.copy;\
			endval = streampairs.size - 1;\
			forBy (1, endval, 2, \{ \cf3 arg\cf2  i;\
				streampairs.put(i, streampairs.at(i).asStream);\
			\});\
		\});\
	\}\
\}\
\
\cf3 Pseries\cf2  : \cf3 Pattern\cf2  \{	\cf4 // arithmetic series\
\cf2 	\cf3 var\cf2  <>start=0, <>step=1, <>length=\cf3 inf\cf2 ;\
	*new \{ \cf3 arg\cf2  start = 0, step = 1, length=\cf3 inf\cf2 ;\
		^\cf3 super\cf2 .newCopyArgs(start, step, length)\
	\}	\
	asStream \{\
		\cf3 var\cf2  cur, counter = 0;\
		cur = start;\
		^\cf3 FuncStream\cf2 .new(\{\
			\cf3 var\cf2  outval;\
			if (counter >= length, \{\
				\cf3 nil\cf2 \
			\},\{\
				outval = cur;\
				cur = cur + step;\
				counter = counter + 1;\
				outval\
			\});\
		\},\{\
			cur = start;\
			counter = 0;\
		\});\
	\}\
\}\
		\
\cf3 Pgeom\cf2  : \cf3 Pattern\cf2  \{	\cf4 // geometric series\
\cf2 	\cf3 var\cf2  <>start=1.0, <>grow=1.0, <>length=\cf3 inf\cf2 ;\
	*new \{ \cf3 arg\cf2  start = 0, grow = 1, length=\cf3 inf\cf2 ;\
		^\cf3 super\cf2 .newCopyArgs(start, grow, length)\
	\}	\
	asStream \{\
		\cf3 var\cf2  cur, counter = 0;\
		cur = start;\
		^\cf3 FuncStream\cf2 .new(\{\
			\cf3 var\cf2  outval;\
			if (counter >= length, \{\
				\cf3 nil\cf2 \
			\},\{\
				outval = cur;\
				cur = cur * grow;\
				counter = counter + 1;\
				outval\
			\});\
		\},\{\
			cur = start;\
			counter = 0;\
		\});\
	\}\
\}\
\
\cf3 Pbrown\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>lo, <>hi, <>step, <>length;\
	*new \{ \cf3 arg\cf2  lo, hi, step, length=\cf3 inf\cf2 ;\
		^\cf3 super\cf2 .newCopyArgs(lo, hi, step, length)\
	\}	\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{\
			\cf3 var\cf2  cur;\
			cur = lo rrand: hi;\
			length.do(\{\
				cur = (cur + step.xrand2).fold(lo,hi);\
				cur.yield;				\
			\});\
		\});\
	\}\
\}	\
\
\cf3 Pwhite\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>lo, <>hi, <>length;\
	*new \{ \cf3 arg\cf2  lo, hi, length=\cf3 inf\cf2 ;\
		^\cf3 super\cf2 .newCopyArgs(lo, hi, length)\
	\}	\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{\
			length.do(\{\
				rrand(lo,hi).yield;				\
			\});\
		\});\
	\}\
\}	\
				\
\cf3 Pstep2add\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>pattern1, <>pattern2;\
	*new \{ \cf3 arg\cf2  pattern1, pattern2;\
		^\cf3 super\cf2 .newCopyArgs(pattern1, pattern2)\
	\}	\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inval;\
			\cf3 var\cf2  stream1, stream2, val1, val2;\
			\
			stream1 = pattern1.asStream;\
			while (\{\
				(val1 = stream1.next(inval)).notNil;\
			\},\{\
				stream2 = pattern2.asStream;\
				while (\{\
					(val2 = stream2.next(inval)).notNil;\
				\},\{\
					inval = yield( val1 + val2 );\
				\});\
			\});\
		\})		\
	\}\
\}\
\
\cf3 Pstep3add\cf2  : \cf3 Pattern\cf2  \{\
	\cf3 var\cf2  <>pattern1, <>pattern2, <>pattern3;\
	*new \{ \cf3 arg\cf2  pattern1, pattern2, pattern3;\
		^\cf3 super\cf2 .newCopyArgs(pattern1, pattern2, pattern3)\
	\}	\
	asStream \{\
		^\cf3 Routine\cf2 .new(\{ \cf3 arg\cf2  inval;\
			\cf3 var\cf2  stream1, stream2, stream3, val1, val2, val3;\
			\
			stream1 = pattern1.asStream;\
			while (\{\
				(val1 = stream1.next(inval)).notNil;\
			\},\{\
				stream2 = pattern2.asStream;\
				while (\{\
					(val2 = stream2.next(inval)).notNil;\
				\},\{\
					stream3 = pattern3.asStream;\
					while (\{\
						(val3 = stream3.next(inval)).notNil;\
					\},\{\
						inval = yield( val1 + val2 + val3 );\
					\});\
				\});\
			\});\
		\})		\
	\}\
\}\
\
\
}