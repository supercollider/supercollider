{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Nil\cf0  \{\
	*new \{ ^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 ) \}\
	*newClear \{ ^\cf2 this\cf0 .shouldNotImplement(\cf2 thisMethod\cf0 ) \}\
	\
	isNil \{ ^\cf2 true\cf0  \}\
	notNil \{ ^\cf2 false\cf0  \}\
	? \{ \cf2 arg\cf0  obj; ^obj \}\
	?? \{ \cf2 arg\cf0  obj; ^obj.value \}\
	\
	\cf3 // support a nil Environment\
\cf0 	push \{ \cf2 arg\cf0  function; ^function.value \}\
	\
	\cf3 // support a nil Spawn event function\
\cf0 	spawn \{\}\
	\
	\cf3 // nil parent view\
\cf0 	asView \{\}\
\
	\cf3 // support a nil Plug\
\cf0 	source \{\}\
	source_ \{\}\
	\cf3 // nil Synth support\
\cf0 	setPlug \{\} \
	fadeEnd \{\}\
	\
	do \{\}\
	doReverse \{\}\
	\
	\cf3 // dependancy operators are no-ops\
\cf0 	dependants \{ 			\
		^\cf2 IdentitySet\cf0 .new \
	\}\
	changed \{\}\
	addDependant \{\}\
	removeDependant \{\}\
	release \{\}\
	update \{\}\
\
	\cf3 // nil Event support\
\cf0 	transformEvent \{ \cf2 arg\cf0  event;\
		^event\
	\}\
	awake \{\}\
	\
	swapThisGroup\{\}\
	\
	performMsg \{\}\
	\
	printOn \{ \cf2 arg\cf0  stream;\
		stream.putAll(\cf2 this\cf0 .asString);\
	\}\
	storeOn \{ \cf2 arg\cf0  stream;\
		stream.putAll(\cf2 this\cf0 .asCompileString);\
	\}\
	\
	\cf3 // Array support\
\cf0 	add \{ \cf2 arg\cf0  value; \
		\cf3 // This makes it unecessary to check for array.isNil when doing:\
\cf0 		\cf3 // array = array.add(thing);     Instead, it just works.\
\cf0 		^[value] \
	\}\
	addAll \{ \cf2 arg\cf0  array; ^array \}\
	++ \{ arg array; ^array \}\
	\
	\cf3 // graphical support\
\cf0 	draw\{\}\
	click \{ ^\cf2 nil\cf0  \}\
	fontID \{ ^\cf2 nil\cf0  \}\
	asRect \{ ^\cf2 Rect\cf0 .new \}\
	asArray \{ ^[] \}\
	\
	\cf3 // ControlView support\
\cf0 	set \{\}\
	get \{ \cf2 arg\cf0  prevVal; ^prevVal \}\
\}\
}