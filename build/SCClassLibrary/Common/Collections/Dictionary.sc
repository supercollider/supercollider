{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Dictionary\cf0  : \cf2 Set\cf0  \{\
		\
	*new \{ \cf2 arg\cf0  n=3; ^\cf2 super\cf0 .new(n*2) \}\
	\
	\cf3 // accessing\
\cf0 	at \{ \cf2 arg\cf0  key;\
		^array.at(\cf2 this\cf0 .scanFor(key) + 1)\
	\}\
	atFail \{ \cf2 arg\cf0  key, function;\
		\cf2 var\cf0  val;\
		val = array.at(\cf2 this\cf0 .scanFor(key) + 1);\
		if ( val.isNil, \{ ^function.value \}, \{ ^val \});\
	\}\
	add \{ \cf2 arg\cf0  anAssociation;\
		\cf2 this\cf0 .put(anAssociation.key, anAssociation.value);\
	\}\
	put \{ \cf2 arg\cf0  key, obj;\
		\cf2 var\cf0  index, atKey;\
		index = \cf2 this\cf0 .scanFor(key);\
		array.put(index+1, obj);\
		if ( array.at(index).isNil, \{\
			array.put(index, key);\
			size = size + 1;\
			if (array.size < (size * 3), \{ \cf2 this\cf0 .grow \});\
		\});\
	\}\
	putAll \{ \cf2 arg\cf0  aDictionary; \
		aDictionary.keysValuesDo(\{ \cf2 arg\cf0  key, value; \
			\cf2 this\cf0 .put(key, value) \
		\}) \
	\}\
	associationAt \{ \cf2 arg\cf0  key;\
		\cf2 var\cf0  index, anAssociation;\
		index = \cf2 this\cf0 .scanFor(key);\
		if (index >= 0, \{\
			^\cf2 Association\cf0 .new(array.at(index), array.at(index+1));\
		\},\{\
			^\cf2 nil\cf0 \
		\});\
	\}\
	associationAtFail \{ \cf2 arg\cf0  argKey, function;\
		\cf2 var\cf0  index, key;\
		index = \cf2 this\cf0 .scanFor(argKey);\
		key = array.at(index);\
		if ( key.isNil, \{ ^function.value \}, \{ \
			^\cf2 Association\cf0 .new(key, array.at(index+1)) \});\
	\}\
	keys \{\
		\cf2 var\cf0  set;\
		set = \cf2 Set\cf0 .new(size);\
		\cf2 this\cf0 .keysDo(\{ \cf2 arg\cf0  key; set.add(key) \});\
		^set\
	\}\
	values \{\
		\cf2 var\cf0  list;\
		list = \cf2 List\cf0 .new(size);\
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  value; list.add(value) \});\
		^list\
	\}\
	\
	\cf3 // testing\
\cf0 	includes \{ \cf2 arg\cf0  item1; \
		\cf2 this\cf0 .do(\{ \cf2 arg\cf0  item2; if (item1 == item2, \{^\cf2 true\cf0 \}) \});\
		^\cf2 false\cf0 \
	\}\
	includesKey \{ \cf2 arg\cf0  item1; \
		\cf2 this\cf0 .keysDo(\{ \cf2 arg\cf0  item2; if (item1 == item2, \{^\cf2 true\cf0 \}) \});\
		^\cf2 false\cf0 \
	\}\
	\
	\cf3 // removing\
\cf0 	removeAt \{ \cf2 arg\cf0  key;\
		\cf2 var\cf0  index, atKeyIndex, val;\
		index = \cf2 this\cf0 .scanFor(key);\
		atKeyIndex = array.at(index);\
		if ( atKeyIndex.isNil, \{ ^\cf2 nil\cf0  \});\
		val = array.at(index+1);\
		array.put(index, \cf2 nil\cf0 );\
		array.put(index+1, \cf2 nil\cf0 );\
		size = size - 1;\
		\cf2 this\cf0 .fixCollisionsFrom(index);\
		^val\
	\}	\
	removeAtFail \{ \cf2 arg\cf0  key, function;\
		\cf2 var\cf0  index, atKeyIndex, val;\
		index = \cf2 this\cf0 .scanFor(key);\
		atKeyIndex = array.at(index);\
		if ( atKeyIndex.isNil, \{ ^function.value \});\
		val = array.at(index+1);\
		array.put(index, \cf2 nil\cf0 );\
		array.put(index+1, \cf2 nil\cf0 );\
		size = size - 1;\
		\cf2 this\cf0 .fixCollisionsFrom(index);\
		^val\
	\}	\
		\
	remove \{ ^\cf2 this\cf0 .shouldNotImplement \}\
	removeFail \{ ^\cf2 this\cf0 .shouldNotImplement \}\
	\
	\cf3 // enumerating\
