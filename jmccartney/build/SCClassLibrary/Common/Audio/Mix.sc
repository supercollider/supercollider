{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf2 Mix\cf0  \{\
	*new \{ \cf2 arg\cf0  array;\
		\cf2 var\cf0  sum;\
		array.asArray.do(\{ \cf2 arg\cf0  val, i;\
			if (i == 0, \{ sum = val \},\{ sum = sum + val \});\
		\});\
		^sum\
	\}\
	\cf3 // support this common idiom \
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural
\cf0 	*fill \{ \cf2 arg\cf0  n, function;\
		\cf2 var\cf0  val, sum;\
		n.do(\{ \cf2 arg\cf0  i;\
			val = function.value(i);\
			if (i == 0, \{ sum = val \},\{ sum = sum + val \});\
		\});\
		^sum;\
	\}\
\}\
\
}