{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 ArrayedCollection\cf0  : \cf2 SequenceableCollection\cf0  \{\
\
	\cf3 // ArrayedCollections are vectors which have a \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	\cf3 // fixed maximum capacity. \
\cf0 \
	size \{ \cf2 _BasicSize\cf0  \}\
	maxSize \{ \cf2 _BasicMaxSize\cf0  \}\
		\
	swap \{ \cf2 arg\cf0  i, j; \cf2 var\cf0  temp;\
		\cf2 _BasicSwap\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
\
	at \{ \cf2 arg\cf0  index; \
		\cf2 _BasicAt\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	clipAt \{ \cf2 arg\cf0  index; \
		\cf2 _BasicClipAt\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	wrapAt \{ \cf2 arg\cf0  index; \
		\cf2 _BasicWrapAt\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	foldAt \{ \cf2 arg\cf0  index; \
		\cf2 _BasicFoldAt\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	put \{ \cf2 arg\cf0  index, item; \
		\cf2 _BasicPut\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	clipPut \{ \cf2 arg\cf0  index, item; \
		\cf2 _BasicClipPut\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	wrapPut \{ \cf2 arg\cf0  index, item; \
		\cf2 _BasicWrapPut\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	foldPut \{ \cf2 arg\cf0  index, item; \
		\cf2 _BasicFoldPut\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	removeAt \{ \cf2 arg\cf0  index; \
		\cf2 _BasicRemoveAt\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	takeAt \{ \cf2 arg\cf0  index; \
		\cf2 _BasicTakeAt\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
\
	\cf3 // see counterparts to these in Object \
\cf0 	slotSize \{\
		^\cf2 this\cf0 .size;\
	\}\
	slotAt \{ \cf2 arg\cf0  index;\
		^\cf2 this\cf0 .at(index);\
	\}\
	slotPut \{ \cf2 arg\cf0  index, value;\
		^\cf2 this\cf0 .put(index, value);\
	\}\
	slotKey \{ arg index;\
		^index\
	\}\
	\
	atModify \{ \cf2 arg\cf0  index, function; function.value(\cf2 this\cf0 .at(index)) \}\
	atInc \{ \cf2 arg\cf0  index, inc=1; \cf2 this\cf0 .put(index, \cf2 this\cf0 .at(index)+inc); \}\
	atDec \{ \cf2 arg\cf0  index, dec=1; \cf2 this\cf0 .put(index, \cf2 this\cf0 .at(index)-dec); \}\
\
	asArray \{ ^\cf2 this\cf0  \}\
	*with \{ \cf2 arg\cf0  ... args; \
		\cf2 var\cf0  newColl;\
		\cf3 // answer an array of my class of the given arguments\
\cf0 		\cf3 // the class Array has a simpler implementation\
\cf0 		newColl = \cf2 this\cf0 .new(args.size);\
		args.do(\{ \cf2 arg\cf0  item, i; newColl.add(i, item) \});\
		^newColl\
	\}\
	*series \{ \cf2 arg\cf0  size, start=0, step=1;\
		\cf2 var\cf0  array;\
		array = \cf2 this\cf0 .new(size);\
		size.do(\{ \cf2 arg\cf0  i;\
			array.add(start + (step * i));\
		\});\
		^array\
	\}\
	*geom \{ \cf2 arg\cf0  size, start, grow;\
		\cf2 var\cf0  array;\
		array = \cf2 this\cf0 .new(size);\
		size.do(\{ \cf2 arg\cf0  i;\
			array.add(start);\
			start = start * grow;\
		\});\
		^array\
	\}		\
\
	copyRange \{ \cf2 arg\cf0  start, end; \
		\cf3 // copies the fixed part of an object and the indexed slots\
\cf0 		\cf3 // from start to end.\
\cf0 		\cf2 _ObjectCopyRange\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed \
	\}\
\
	add \{ \cf2 arg\cf0  item; \
		\cf3 // add item to end of array.\
\cf0 		\cf3 // if the capacity is exceeded, this returns a new\
\cf0 		\cf3 // ArrayedCollection.\
\cf0 		\cf2 _ArrayAdd\cf0  \
		^\cf2 this\cf0 .primitiveFailed; \
	\}\
	addAll \{ \cf2 arg\cf0  aCollection; \
		\cf2 var\cf0  array;\
		\cf2 _ArrayAddAll\cf0 \
		array = \cf2 this\cf0 ;\
		aCollection.do(\{ \cf2 arg\cf0  item; array = array.add(item) \}) ;\
		^array\
	\}\
	extend \{ \cf2 arg\cf0  size, item; \
		\cf2 _ArrayExtend\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	insert \{ \cf2 arg\cf0  index, item; \
		\cf3 // add item at specified index.\
\cf0 		\cf3 // if the capacity is exceeded, this returns a new\
\cf0 		\cf3 // ArrayedCollection.\
\cf0 		\cf2 _ArrayInsert\cf0 \
		^\cf2 this\cf0 .primitiveFailed; \
	\}\
	addFirst \{ \cf2 arg\cf0  item; ^\cf2 this\cf0 .insert(0, item) \}\
	pop \{ \
		\cf3 // remove and return last item in array\
\cf0 		\cf2 _ArrayPop\cf0  \
		^\cf2 nil\cf0 ; \
	\}\
	++ \{ \cf2 arg\cf0  anArray; \
		\cf3 // concatenate two arrays of the same type\
\cf0 		\cf3 // this primitive will handle all array element types\
\cf0 		\cf2 _ArrayCat\cf0 ;\
		\cf3 // primitive fails if arrays are different types\
\cf0 		^\cf2 super\cf0  ++ anArray\
	\}\
	overWrite \{ \cf2 arg\cf0  aCollection, pos;\
		\cf2 var\cf0  array, grow;\
		\cf2 _ArrayOverwrite\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 //array = this.growClear(pos + aCollection.size - this.size);\
\cf0 		\cf3 //grow = pos + aCollection.size - this.size;\
\cf0 		\cf3 //if (grow > 0, \{ array = this ++ this.class.newClear(grow); \},\{ array = this \});\
\cf0 		\cf3 //aCollection.do(\{ arg item, i; array.put(pos + i, item); \});\
\cf0 		\cf3 //^array\
\cf0 	\}\
	grow \{ \cf2 arg\cf0  sizeIncrease;\
		\cf3 // returns an array of sufficient capacity.\
\cf0 		\cf3 // may return same object if it still has enough space or if sizeIncrease <= 0.\
\cf0 		\cf2 _ArrayGrow\cf0  \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	growClear \{ \cf2 arg\cf0  sizeIncrease;\
		\cf3 // returns an array of sufficient capacity.\
\cf0 		\cf3 // may return same object if it still has enough space or if sizeIncrease <= 0.\
\cf0 		\cf3 // clears new space\
\cf0 		\cf2 _ArrayGrowClear\cf0  \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	seriesFill \{ \cf2 arg\cf0  start, step;\
		\cf2 this\cf0 .size.do(\{ \cf2 arg\cf0  i;\
			\cf2 this\cf0 .put(i, start);\
			start = start + step;\
		\});\
	\}\
\
	fill \{ \cf2 arg\cf0  value; \
		\cf2 _ArrayFill\cf0  \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 /* replaced by primitive\
		var i = 0, size;\
		size = this.size;\
		while (\{ i < size \}, \{\
			this.put(i, val);\
			i = i + 1;\
		\});\
		*/\cf0 \
	\}\
	do \{ \cf2 arg\cf0  function;\
		\cf3 // special byte codes inserted by compiler for this method\
\cf0 		\cf2 var\cf0  size, i=0;\
		size = \cf2 this\cf0 .size;\
		while (\{ i < size \}, \{\
			function.value(\cf2 this\cf0 .at(i), i);\
			i = i + 1;\
		\})\
	\}\
	reverseDo \{ \cf2 arg\cf0  function;\
		\cf3 // special byte codes inserted by compiler for this method\
\cf0 		\cf2 var\cf0  i=0, j=0;\
		i = \cf2 this\cf0 .size - 1;\
		while (\{ i >= 0 \},\{ \
			function.value(\cf2 this\cf0 .at(i), j);\
			i = i - 1;\
			j = j + 1;\
		\})\
	\}\
	reverse \{ \
		\cf2 var\cf0  i = 0, size1, halfsize;\
		size1 = \cf2 this\cf0 .size - 1;\
		halfsize = \cf2 this\cf0 .size div: 2;\
		halfsize.do(\{ \cf2 arg\cf0  i;\
			\cf2 this\cf0 .swap(i, size1 - i);\
		\});\
	\}\
	windex \{\
		\cf2 _ArrayWIndex\cf0 \
		^\cf2 this\cf0 .primitiveFailed \
		\cf3 //		var r, sum = 0.0, index;\
\cf0 		\cf3 //		r = 1.0.rand;\
\cf0 		\cf3 //		this.detect(\{ arg weight, i;\
\cf0 		\cf3 //			sum = sum + weight;\
\cf0 		\cf3 //			if (sum >= r, \{\
\cf0 		\cf3 //				index = i;\
\cf0 		\cf3 //				true;\
\cf0 		\cf3 //			\},\{ false \});\
\cf0 		\cf3 //		\});\
\cf0 		\cf3 //		^index;\
\cf0 	\}\
	normalizeSum \{\
		\cf2 _ArrayNormalizeSum\cf0 \
		^\cf2 this\cf0 .primitiveFailed \
		\cf3 //	^(this / this.sum)\
\cf0 	\}\
\}\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 RawArray\cf0  : \cf2 ArrayedCollection\cf0  \{\
\}\
\
\cf2 Int8Array\cf0 [int8] : \cf2 RawArray\cf0  \{\
	unarchive \{\
		\cf2 _Unarchive\cf0 \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
\}\
\
\cf2 Int16Array\cf0 [int16] : \cf2 RawArray\cf0  \{\
\}\
\
\cf2 Int32Array\cf0 [int32] : \cf2 RawArray\cf0  \{\
\}\
\
\cf2 FloatArray\cf0 [float] : \cf2 RawArray\cf0  \{\
\}\
\
\cf2 DoubleArray\cf0 [double] : \cf2 RawArray\cf0  \{\
\}\
\
\cf2 SymbolArray\cf0 [symbol] : \cf2 RawArray\cf0  \{\
\}\
\
}