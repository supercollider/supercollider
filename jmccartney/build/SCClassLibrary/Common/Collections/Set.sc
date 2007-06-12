{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Set\cf3  : \cf2 Collection\cf3  \{\
	\cf2 var\cf3  <>array, <size=0;\
	\
	*new \{ \cf2 arg\cf3  n=3; ^\cf2 super\cf3 .new.initSet(\cf2 this\cf3 .sizeFor(n)) \}\
	*sizeFor \{ \cf2 arg\cf3  n=3; ^if (n < 0, \{1\}, \{ n * 3 div: 2 \}) \}\
	species \{ ^\cf2 this\cf3 .class \}\
	copy \{ ^\cf2 this\cf3 .shallowCopy.array_( array.copy ) \}\
	makeEmpty \{ array.fill; \}\
	do \{ \cf2 arg\cf3  function;\
		\cf2 var\cf3  i=0;\
		if ( size > 0, \{\
			array.do(\{ \cf2 arg\cf3  item;\
				if (item.notNil, \{\
					function.value(item, i);\
					i = i + 1;\
				\})\
			\})\
		\})\
	\}\
	\
	includes \{ \cf2 arg\cf3  item; \
		^array.at(\cf2 this\cf3 .scanFor(item)).notNil;\
	\}\
	findMatch \{ \cf2 arg\cf3  item; \
		\cf4 // return an item that matches a given item\
\cf3 		^array.at(\cf2 this\cf3 .scanFor(item));\
	\}\
	add \{ \cf2 arg\cf3  item;\
		\cf2 var\cf3  index;\
		if (item.isNil, \{ \cf5 "A Set cannot contain nil.\\n"\cf3 .error; \cf2 this\cf3 .halt; \});\
		index = \cf2 this\cf3 .scanFor(item);\
		if ( array.at(index).isNil, \{ \cf2 this\cf3 .putCheck(index, item) \});\
	\}\
	remove \{ \cf2 arg\cf3  item;\
		\cf2 var\cf3  index;\
		index = \cf2 this\cf3 .scanFor(item);\
		if ( array.at(index).notNil, \{\
			array.put(index, \cf2 nil\cf3 );\
			size = size - 1;\
			\cf2 this\cf3 .fixCollisionsFrom(index);\
		\});\
	\}\
	choose \{\
		\cf2 var\cf3  index, val;\
		if (size <= 0, \{ ^\cf2 nil\cf3  \});\
		while(\{\
			index = array.size.rand;\
			(val = array.at(index)).isNil;\
		\});\
		^val\
	\}\
	pop \{\
		\cf2 var\cf3  index = 0, val;\
		while(\{\
			(val = array.at(index)).isNil;\
		\},\{ \
			index = index + 1 \
		\});\
		\cf2 this\cf3 .remove(val);\
		^val\
	\}\
	\
	sect \{ \cf2 arg\cf3  that;\
		\cf2 var\cf3  result;\
		result = \cf2 Set\cf3 .new;\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  item;\
			if (that.includes(item), \{\
				result.add(item);\
			\});\
		\});\
		^result\
	\}\
	union \{ \cf2 arg\cf3  that;\
		\cf2 var\cf3  result;\
		result = \cf2 Set\cf3 .new;\
		result.addAll(\cf2 this\cf3 );\
		result.addAll(that);\
		^result\
	\}\
	\
	\cf4 // PRIVATE IMPLEMENTATION\
\cf3 	initSet \{ \cf2 arg\cf3  n; array = \cf2 Array\cf3 .newClear(n); \}\
	putCheck \{ \cf2 arg\cf3  index, item;\
		array.put(index, item);\
		size = size + 1;\
		\cf2 this\cf3 .fullCheck;\
	\}\
	fullCheck \{\
		\cf4 //[\\fullCheck, array.size, size * 3 div: 2, size].postln;\
\cf3 		if (array.size < (size * 3 div: 2), \{ \cf2 this\cf3 .grow \});\
	\}\
	growSize \{ ^max( array.size, 2 ) \}\
	grow \{\
		\cf2 var\cf3  oldElements;\
		oldElements = array;\
		array = \cf2 Array\cf3 .newClear(array.size + \cf2 this\cf3 .growSize);\
		size = 0;\
		oldElements.do(\{ \cf2 arg\cf3  item;\
			if ( item.notNil, \{ \cf2 this\cf3 .noCheckAdd(item) \})\
		\});\
	\}\
	noCheckAdd \{ \cf2 arg\cf3  item;\
		array.put(\cf2 this\cf3 .scanFor(item), item);\
		size = size + 1;\
	\}\
	scanFor \{ \cf2 arg\cf3  obj;\
		\cf2 var\cf3  i, start, end, elem;\
		\
		start = obj.hash % array.size;\
		end = array.size;\
		i = start;\
		while (\{ i < end \}, \{\
			elem = array.at(i);\
			\
			if ( elem.isNil or: \{ elem == obj \}, \{ ^i \});\
			i = i + 1;\
		\});\
		end = start - 1;\
		i = 0;\
		while (\{ i <= end \}, \{\
			elem = array.at(i);\
			if ( elem.isNil or: \{ elem == obj \}, \{ ^i \});\
			i = i + 1;\
		\});\
		error(\cf5 "There is no free space in this set!\\n"\cf3 );\
		array.postln;\
		^-1\
	\}\
	\
	fixCollisionsFrom \{ \cf2 arg\cf3  index; \
		\cf2 var\cf3  lastKeyIndex, oldIndex, newIndex, element;\
\
		oldIndex = index;\
		lastKeyIndex = array.size - 1;\
		while (\{\
			if (oldIndex == lastKeyIndex, \{ oldIndex = 0 \}, \{ oldIndex = oldIndex + 1 \});\
			(element = \cf2 this\cf3 .keyAt(oldIndex)).notNil\
		\},\{\
			newIndex = \cf2 this\cf3 .scanFor(element);\
			if ( oldIndex != newIndex, \{ \cf2 this\cf3 .swap(oldIndex, newIndex) \})\
		\})\
	\}\
	swap \{ \cf2 arg\cf3  i, j; array.swap(i, j) \}\
	keyAt \{ \cf2 arg\cf3  index; ^array.at(index) \}\
	\
\}\
\
\cf2 IdentitySet\cf3  : \cf2 Set\cf3  \{\
	scanFor \{ \cf2 arg\cf3  argKey;\
		^array.atIdentityHash(argKey)\
	\cf4 /*\
		var i, start, end, elem;\
		\
		start = obj.identityHash % array.size;\
		end = array.size;\
		i = start;\
		while (\{ i < end \}, \{\
			elem = array.at(i);\
			if ( elem.isNil or: \{ elem === obj \}, \{ ^i \});\
			i = i + 1;\
		\});\
		end = start - 1;\
		i = 0;\
		while (\{ i < end \}, \{\
			elem = array.at(i);\
			if ( elem.isNil or: \{ elem === obj \}, \{ ^i \});\
			i = i + 1;\
		\});\
		^-1\
	*/\cf3 \
	\}\
\}}