\cf0 	keysValuesDo \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .keysValuesArrayDo(array, function);\
	\}\
	keysValuesChange \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .keysValuesDo(\{ \cf2 arg\cf0  key, value, i;\
			\cf2 this\cf0 .put(key, function.value(key, value, i));\
		\})\
	\}\
	do \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .keysValuesDo(\{ \cf2 arg\cf0  key, value, i;\
			function.value(value, i);\
		\})\
	\}\
	keysDo \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .keysValuesDo(\{ \cf2 arg\cf0  key, val, i;\
			function.value(key, i);\
		\})\
	\}\
	associationsDo \{ \cf2 arg\cf0  function;\
		\cf2 this\cf0 .keysValuesDo(\{ \cf2 arg\cf0  key, val, i;\
			function.value( \cf2 Association\cf0 .new(key, val), i);\
		\})\
	\}\
\
	findKeyForValue \{ \cf2 arg\cf0  argValue;\
		\cf2 this\cf0 .keysValuesArrayDo(array, \{ \cf2 arg\cf0  key, val, i;\
			if (argValue == val, \{ ^key \})\
		\});\
		^\cf2 nil\cf0 \
	\}\
	\
    choose \{\
        \cf2 var\cf0  index, key, val;\
        if (\cf2 this\cf0 .size <= 0, \{ ^\cf2 nil\cf0  \}); \cf3 // empty dictionary\
\cf0         while(\{\
            index = (array.size >> 1).rand << 1; \cf3 // generate an even index.\
\cf0             array.at(index).isNil;              \cf3 // key is at even index.\
\cf0         \});\
        \cf3 // return the value for the first non Nil key we find.\
\cf0         \cf3 // the value is at the odd index.\
\cf0         ^array.at(index + 1); \
    \}\
	\
	\cf3 // Pattern support\
\cf0 	transformEvent \{ \cf2 arg\cf0  event;\
		^event.putAll(\cf2 this\cf0 );\
	\}\
	embedInStream \{ \cf2 arg\cf0  event;\
		^event.putAll(\cf2 this\cf0 ).yield;\
	\}\
	\
	\cf3 // PRIVATE IMPLEMENTATION\
\cf0 	keysValuesArrayDo \{ \cf2 arg\cf0  argArray, function;\
		\cf3 // special byte codes inserted by compiler for this method\
\cf0 		\cf2 var\cf0  i=0, j=0, arraySize, key, val;\
		arraySize = argArray.size;			\
		while (\{ i < arraySize \},\{\
			key = argArray.at(i);\
			if (key.notNil, \{\
				val = argArray.at(i+1);\
				function.value(key, val, j);\
				j = j + 1;\
			\});\
			i = i + 2;\
		\});\
	\}\
	*sizeFor \{ \cf2 arg\cf0  n=3; ^if (n < 0, \{1\}, \{ n * 3 \}) \}\
	grow \{\
		\cf2 var\cf0  oldElements, index;\
		oldElements = array;\
		array = \cf2 Array\cf0 .newClear(array.size + \cf2 this\cf0 .growSize);\
		\cf2 this\cf0 .keysValuesArrayDo(oldElements, \
		\{ \cf2 arg\cf0  key, val;\
			index = \cf2 this\cf0 .scanFor(key);\
			array.put(index, key);\
			array.put(index+1, val);\
		\});\
	\}\
	fixCollisionsFrom \{ \cf2 arg\cf0  index; \
		\cf2 var\cf0  lastKeyIndex, oldIndex, newIndex, key;\
\
		oldIndex = index;\
		lastKeyIndex = array.size - 2;\
		while (\{\
			if (oldIndex == lastKeyIndex, \{ oldIndex = 0 \}, \{ oldIndex = oldIndex + 2 \});\
			(key = array.at(oldIndex)).notNil\
		\},\{\
			newIndex = \cf2 this\cf0 .scanFor(key);\
			if ( oldIndex != newIndex, \{ \
				\cf2 this\cf0 .swap(oldIndex, newIndex);\
				\cf2 this\cf0 .swap(oldIndex+1, newIndex+1) \
			\})\
		\})\
	\}\
	scanFor \{ \cf2 arg\cf0  argKey;\
		\cf2 var\cf0  i, start, end, key, maxHash;\
		\
		maxHash = array.size div: 2;\
		start = (argKey.hash % maxHash) * 2;\
		end = array.size-1;\
		i = start;\
		forBy( start, end, 2, \{ \cf2 arg\cf0  i;\
			key = array.at(i);\
			if ( key.isNil or: \{ key == argKey \}, \{ ^i \});\
		\});\
		end = start - 1;\
		forBy( 0, start-2, 2, \{ \cf2 arg\cf0  i;\
			key = array.at(i);\
			if ( key.isNil or: \{ key == argKey \}, \{ ^i \});\
		\});\
		^-2\
	\}\
	\
	storeItemsOn \{ \cf2 arg\cf0  stream, itemsPerLine = 5;\
		\cf2 var\cf0  last, itemsPerLinem1;\
		itemsPerLinem1 = itemsPerLine - 1;\
		last = \cf2 this\cf0 .size - 1;\
		\cf2 this\cf0 .associationsDo(\{ \cf2 arg\cf0  item, i;\
			item.storeOn(stream);\
			if (i < last, \{ stream.comma.space;\
				if (i % itemsPerLine == itemsPerLinem1, \{ stream.nl.space.space \});\
			\});\
		\});\
	\}\
	printItemsOn \{ \cf2 arg\cf0  stream, itemsPerLine = 5;\
		\cf2 var\cf0  last, itemsPerLinem1;\
		itemsPerLinem1 = itemsPerLine - 1;\
		last = \cf2 this\cf0 .size - 1;\
		\cf2 this\cf0 .associationsDo(\{ \cf2 arg\cf0  item, i;\
			item.printOn(stream);\
			if (i < last, \{ stream.comma.space;\
				if (i % itemsPerLine == itemsPerLinem1, \{ stream.nl.space.space \});\
			\});\
		\});\
	\}\
