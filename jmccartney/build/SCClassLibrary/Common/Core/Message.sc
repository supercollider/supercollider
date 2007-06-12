{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red0\green0\blue191;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 Message\cf2  \{\
	\cf3 var\cf2  <>receiver, <>selector, <>args;\
	\
	*new \{ \cf3 arg\cf2  receiver, selector, args;\
		^\cf3 super\cf2 .newCopyArgs(receiver, selector, args);\
	\}\
	value \{ \cf3 arg\cf2  ... moreArgs;\
		^receiver.performList(selector, args ++ moreArgs);\
	\}\
\}\
\
\cf3 MethodQuote\cf2  \{\
	\cf3 var\cf2  <>selector;\
	\
	*new \{ \cf3 arg\cf2  selector;\
		^\cf3 super\cf2 .newCopyArgs(selector);\
	\}\
	value \{ \cf3 arg\cf2  receiver ... args;\
		^receiver.performList(selector, args);\
	\}\
\}\
}