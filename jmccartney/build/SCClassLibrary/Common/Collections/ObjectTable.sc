{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 TwoWayIdentityDictionary\cf0  : \cf2 Collection\cf0 \
\{\
	\cf2 var\cf0  idToObj, objToID;\
	\
	*new \{\
		^\cf2 super\cf0 .new.init;\
	\}\
	\
	add \{ \cf2 arg\cf0  anAssociation;\
		\cf2 this\cf0 .put(anAssociation.key, anAssociation.value);\
	\}\
	put \{ \cf2 arg\cf0  key, obj;\
		idToObj.put(key, obj);\
		objToID.put(obj, key);\
	\}\
	\
	remove \{ \cf2 arg\cf0  obj;\
		\cf2 var\cf0  key;\
		key = \cf2 this\cf0 .getID(obj);\
		idToObj.removeAt(key);\
		objToID.removeAt(obj);\
	\}\
	\
	do \{ \cf2 arg\cf0  function;\
		^idToObj.do(function);\
	\}\
	\
	at \{ \cf2 arg\cf0  id;\
		^idToObj.at(id);\
	\}\
	getID \{ \cf2 arg\cf0  obj;\
		^objToID.at(obj);\
	\}\
	\
	\cf3 // PRIVATE\
\cf0 	init \{\
		idToObj = \cf2 IdentityDictionary\cf0 .new;\
		objToID = \cf2 IdentityDictionary\cf0 .new;\
	\}\
\}\
\
\cf2 UniqueID\cf0  \{\
	\cf2 classvar\cf0  <id=1000;\
	*initClass \{ id = 1000; \}\
	*next  \{ ^id = id + 1; \}\
\}\
\
\cf2 ObjectTable\cf0  : \cf2 TwoWayIdentityDictionary\cf0 \
\{\
	\cf2 classvar\cf0  <global;\
	\
	\
	*new \{\
		^\cf2 super\cf0 .new;\
	\}\
	\
	add \{ \cf2 arg\cf0  obj;\
		\cf2 this\cf0 .put(\cf2 UniqueID\cf0 .next, obj);\
	\}\
	\
	*initClass \{\
		global = \cf2 this\cf0 .new;\
	\}\
	*add \{ \cf2 arg\cf0  obj;\
		global.add(obj);\
		^\cf2 UniqueID\cf0 .id\
	\}\
	*put \{ \cf2 arg\cf0  key, obj;\
		global.put(key, obj);\
	\}\
	*remove \{ \cf2 arg\cf0  obj;\
		global.remove(obj);\
	\}\
	*at \{ \cf2 arg\cf0  id;\
		^global.at(id);\
	\}\
	*getID \{ \cf2 arg\cf0  obj;\
		^global.getID(obj);\
	\}\
	*objPerform \{ \cf2 arg\cf0  id, selector ... args;\
		\cf2 var\cf0  obj;\
		obj = global.at(id); \
		if (obj.notNil, \{\
			obj.performList(selector, args);\
		\});\
	\}\
\}\
\
\
}