{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red96\green96\blue96;
\red0\green115\blue0;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 ListPattern\cf3  : \cf2 Pattern\cf3  \{\
	\cf2 var\cf3  <>list, <>repeats=1;\
	\
	*new \{ \cf2 arg\cf3  list, repeats=1;\
		^\cf2 super\cf3 .new.list_(list).repeats_(repeats)\
	\}\
	copy \{\
		^\cf2 super\cf3 .copy.list_(list.copy)\
	\}\
	storeParamsOn \{ \cf2 arg\cf3  stream;\
		stream << \cf4 "("\cf3  <<<* [ list, repeats ] << \cf4 ")"\cf3 ;\
	\}\
\}\
\
\cf2 Pseq\cf3  : \cf2 ListPattern\cf3  \{\
	\cf2 var\cf3  <>offset;\
	*new \{ \cf2 arg\cf3  list, repeats=1, offset=0;\
		^\cf2 super\cf3 .new(list, repeats).offset_(offset)\
	\}\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item, offsetValue;\
			offsetValue = offset.value;\
			if (inval.eventAt(\cf5 'reverse'\cf3 ) == \cf2 true\cf3 , \{\
				repeats.value.do(\{ \cf2 arg\cf3  j;\
					list.size.reverseDo(\{ \cf2 arg\cf3  i;\
						item = list @@ (i + offsetValue);\
						inval = item.embedInStream(inval);\
					\});\
				\});\
			\},\{\
				repeats.value.do(\{ \cf2 arg\cf3  j;\
					list.size.do(\{ \cf2 arg\cf3  i;\
						item = list @@ (i + offsetValue);\
						inval = item.embedInStream(inval);\
					\});\
				\});\
			\});\
		\});\
	\}\
	storeParamsOn \{ \cf2 arg\cf3  stream;\
		stream << \cf4 "("\cf3  <<<* [ list, repeats, offset ] << \cf4 ")"\cf3 ;\
	\}\
\}\
\
\cf2 Pser\cf3  : \cf2 Pseq\cf3  \{\
	asStream \{\
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item, offsetValue;\
			\
			offsetValue = offset.value;\
			if (inval.eventAt(\cf5 'reverse'\cf3 ) == \cf2 true\cf3 , \{\
				repeats.value.reverseDo(\{ \cf2 arg\cf3  i;\
					item = list @@ (i + offsetValue);\
					inval = item.embedInStream(inval);\
				\});\
			\},\{\
				repeats.value.do(\{ \cf2 arg\cf3  i;\
					item = list @@ (i + offsetValue);\
					inval = item.embedInStream(inval);\
				\});\
			\});\
		\});\
	\}\
\}	\
\
\cf2 Pshuf\cf3  : \cf2 ListPattern\cf3  \{\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  localList, item, stream;\
			\
			localList = list.copy.scramble;\
			repeats.value.do(\{ \cf2 arg\cf3  j;\
				localList.size.do(\{ \cf2 arg\cf3  i;\
					item = localList @@ i;\
					inval = item.embedInStream(inval);\
				\});\
			\});\
		\});\
	\}\
\}\
\
\cf2 Prand\cf3  : \cf2 ListPattern\cf3  \{\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item;\
			\
			repeats.value.do(\{ \cf2 arg\cf3  i;\
				item = list.at(list.size.rand);\
				inval = item.embedInStream(inval);\
			\});\
		\});\
	\}\
\}		\
\
\cf2 Pxrand\cf3  : \cf2 ListPattern\cf3  \{\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;			\
			\cf2 var\cf3  item, index, size;\
			index = list.size.rand;\
			repeats.value.do(\{ \cf2 arg\cf3  i;\
				size = list.size;\
				index = (index + (size - 1).rand + 1) % size;\
				item = list.at(index);\
				inval = item.embedInStream(inval);\
			\});\
		\});\
	\}\
\}		\
\
\cf2 Pwrand\cf3  : \cf2 ListPattern\cf3  \{\
	\cf2 var\cf3  <>weights;\
	*new \{ \cf2 arg\cf3  list, weights, repeats=1;\
		^\cf2 super\cf3 .new(list, repeats).weights_(weights)\
	\}\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item;\
			repeats.value.do(\{ \cf2 arg\cf3  i;\
				item = list.at(weights.windex);\
				inval = item.embedInStream(inval);\
			\});\
		\});\
	\}\
	storeParamsOn \{ \cf2 arg\cf3  stream;\
		stream << \cf4 "("\cf3  <<<* [ list, weights, repeats ] << \cf4 ")"\cf3 ;\
	\}\
