{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red0\green0\blue0;\red0\green115\blue0;
\red191\green0\blue0;\red96\green96\blue96;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 Model\cf3  \{\
	\cf2 var\cf3  <>dependants;\
	\
	changed \{ \cf2 arg\cf3  what ... moreArgs;\
		if (dependants.notNil, \{\
			dependants.do(\{ \cf2 arg\cf3  item;\
				item.performList(\cf4 \\update\cf3 , \cf2 this\cf3 , what, moreArgs);\
			\});\
		\});\
	\}\
	addDependant \{ \cf2 arg\cf3  dependant;\
		\cf2 this\cf3 .checkDependants;\
		dependants.add(dependant);\
	\}\
	removeDependant \{ \cf2 arg\cf3  dependant;\
		if (dependants.notNil, \{\
			dependants.remove(dependant);\
		\})\
	\}\
	release \{\
		dependants = \cf2 nil\cf3 ;\
	\}\
	checkDependants \{\
		if (dependants.isNil, \{\
			dependants = \cf2 IdentitySet\cf3 .new(4);\
		\})\
	\}\
\}\
\
\cf2 SimpleController\cf3  \{\
	\cf2 var\cf3  model, actions;\
	\cf5 // responds to updates of a model\
\cf3 	\
	*new \{ \cf2 arg\cf3  model;\
		^\cf2 super\cf3 .newCopyArgs(model).init\
	\}\
	init \{\
		model.addDependant(\cf2 this\cf3 );\
	\}\
	put \{ \cf2 arg\cf3  what, action;\
		if (actions.isNil, \{\
			actions = \cf2 IdentityDictionary\cf3 .new(4);\
		\});\
		actions.put(what, action);\
	\}\
	update \{ \cf2 arg\cf3  theChanger, what ... moreArgs;\
		\cf2 var\cf3  action;\
		action = actions.at(what);\
		if (action.notNil, \{\
			action.valueArray(theChanger, what, moreArgs);\
		\});\
	\}\
	remove \{\
		model.removeDependant(\cf2 this\cf3 );\
	\}\
\}\
\
\cf2 TestDependant\cf3  \{\
	update \{ \cf2 arg\cf3  thing;\
		(thing.asString ++ \cf6 " was changed.\\n"\cf3 ).post;\
	\}\
\}}