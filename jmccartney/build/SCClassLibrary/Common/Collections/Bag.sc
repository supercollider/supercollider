{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red0\green0\blue191;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Bag\cf0  : \cf2 Collection\cf0  \{\
	\cf2 var\cf0  contents;\
	\
	*new \{ \cf2 arg\cf0  n=4;\
		^super.new.setDictionary(n)\
	\}\
	\cf3 // testing\
\cf0 	includes \{ \cf2 arg\cf0  item;\
		^contents.includesKey(item)\
	\}\
	\cf3 // adding\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	add \{ \cf4 arg\cf0  item, count=1;\
		if ( \cf4 this\cf0 .includes(item), \{\
			contents.put(item, contents.at(item) + count);\
		\},\{\
			contents.put(item, count);\
		\});\
	\}\
	remove \{ \cf4 arg\cf0  item, count=1;\
		var newCount;\
		if ( \cf4 this\cf0 .includes(item), \{\
			newCount = contents.at(item) - count;\
			if (newCount <= 0, \{\
				contents.removeAt(item);\
			\},\{\
				contents.put(item, newCount);\
			\});\
		\});\
	\}\
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	\
	\cf3 // accessing\
\cf0 	at \{ ^\cf2 this\cf0 .shouldNotImplement \}\
	atFail \{ ^\cf2 this\cf0 .shouldNotImplement \}\
	put \{ ^\cf2 this\cf0 .shouldNotImplement \}\
\
	\cf3 // enumerating\
\cf0 	do \{ \cf2 arg\cf0  function;\
		\cf2 var\cf0  j = 0;\
		contents.associationsDo(\{ \cf2 arg\cf0  assn;\
			 assn.value.do(\{ \
			 	function.value(assn.key, j);\
			 	j = j + 1;\
			 \})\
		\});\
	\}\
	\
	\cf3 // PRIVATE IMPLEMENTATION\
\cf0 	setDictionary \{ \cf2 arg\cf0  n;\
		contents = \cf2 Dictionary\cf0 .new(n)\
	\}\
\}}