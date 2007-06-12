{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red191\green0\blue0;
\red96\green96\blue96;\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Array\cf3 [slot] : \cf2 ArrayedCollection\cf3  \{\
	\
	*with \{ \cf2 arg\cf3  ... args; \
		\cf4 // return an array of the arguments given\
\cf3 		\cf4 // cool! the interpreter does it for me..\
\cf3 		^args \
	\}\
	reverse \{ \
		\cf2 _ArrayReverse\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	scramble \{ \
		\cf2 _ArrayScramble\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	mirror \{ \
		\cf2 _ArrayMirror\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	mirror1 \{ \
		\cf2 _ArrayMirror1\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	mirror2 \{ \
		\cf2 _ArrayMirror2\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	stutter \{ \cf2 arg\cf3  n=2;\
		\cf2 _ArrayStutter\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	rotate \{ \cf2 arg\cf3  n=1;\
		\cf2 _ArrayRotate\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	pyramid \{ \cf2 arg\cf3  patternType=1; \cf4 // an integer from 1-10\
\cf3 		\cf2 _ArrayPyramid\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	lace \{ \cf2 arg\cf3  length;\
		\cf2 _ArrayLace\cf3 \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	permute \{ \cf2 arg\cf3  nthPermutation;\
		\cf2 _ArrayPermute\cf3 \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	wrapExtend \{ \cf2 arg\cf3  length;\
		\cf2 _ArrayExtendWrap\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	foldExtend \{ \cf2 arg\cf3  length;\
		\cf2 _ArrayExtendFold\cf3 \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	slide \{ \cf2 arg\cf3  windowLength=3, stepSize=1;\
		\cf2 _ArraySlide\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	containsSeqColl \{\
		\cf2 _ArrayContainsSeqColl\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	\
	\cf4 // UGen support:\
\cf3 	multiChannelExpand \{\
		\cf2 _ArrayMultiChannelExpand\cf3  \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	source \{\
		\cf2 var\cf3  elem;\
		\cf4 // returns the source UGen from an Array of OutputProxy(s)\
\cf3 		elem = \cf2 this\cf3 .at(0);\
		if (elem.isKindOf(\cf2 OutputProxy\cf3 ), \{\
			^elem.source\
		\},\{\
			error(\cf5 "source: Not an Array of OutputProxy(s)\\n"\cf3 );\
			\cf2 this\cf3 .halt;\
		\});\
	\}\
	isValidUGenInput \{ ^\cf2 true\cf3  \}\
	\
\cf4 //	// 2D array support\
//	*newClear2D \{ arg rows=1, cols=1;\
//		^super.fill(rows, \{ Array.newClear(cols) \});\
//	\}\
//	*new2D \{ arg rows=1, cols=1; \
//		^this.newClear2D(rows, cols);\
//	\}\
//	at2D \{ arg row, col; ^this.at(row).at(col) \}\
//	put2D \{ arg row, col, val; ^this.at(row).put(col, val) \}\
//	fill2D \{ arg val; \
//		this.do(\{ arg row; \
//			row.size.do(\{ arg i; \
//				row.put(i, val) \
//			\}) \
//		\}) \
//	\}\
\cf3 	\
	\
	\cf4 // IdentitySet support\
\cf3 	atIdentityHash \{ \cf2 arg\cf3  argKey;\
		\cf2 _Array_AtIdentityHash\cf3 \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
	\cf4 // IdentityDictionary support\
\cf3 	atIdentityHashInPairs \{ \cf2 arg\cf3  argKey;\
		\cf2 _Array_AtIdentityHashInPairs\cf3 \
		^\cf2 this\cf3 .primitiveFailed \
	\}\
\
	asSpec \{ ^\cf2 ControlSpec\cf3 .performList(\cf6 \\new\cf3 , \cf2 this\cf3 ) \}\
	\
	printOn \{ \cf2 arg\cf3  stream;\
		if (stream.atLimit, \{ ^\cf2 this\cf3  \});\
		stream << \cf5 "[ "\cf3  ;\
		\cf2 this\cf3 .printItemsOn(stream);\
		stream << \cf5 " ]"\cf3  ;\
	\}\
	storeOn \{ \cf2 arg\cf3  stream;\
		if (stream.atLimit, \{ ^\cf2 this\cf3  \});\
		stream << \cf5 "[ "\cf3  ;\
		\cf2 this\cf3 .storeItemsOn(stream);\
		stream << \cf5 " ]"\cf3  ;\
	\}\
\
	\cf4 // threads\
\cf3 	fork \{ \cf2 arg\cf3  join;\
		\cf2 var\cf3  count = 0, cond;\
		join = join ? \cf2 this\cf3 .size;\
		cond = \cf2 Condition\cf3 (\{ count >= join \});\
		\cf2 this\cf3 .do(\{ \cf2 arg\cf3  func; \
			\cf2 Routine\cf3 (\{\
				func.value;\
				count = count + 1;\
				cond.signal;\
			\}).play;\
		\});\
		cond.wait;\
	\}\
	\
	\cf4 // UGen support\
\cf3 	madd \{ \cf2 arg\cf3  mul = 1.0, add = 0.0;\
		^\cf2 MulAdd(this, mul, add);\cf3 \
	\}	\
\}\
}