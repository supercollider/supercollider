{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red96\green96\blue96;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Collection\cf0  \{ \
	*newFrom \{ \cf2 arg\cf0  aCollection;\
		\cf2 var\cf0  newCollection;\
		newCollection = \cf2 this\cf0 .new(aCollection.size);\
		aCollection.do(\{ \cf2 arg\cf0  item, i; newCollection.add(item) \});\
		^newCollection\
	\}\
	*with \{ \cf2 arg\cf0  ... args; \
		\cf2 var\cf0  newColl;\
		\cf3 // answer an array of my class of the given arguments\
\cf0 		\cf3 // the class Array has a simpler implementation\
\cf0 		newColl = \cf2 this\cf0 .new(args.size);\
		newColl.addAll(args);\
		^newColl\
	\}\
	*fill \{ \cf2 arg\cf0  size, function;\
		\cf2 var\cf0  obj;\
		obj = \cf2 this\cf0 .new(size);\
		size.do(\{ \cf2 arg\cf0  i;\
			obj.add(function.value(i));\
		\});\
		^obj\
	\}	\
\
	@ \{ \cf2 arg\cf0  index; ^\cf2 this\cf0 .at(index) \}\
	\
	== \{ \cf2 arg\cf0  aCollection;\
		if (aCollection.class != \cf2 this\cf0 .class, \{ ^\cf2 false\cf0  \});\
		if (\cf2 this\cf0 .size != aCollection.size, \{ ^\cf2 false\cf0  \});\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item, i;\
			if (item != aCollection.at(i), \{ ^\cf2 false\cf0  \});\
		\});\
		^\cf2 true\cf0 \
	\}\
	hash \{ \
		\cf2 var\cf0  hash;\
		hash = \cf2 this\cf0 .class.hash;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item;\
			hash = hash bitXor: item.hash;\
		\});\
	\}\
	\
	species \{ ^\cf2 Array\cf0  \}\
	do \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	size \{ \
		\cf3 // this is the slow way. Most collections have a faster way.\
\cf0 		\cf2 var\cf0  tally = 0;\
		\cf2 this\cf0 .do(\{ tally = tally + 1 \});\
		^tally\
	\}\
	\
	isEmpty \{ ^\cf2 this\cf0 .size == 0 \}\
	notEmpty \{ ^\cf2 this\cf0 .size > 0 \}\
	\
	add \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	addAll \{ \cf2 arg\cf0  aCollection; aCollection.do(\{ \cf2 arg\cf0  item; \cf2 this\cf0 .add(item) \}) \}\
	remove \{ ^\cf2 this\cf0 .subclassResponsibility(\cf2 thisMethod\cf0 ) \}\
	removeAll \{ \cf2 arg\cf0  list; list.do(\{ \cf2 arg\cf0  item; \cf2 this\cf0 .remove(item) \}) \}\
	removeAllSuchThat \{ \cf2 arg\cf0  function; \
		\cf2 var\cf0  removedItems, copy;\
		removedItems = \cf2 this\cf0 .species.new;\
		copy = \cf2 this\cf0 .copy;\
		copy.do(\{ \cf2 arg\cf0  item, i;\
			if ( function.value(item, i), \{\
				\cf2 this\cf0 .remove(item);\
				removedItems.add(item);\
			\})\
		\});\
		^removedItems\
	\}\
		\
	includes \{ \cf2 arg\cf0  item1; \
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item2; if (item1 === item2, \{^\cf2 true\cf0 \}) \});\
		^\cf2 false\cf0 \
	\}\
	includesAny \{ \cf2 arg\cf0  aCollection;\
		aCollection.do(\{ \cf2 arg\cf0  item; if (\cf2 this\cf0 .includes(item), \{^\cf2 true\cf0 \}) \});\
		^\cf2 false\cf0 \
	\}\
	includesAll \{ \cf2 arg\cf0  aCollection;\
		aCollection.do(\{ \cf2 arg\cf0  item; if (\cf2 this\cf0 .includes(item).not, \{^\cf2 false\cf0 \}) \});\
		^\cf2 true\cf0  \
	\}\
	collect \{ \cf2 arg\cf0  function;\
		^\cf2 this\cf0 .collectAs(function, \cf2 this\cf0 .species);\
	\}\
	select \{ \cf2 arg\cf0  function;\
		^\cf2 this\cf0 .selectAs(function, \cf2 this\cf0 .species);\
	\}\
	reject \{ \cf2 arg\cf0  function;\
		^\cf2 this\cf0 .rejectAs(function, \cf2 this\cf0 .species);\
	\}\
	collectAs \{ \cf2 arg\cf0  function, class;\
		\cf2 var\cf0  i, res;\
		res = class.new(\cf2 this\cf0 .size);\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; res.add(function.value(elem, i)) \})\
		^res;\
	\}\
	selectAs \{ \cf2 arg\cf0  function, class;\
		\cf2 var\cf0  i, res;\
		res = class.new(\cf2 this\cf0 .size);\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; if (function.value(elem, i), \{ res.add(elem) \}) \})\
		^res;\
	\}\
	rejectAs \{ \cf2 arg\cf0  function, class;\
		\cf2 var\cf0  i, res;\
		res = class.new(\cf2 this\cf0 .size);\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; if (function.value(elem, i).not, \{res.add(elem)\}) \})\
		^res;\
	\}\
	detect \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; if (function.value(elem, i), \{ ^elem \}) \})\
		^\cf2 nil\cf0 ;\
	\}\
	inject \{ \cf2 arg\cf0  thisValue, function;\
		\cf2 var\cf0  nextValue;\
		nextValue = thisValue;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item, i; \
			nextValue = function.value(nextValue, item, i);\
		\});\
		^nextValue\
	\}\
	count \{ \cf2 arg\cf0  function;\
		\cf2 var\cf0  sum = 0;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; if (function.value(elem, i), \{ sum=sum+1 \}) \})\
		^sum;\
	\}\
	occurencesOf \{ \cf2 arg\cf0  obj;\
		\cf2 var\cf0  sum = 0;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem; if (elem == obj, \{ sum=sum+1 \}) \})\
		^sum;\
	\}\
	any \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; if (function.value(elem, i), \{ ^\cf2 true\cf0  \}) \})\
		^\cf2 false\cf0 ;\
	\}\
	every \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; if (function.value(elem, i).not, \{ ^\cf2 false\cf0  \}) \})\
		^\cf2 true\cf0 ;\
	\}\
	sum \{ \cf2 arg\cf0  function;\
		\cf2 var\cf0  i, sum = 0;\
		if (function.isNil, \{ \cf3 // optimized version if no function\
\cf0 			\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem; sum = sum + elem; \})\
		\},\{\
			\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; sum = sum + function.value(elem, i); \})\
		\})\
		^sum;\
	\}\
	sumabs \{  \cf3 // sum of the absolute values - used to convert Mikael Laursen's rhythm lists.\
\cf0 		\cf2 var\cf0  sum = 0;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem; \
			if (elem.isKindOf(\cf2 SequenceableCollection\cf0 ), \{ elem = elem.at(0) \});\
			sum = sum + elem.abs; \
		\})\
		^sum;\
	\}\
	maxItem \{ \cf2 arg\cf0  function;\
		\cf2 var\cf0  maxValue, maxElement;\
		if (function.isNil, \{ \cf3 // optimized version if no function\
\cf0 			\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem; \
				if (maxElement.isNil, \{\
					maxElement = elem; \
				\},\{\
					if (elem > maxElement, \{\
						maxElement = elem; \
					\})\
				\})		\
			\})\
			^maxElement;\
		\},\{\
			\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; \cf2 var\cf0  val;\
				if (maxValue.isNil, \{\
					maxValue = function.value(elem, i);\
					maxElement = elem; \
				\},\{ \
					val = function.value(elem, i);\
					if (val > maxValue, \{\
						maxValue = val;\
						maxElement = elem; \
					\})\
				\})		\
			\})\
			^maxElement;\
		\})\
	\}\
	minItem \{ \cf2 arg\cf0  function;\
		\cf2 var\cf0  minValue, minElement;\
		if (function.isNil, \{ \cf3 // optimized version if no function\
\cf0 			\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; \
				if (minElement.isNil, \{\
					minElement = elem; \
				\},\{\
					if (elem < minElement, \{\
						minElement = elem; \
					\})\
				\})		\
			\});\
			^minElement;\
		\},\{\
			\cf2 this\cf0 .do(\{ \cf2 arg\cf0  elem, i; \cf2 var\cf0  val;\
				if (minValue.isNil, \{\
					minValue = function.value(elem, i);\
					minElement = elem; \
				\},\{ \
					val = function.value(elem, i);\
					if (val < minValue, \{\
						minValue = val;\
						minElement = elem; \
					\})\
				\})		\
			\})\
			^minElement;\
		\})\
	\}\
	asBag \{ ^\cf2 Bag\cf0 .new(\cf2 this\cf0 .size).addAll(\cf2 this\cf0 ); \}\
	asList \{ ^\cf2 List\cf0 .new(\cf2 this\cf0 .size).addAll(\cf2 this\cf0 ); \}\
	asSet \{ ^\cf2 Set\cf0 .new(\cf2 this\cf0 .size).addAll(\cf2 this\cf0 ); \}\
	asSortedList \{ ^\cf2 SortedList\cf0 .new(\cf2 this\cf0 .size).addAll(\cf2 this\cf0 ); \}\
	\
	printOn \{ \cf2 arg\cf0  stream;\
		if (stream.atLimit, \{ ^\cf2 this\cf0  \});\
		stream << \cf2 this\cf0 .class.name << \cf4 "[ "\cf0  ;\
		\cf2 this\cf0 .printItemsOn(stream);\
		stream << \cf4 " ]"\cf0  ;\
	\}\
	storeOn \{ \cf2 arg\cf0  stream;\
		if (stream.atLimit, \{ ^\cf2 this\cf0  \});\
		stream << \cf2 this\cf0 .class.name << \cf4 "[ "\cf0  ;\
		\cf2 this\cf0 .storeItemsOn(stream);\
		stream << \cf4 " ]"\cf0  ;\
	\}\
	storeItemsOn \{ \cf2 arg\cf0  stream;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item, i;\
			if (stream.atLimit, \{ ^\cf2 this\cf0  \});\
			if (i != 0, \{ stream.comma.space; \});\
			item.storeOn(stream);\
		\});\
	\}\
	printItemsOn \{ \cf2 arg\cf0  stream;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item, i;\
			if (stream.atLimit, \{ ^\cf2 this\cf0  \});\
			if (i != 0, \{ stream.comma.space; \});\
			item.printOn(stream);\
		\});\
	\}\
	\
	\cf3 // Synth support\
\cf0 	writeDefFile \{ \cf2 arg\cf0  name;\
		\cf2 var\cf0  file;\
		\
		if (name.isNil, \{ error(\cf4 "no file name"\cf0 ); ^\cf2 nil\cf0  \});\
		\
		file = \cf2 File\cf0 (\cf4 "server/synthdefs/"\cf0  ++ name ++ \cf4 ".scsyndef"\cf0 , \cf4 "w"\cf0 );\
		\
		file.putString(\cf4 "SCgf"\cf0 );\
		file.putInt32(0); \cf3 // file version\
\cf0 		file.putInt16(\cf2 this\cf0 .size); \cf3 // number of defs in file.\
\cf0 		\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item; item.writeDef(file); \});\
		\
		file.length.postln;\
		file.close;\
	\}\
\
	\cf3 // graphical support\
\cf0 	draw \{ \cf2 arg\cf0  args;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item; item.draw(args); \});\
	\}\
	layout \{ \cf2 arg\cf0  bounds;\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item; item.layout(bounds); \});\
	\}\
\}\
}