\}\
\
\cf2 IdentityDictionary\cf0  : \cf2 Dictionary\cf0  \{\
	at \{ \cf2 arg\cf0  key, value;\
		\cf2 _IdentDict_At\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 /*^array.at(this.scanFor(key) + 1)*/\cf0 \
	\}\
	put \{ \cf2 arg\cf0  key, value;\
		\cf2 _IdentDict_Put\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 /*\
		var index, atKey;\
		index = this.scanFor(key);\
		array.put(index+1, obj);\
		if ( array.at(index).isNil, \{\
			array.put(index, key);\
			size = size + 1;\
			if (array.size < (size * 3), \{ this.grow \});\
		\});\
		*/\cf0 \
	\}\
	putGet \{ \cf2 arg\cf0  key, value;\
		\cf2 _IdentDict_PutGet\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 /*\
		var index, atKey, prev;\
		index = this.scanFor(key);\
		prev = array.at(index);\
		array.put(index+1, obj);\
		if ( array.at(index).isNil, \{\
			array.put(index, key);\
			size = size + 1;\
			if (array.size < (size * 3), \{ this.grow \});\
		\});\
		^prev\
		*/\cf0 \
	\}\
	findKeyForValue \{ \cf2 arg\cf0  argValue;\
		\cf2 this\cf0 .keysValuesArrayDo(array, \{ \cf2 arg\cf0  key, val, i;\
			if (argValue === val, \{ ^key \})\
		\});\
		^\cf2 nil\cf0 \
	\}\
	scanFor \{ \cf2 arg\cf0  argKey;\
		^array.atIdentityHashInPairs(argKey)\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf3 /*\
		var i, start, end, key, maxHash;\
		\
		maxHash = array.size div: 2;\
		start = (argKey.identityHash % maxHash) * 2;\
		end = array.size;\
		i = start;\
		while (\{ i < end \}, \{\
			key = array.at(i);\
			if ( key.isNil or: \{ key === argKey \}, \{ ^i \});\
			i = i + 2;\
		\});\
		end = start - 1;\
		i = 0;\
		while (\{ i < end \}, \{\
			key = array.at(i);\
			if ( key.isNil or: \{ key === argKey \}, \{ ^i \});\
			i = i + 2;\
		\});\
		^-2\
*/\cf0 \
	\}\
	\
\}\
\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 NameDictionary\cf0  : \cf2 IdentityDictionary\cf0 \
\{\
	add \{ \cf2 arg\cf0  aNamedObject;\
		\cf2 this\cf0 .put(aNamedObject.name, aNamedObject);\
	\}\
\}\
\
}