\}		\
\
\
\cf2 Pfsm\cf3  : \cf2 ListPattern\cf3  \{\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item, index=0, maxState;\
			maxState = ((list.size - 1) div: 2) - 1;\
			repeats.value.do(\{\
				index = 0;\
				while(\{\
					index = list.at(index).choose.clip(0, maxState) * 2 + 2;\
					item = list.at(index - 1);\
					inval = item.embedInStream(inval);\
					inval.notNil;\
				\});\
			\});\
		\});\
	\}\
\}	\
\
	\
\cf6 //Ppar : ListPattern \{\
//	initStreams \{ arg priorityQ;\
//		list.do(\{ arg pattern, i; \
//			priorityQ.put(0.0, pattern.asStream);\
//		\});\
//	\}\
//	asStream \{\
//		^Routine(\{ arg inval;\
//			var count = 0, join, cond;\
//			join = list.size;\
//			cond = Condition(\{ count >= join \});\
//			list.do(\{ arg func; \
//				Routine(\{ arg time;\
//					inval.\
//					pattern.embedInStream(inval.copy);\
//					count = count + 1;\
//					cond.signal;\
//				\}).play;\
//			\});\
//			cond.wait;\
//		\});\
//	\}\
//\}\
\cf3 \
\cf2 Ppar\cf3  : \cf2 ListPattern\cf3  \{\
	initStreams \{ \cf2 arg\cf3  priorityQ;\
		list.do(\{ \cf2 arg\cf3  pattern, i; \
			priorityQ.put(0.0, pattern.asStream);\
		\});\
	\}\
	asStream \{\
		\cf2 var\cf3  priorityQ, assn;\
		\
		priorityQ = \cf2 PriorityQueue\cf3 .new;\
		\
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
		\
			repeats.value.do(\{ \cf2 arg\cf3  j;\
				\cf2 var\cf3  outval, stream, nexttime, now = 0.0;\
\
				\cf2 this\cf3 .initStreams(priorityQ);\
				\
				\cf6 // if first event not at time zero\
\cf3 				if (priorityQ.notEmpty and: \{ (nexttime = priorityQ.topPriority) > 0.0 \}, \{\
					outval = inval;\
					outval.put(\cf5 \\freq\cf3 , \cf5 \\rest\cf3 );					\
					outval.put(\cf5 \\delta\cf3 , nexttime);\
					\
					inval = outval.yield;\
					now = nexttime;	\
				\});\
				\
				while(\{\
					priorityQ.notEmpty\
				\},\{\
					stream = priorityQ.pop;\
					outval = stream.next(inval);\
					if (outval.isNil, \{\
						nexttime = priorityQ.topPriority;\
						if (nexttime.notNil, \{\
							\cf6 // that child stream ended, so rest until next one\
\cf3 							outval = inval;\
							outval.put(\cf5 \\freq\cf3 , \cf5 \\rest\cf3 );					\
							outval.put(\cf5 \\delta\cf3 , nexttime - now);\
							\
							inval = outval.yield;\
							now = nexttime;	\
						\},\{\
							priorityQ.clear;\
						\});		\
					\},\{			\
						\cf6 // requeue stream\
\cf3 						priorityQ.put(now + outval.delta, stream);\
						nexttime = priorityQ.topPriority;\
						outval.put(\cf5 \\delta\cf3 , nexttime - now);\
						\
						inval = outval.yield;\
						now = nexttime;	\
					\});	\
				\});\
			\});\
		\});\
	\}\
\}	\
\
\cf2 Ptpar\cf3  : \cf2 Ppar\cf3  \{\
	initStreams \{ \cf2 arg\cf3  priorityQ;\
		forBy(0, list.size-1, 2, \{ \cf2 arg\cf3  i;\
			priorityQ.put(list.at(i).value, list.at(i+1).asStream);\
		\});\
	\}\
\}\
\
\cf2 Pswitch\cf3  : \cf2 Pattern\cf3  \{\
	\cf2 var\cf3  <>list, <>which=0;\
	*new \{ \cf2 arg\cf3  list, which=0;\
		^\cf2 super\cf3 .new.list_(list).which_(which)\
	\}\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item, index, indexStream;\
			\
			indexStream = which.asStream;\
			while (\{\
				(index = indexStream.next).notNil;\
			\},\{\
				inval = list.wrapAt(index).embedInStream(inval);\
			\});\
		\});\
	\}\
	storeParamsOn \{ \cf2 arg\cf3  stream;\
		stream << \cf4 "("\cf3  <<<* [ list, which ] << \cf4 ")"\cf3 ;\
	\}\
