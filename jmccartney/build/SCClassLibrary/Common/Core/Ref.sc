{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue0;\red191\green0\blue0;\red0\green0\blue191;
\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 \
\cf3 // a Ref is a handle to a value. you can use it to return results by reference\
// example:   \
//		x = Ref.new(nil);\
// 		z = obj.method(x); // method puts something in reference\
//		x.value.doSomething; // retrieve value\
//\
// it is also used as a quoting device to insulate from multiChannelPerform in UGens\
//\
// A special syntax shortcut for Ref.new( expr ) is to use a backquote: `expr\
\cf2 \
\cf4 Ref\cf2  : \cf4 AbstractFunction\cf2  \
\{\
	\cf4 var\cf2  <>value;\
	*new \{ \cf4 arg\cf2  thing; ^\cf4 super\cf2 .new.value_(thing) \}\
	set \{ \cf4 arg\cf2  thing; value = thing \}\
	get \{ ^value \}\
	dereference \{ ^value \}\
	asRef \{ ^\cf4 this\cf2  \}\
	printOn \{ \cf4 arg\cf2  stream;\
		stream << \cf5 "`("\cf2  << value << \cf5 ")"\cf2 ;\
	\}\
	storeOn \{ \cf4 arg\cf2  stream;\
		stream << \cf5 "`("\cf2  <<< value << \cf5 ")"\cf2 ;\
	\}\
\}\
}