\}\
\
\cf2 Pswitch1\cf3  : \cf2 Pswitch\cf3  \{	\
	asStream \{ \
		\cf2 var\cf3  streamList, indexStream;\
		\
		streamList = list.collect(\{ \cf2 arg\cf3  pattern; pattern.asStream; \});\
		indexStream = which.asStream;\
		\
		^\cf2 FuncStream\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  index;\
			if ((index = indexStream.next).notNil, \{\
				streamList.wrapAt(index).next(inval);\
			\});\
		\},\{\
			streamList.do(\{ \cf2 arg\cf3  stream; stream.reset; \});\
		\});\
	\}\
\}	\
\
\cf2 Ptuple\cf3  : \cf2 ListPattern\cf3  \{\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item, streams, tuple, outval;\
						\
			streams = list.collect(\{ \cf2 arg\cf3  item; item.asStream \});\
			\
			repeats.value.do(\{ \cf2 arg\cf3  j;\
				\cf2 var\cf3  sawNil = \cf2 false\cf3 ;\
								\
				while (\{\
					tuple = \cf2 Array\cf3 .new(streams.size);\
					streams.do(\{ \cf2 arg\cf3  stream; \
						outval = stream.next(inval);\
						if (outval.isNil, \{ sawNil = \cf2 true\cf3 ; \});\
						tuple.add(outval);\
					\});\
					sawNil.not\
				\},\{\
					inval = yield(tuple);\
				\});\
					\
			\});\
		\});\
	\}\
\}\
\
\cf2 Place\cf3  : \cf2 Pseq\cf3  \{\
	asStream \{ \
		^\cf2 Routine\cf3 .new(\{ \cf2 arg\cf3  inval;\
			\cf2 var\cf3  item, offsetValue;\
			\
			offsetValue = offset.value;\
			if (inval.eventAt(\cf5 'reverse'\cf3 ) == \cf2 true\cf3 , \{\
				repeats.value.do(\{ \cf2 arg\cf3  j;\
					list.size.reverseDo(\{ \cf2 arg\cf3  i;\
						item = list @@ (i + offsetValue);\
						if (item.isKindOf(\cf2 SequenceableCollection\cf3 ), \{\
							item = item @@ j;\
						\});\
						inval = item.embedInStream(inval);\
					\});\
				\});\
			\},\{\
				repeats.value.do(\{ \cf2 arg\cf3  j;\
					list.size.do(\{ \cf2 arg\cf3  i;\
						item = list @@ (i + offsetValue);\
						if (item.isKindOf(\cf2 SequenceableCollection\cf3 ), \{\
							item = item @@ j;\
						\});\
						inval = item.embedInStream(inval);\
					\});\
				\});\
			\});\
		\});\
	\}\
\}\
\
\cf2 Pslide\cf3  : \cf2 ListPattern\cf3  \{\
    \cf6 // 'repeats' is the number of segments.\
\cf3     \cf6 // 'len' is the length of each segment.\
\cf3     \cf6 // 'step' is how far to step the start of each segment from previous.\
\cf3     \cf6 // 'start' is what index to start at.\
\cf3     \cf6 // indexing wraps around if goes past beginning or end.\
\cf3     \cf6 // step can be negative.\
\cf3 \
    \cf2 var\cf3  <>len, <>step, <>start;\
    *new \{ \cf2 arg\cf3  list, repeats = 1, len = 3, step = 1, start = 0;\
        ^\cf2 super\cf3 .new(list, repeats).len_(len).step_(step).start_(start)\
    \}\
    asStream \{\
        ^\cf2 Routine\cf3 (\{ \cf2 arg\cf3  inval;\
            \cf2 var\cf3  pos, item;\
            pos = start;\
            repeats.do(\{\
                len.do(\{ \cf2 arg\cf3  j;\
                    item = list.wrapAt(pos + j);\
                    inval = item.embedInStream(inval);\
                \});\
                pos = pos + step;\
            \});\
        \});\
    \}\
\}\
\
}