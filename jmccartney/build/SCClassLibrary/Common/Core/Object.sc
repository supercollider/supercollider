{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red0\green0\blue191;\red191\green0\blue0;\red96\green96\blue96;
\red0\green115\blue0;}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf0 \
\cf2 Object\cf0  \{\
	\cf2 classvar\cf0  <dependantsDictionary, currentEnvironment, topEnvironment, <uniqueMethods; \
	\
	*new \{ \cf2 arg\cf0  maxSize = 0; \
		\cf2 _BasicNew\cf0  \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 // creates a new instance that can hold up to maxSize \
\cf0 		\cf3 // indexable slots. the indexed size will be zero.\
\cf0 		\cf3 // to actually put things in the object you need to\
\cf0 		\cf3 // add them.\
\cf0 	\}\
	*newClear \{ \cf2 arg\cf0  indexedSize = 0; \
		\cf2 _BasicNewClear\cf0  \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 // creates a new instance with indexedSize indexable slots.\
\cf0 		\cf3 // the slots are filled with nil, zero or something else\
\cf0 		\cf3 // appropriate to the type of indexable slots in the\
\cf0 		\cf3 // object.\
\cf0 	\}\
	*newCopyArgs \{ \cf2 arg\cf0  ... args; \
		\cf2 _BasicNewCopyArgsToInstVars\cf0  \
		^\cf2 this\cf0 .primitiveFailed\
		\cf3 // creates a new instance that can hold up to maxSize \
\cf0 		\cf3 // indexable slots. the indexed size will be zero.\
\cf0 		\cf3 // to actually put things in the object you need to\
\cf0 		\cf3 // add them.\
\cf0 	\}\
		\
	\cf3 // debugging and diagnostics\
\cf0 	dump \{ \cf2 _ObjectDump\cf0  \}\
	post \{ \cf2 this\cf0 .asString.post \}\
	postln \{ \cf2 this\cf0 .asString.postln; \}\
	postc \{ \cf2 this\cf0 .asString.postc \}\
	postcln \{ \cf2 this\cf0 .asString.postcln; \}\
	totalFree \{ \cf2 _TotalFree\cf0  \}\
	largestFreeBlock \{ \cf2 _LargestFreeBlock\cf0  \}\
	macTotalFree \{ \cf2 _MacTotalFree\cf0  \}\
	macLargestFreeBlock \{ \cf2 _MacLargestFreeBlock\cf0  \}\
	gcAll \{ \cf2 _GCAll\cf0  \}\
	gcInfo \{ \cf2 _GCInfo\cf0  \}\
	gcSanity \{ \cf2 _GCSanity\cf0  \}\
	\
		\
	\cf3 //accessing\
\cf0 	size \{ ^0 \}\
	\
	instVarSize \{ \cf2 _InstVarSize\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	instVarAt \{ \cf2 arg\cf0  index; \
		\cf2 _InstVarAt\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	instVarPut \{ \cf2 arg\cf0  index, item; \
		\cf2 _InstVarPut\cf0 ;\
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	\
	\cf3 // see counterparts to these in ArrayedCollection \
\cf0 	slotSize \{\
		^\cf2 this\cf0 .instVarSize;\
	\}\
	slotAt \{ \cf2 arg\cf0  index;\
		^\cf2 this\cf0 .instVarAt(index);\
	\}\
	slotPut \{ \cf2 arg\cf0  index, value;\
		^\cf2 this\cf0 .instVarPut(index, value);\
	\}\
	slotKey \{ \cf2 arg\cf0  index;\
		^\cf2 this\cf0 .class.instVarNames.at(index)\
	\}\
	\
	\cf3 //do \{ arg function; function.value(this, 0) \}\
\cf0 	\cf3 //reverseDo \{ arg function; function.value(this, 0) \}\
\cf0 	\
	\cf3 // class membership\
\cf0 	class \{ \cf2 _ObjectClass\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	isKindOf \{ \cf2 arg\cf0  aClass; \cf2 _ObjectIsKindOf\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	isMemberOf \{ \cf2 arg\cf0  aClass; \cf2 _ObjectIsMemberOf\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	respondsTo \{ \cf2 arg\cf0  aSymbol; \cf2 _ObjectRespondsTo\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	perform \{ \cf2 arg\cf0  selector ... args;\
		\cf2 _ObjectPerform\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	performMsg \{ \cf2 arg\cf0  msg;\
		\cf2 _ObjectPerformMsg\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	\
	performList \{ \cf2 arg\cf0  selector, arglist;\
		\cf2 _ObjectPerformList\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	\
	\cf3 // copying\
\cf0 	copy \{ ^\cf2 this\cf0 .shallowCopy \}\
	contentsCopy \{ ^\cf2 this\cf0 .shallowCopy \}\
	shallowCopy \{ \cf2 _ObjectShallowCopy\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	copyImmutable \{ \
		\cf3 // if object is immutable then return a shallow copy, else return receiver.\
\cf0 		\cf2 _ObjectCopyImmutable\cf0 ; \
		^\cf2 this\cf0 .primitiveFailed \
	\}\
	\
	deepCopy \{\
		\cf2 _ObjectDeepCopy\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
			\
	\cf3 // evaluation\
\cf0 	poll \{ ^\cf2 this\cf0 .value \}\
	value \{ ^\cf2 this\cf0  \}\
	valueArray \{ ^\cf2 this\cf0  \}\
	\
	\cf3 // equality, identity\
\cf0 	== \{ \cf2 arg\cf0  obj; ^\cf2 this\cf0  === obj \}\
	!= \{ \cf2 arg\cf0  obj; ^\cf2 this\cf0  !== obj \}\
	=== \{ \cf2 arg\cf0  obj; \cf2 _Identical\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	!== \{ \cf2 arg\cf0  obj;\cf2 _NotIdentical\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	\
	basicHash \{ \cf2 _ObjectHash\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	hash \{ \cf2 _ObjectHash\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	identityHash \{ \cf2 _ObjectHash\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	uniqueID \{ \cf2 _ObjectID\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	objPtr \{ \cf2 _ObjectPtr\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
		\
	\cf3 // create an association\
\cf0 	-> \{ \cf2 arg\cf0  obj; ^\cf2 Association\cf0 .new(\cf2 this\cf0 , obj) \}\
	\
	\cf3 // stream\
\cf0 	next \{ ^\cf2 this\cf0  \}\
	reset \{ ^\cf2 this\cf0  \}\
	embedInStream \{ \cf2 this\cf0 .yield; \}\
	asStream \{ ^\cf2 this\cf0  \}\
	eventAt \{ ^\cf2 nil\cf0  \}\
	finishEvent \{\}\
	atLimit \{ ^false \}\
\
\cf3 //	Lazy lists have been removed..	\
//	// support for lazy lists\
//	isLazyList \{ ^false \}	\
//	demandThisNode \{ \
//		^LazyList.new(this, this.next);\
//	\}\
\cf0 	\
	\cf3 // testing\
\cf0 	? \{ \cf2 arg\cf0  obj; ^\cf2 this\cf0  \}\
	?? \{ \cf2 arg\cf0  obj; ^\cf2 this\cf0  \}\
	isNil \{ ^\cf2 false\cf0  \}\
	notNil \{ ^\cf2 true\cf0  \}\
	isNumber \{ ^\cf2 false\cf0  \}\
	isInteger \{ ^\cf2 false\cf0  \}\
	isFloat \{ ^\cf2 false\cf0  \}	\
	isValidUGenInput \{ ^\cf2 false\cf0  \}\
	pointsTo \{ \cf2 arg\cf0  obj; \cf2 _ObjectPointsTo\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	mutable \{ \cf2 _ObjectIsMutable\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	frozen \{ \cf2 _ObjectIsPermanent\cf0 ; ^\cf2 this\cf0 .primitiveFailed \}\
	\
	\cf3 // errors\
\cf0 	halt \{ \
		\cf2 MixerPlayer\cf0 .reset;\
		currentEnvironment = topEnvironment; \cf3 // restore top Environment\
\cf0 		\cf2 this\cf0 .prHalt \
	\}\
	prHalt \{ \cf2 _Halt\cf0  \}\
	primitiveFailed \{ \
		error(\cf4 "Primitive '"\cf0  \
			++ \cf2 thisThread\cf0 .failedPrimitiveName.asString \
			++ \cf4 "' failed.\\n"\cf0 ); \
		\cf2 Thread\cf0 .primitiveErrorString.postln; \
		\cf2 this\cf0 .verboseHalt;\
	\}\
	verboseHalt \{\
		\cf4 "RECEIVER:\\n"\cf0 .post;\
		\cf2 this\cf0 .dump;\
		\cf2 this\cf0 .dumpBackTrace;\
		\cf2 this\cf0 .halt \
	\}\
	\
	subclassResponsibility \{ \cf2 arg\cf0  method;\
		error( \cf4 "'"\cf0  ++ method.name.asString ++ \cf4 "' should have been implemented by "\cf0 \
			++ \cf2 this\cf0 .class.name.asString ++ \cf4 ".\\n"\cf0 );\
		\cf2 this\cf0 .dumpBackTrace;\
		\cf2 this\cf0 .halt;\
	\}\
	doesNotUnderstand \{ \cf2 arg\cf0  selector ... args;\
		\cf3 /*\
		// BUILT INTO VIRTUAL MACHINE\
		var methodDict, methodFunc;\
		// check for an instance specific method\
		if (uniqueMethods.notNil \
			and: \{ (methodDict = uniqueMethods.at(this)).notNil \}\
			and: \{ (methodFunc = methodDict.at(selector)).notNil \},\{\
			^methodFunc.valueArray(args)\
		\});\
		*/\cf0 \
		\
		error(\cf4 "Message '"\cf0  \
			++ selector.asString\
			++ \cf4 "' not understood.\\n"\cf0 ); \
		\cf4 "RECEIVER:\\n"\cf0 .post;\
		\cf2 this\cf0 .dump;\
		\cf4 "ARGS:\\n"\cf0 .post;\
		args.dump;\
		\cf2 this\cf0 .dumpBackTrace;\
		\cf2 this\cf0 .halt  \
	\}\
	shouldNotImplement \{ \cf2 arg\cf0  method;\
		error(method.ownerClass.name.asString ++ \cf4 "::"\cf0  ++ method.name.asString \
			++ \cf4 " : Message not valid for this subclass\\n"\cf0 );\
		\cf2 this\cf0 .dumpBackTrace;\
		\cf2 this\cf0 .halt;\
	\} \
	mustBeBoolean \{\
		error(\cf4 "Non Boolean in test:\\n"\cf0 );\
		\cf2 this\cf0 .dump;\
		\cf2 this\cf0 .dumpBackTrace;\
		\cf2 this\cf0 .halt;\
	\}\
	nonIntegerIndex \{ error(\cf4 "non Integer index used.\\n"\cf0 ) \}\
	indexOutOfRange \{ error(\cf4 "Index out of range.\\n"\cf0 ) \}\
	notYetImplemented \{ inform(\cf4 "Not yet implemented.\\n"\cf0 ) \}\
	dumpBackTrace \{ \cf2 _DumpBackTrace\cf0  \}\
	getBackTrace \{ \cf2 _GetBackTrace\cf0  \}\
		\
	\cf3 // conversion\
\cf0 	species \{ ^\cf2 this\cf0 .class \}\
	asSymbol \{ ^\cf2 this\cf0 .asString.asSymbol \}\
	asString \{ \cf2 arg\cf0  limit = 100;\
		\cf2 var\cf0  string;\
		\cf2 _ObjectString\cf0 \
		string = \cf2 String\cf0 .streamContentsLimit(\{ \cf2 arg\cf0  stream; \cf2 this\cf0 .printOn(stream); \}, limit);\
		if (string.size >= limit, \{ string = string ++ \cf4 "...etc..."\cf0 ; \});\
		^string\
	\}\
	asCompileString \{\
		\cf2 _ObjectCompileString\cf0  \
		^\cf2 String\cf0 .streamContents(\{ \cf2 arg\cf0  stream; \cf2 this\cf0 .storeOn(stream); \});\
	\}\
	printOn \{ \cf2 arg\cf0  stream;\
		\cf2 var\cf0  title;\
		title = \cf2 this\cf0 .class.name.asString;\
		stream << if((title @ 0).isVowel, \{ \cf4 "an "\cf0  \}, \{ \cf4 "a "\cf0  \}) << title ;\
	\}\
	storeOn \{ \cf2 arg\cf0  stream;\
		\cf2 var\cf0  title;\
		stream << \cf2 this\cf0 .class.name << \cf4 ".new"\cf0 ;\
		\cf2 this\cf0 .storeParamsOn(stream);\
		\cf2 this\cf0 .storeModifiersOn(stream);\
	\}\
	storeParamsOn \{ \cf2 arg\cf0  stream;\
	\}\
	storeModifiersOn \{ \cf2 arg\cf0  stream;\
	\}\
\
	\
	as \{ \cf2 arg\cf0  aSimilarClass; ^aSimilarClass.newFrom(\cf2 this\cf0 ) \}\
	dereference \{ ^\cf2 this\cf0  \} \cf3 // see Ref::dereference\
\cf0 	asRef \{ ^\cf2 Ref\cf0 .new(\cf2 this\cf0 ) \}\
	asArray \{ ^\cf2 Array\cf0 .with(\cf2 this\cf0 ) \}\
	\
	\cf3 // looping\
\cf0 	while \{ \cf2 arg\cf0  body;\
		\cf3 // compiler magic: the compiler inlines the following loop\
\cf0 		\cf3 // thus an uninlinable while can be implemented using while itself\
\cf0 		while(\{ \cf2 this\cf0 .value \}, \{\
			body.value\
		\});\
	\}\
	\
	\cf3 // coroutine support\
\cf0 	yield \{ \cf2 arg\cf0  val;\
		\cf2 _RoutineYield\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	alwaysYield \{ \cf2 arg\cf0  val;\
		\cf2 _RoutineAlwaysYield\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	yieldAndReset \{ \cf2 arg\cf0  val, reset = \cf2 true\cf0 ;\
		\cf2 _RoutineYieldAndReset\cf0 \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	\
	\cf3 // dependancy support\
\cf0 	dependants \{\
		if (dependantsDictionary.isNil, \{ \
			^\cf2 IdentitySet\cf0 .new \
		\},\{\
			^dependantsDictionary.atFail(\cf2 this\cf0 , \{ ^\cf2 IdentitySet\cf0 .new \});\
		\})\
	\}\
	changed \{ \cf2 arg\cf0  theChanger;\
		\cf2 var\cf0  theDependants;\
		theDependants = \cf2 this\cf0 .dependants;\
		theDependants.do(\{ \cf2 arg\cf0  item;\
			item.update(\cf2 this\cf0 , theChanger);\
		\});\
	\}\
	addDependant \{ \cf2 arg\cf0  dependant;\
		\cf2 var\cf0  theDependants;\
		if (dependantsDictionary.isNil, \{\
			dependantsDictionary = \cf2 IdentityDictionary\cf0 .new(4);\
		\});\
		theDependants = \cf2 this\cf0 .dependants;\
		dependantsDictionary.put(\cf2 this\cf0 , theDependants.add(dependant));\
	\}\
	removeDependant \{ \cf2 arg\cf0  dependant;\
		\cf2 var\cf0  theDependants;\
		if (dependantsDictionary.notNil, \{ \
			theDependants = dependantsDictionary.atFail(\cf2 this\cf0 );\
			if (theDependants.notNil, \{\
				theDependants.remove(dependant);\
				if (theDependants.size == 0, \{\
					dependantsDictionary.removeAt(\cf2 this\cf0 );\
				\});\
			\});\
		\})\
	\}\
	release \{\
		if (dependantsDictionary.notNil, \{\
			dependantsDictionary.removeAt(\cf2 this\cf0 );\
		\})\
	\}\
	update \{ \cf2 arg\cf0  theChanged, theChanger;	\cf3 // respond to a change in a model\
\cf0 	\}\
\
	\
	\cf3 // instance specific method support\
\cf0 	addUniqueMethod \{ \cf2 arg\cf0  selector, function;\
		\cf2 var\cf0  methodDict;\
		if (uniqueMethods.isNil, \{ uniqueMethods = \cf2 IdentityDictionary\cf0 .new \});\
		methodDict = uniqueMethods.at(\cf2 this\cf0 );\
		if (methodDict.isNil, \{\
			methodDict = \cf2 IdentityDictionary\cf0 .new;\
			uniqueMethods.put(\cf2 this\cf0 , methodDict);\
		\});\
		methodDict.put(selector, function);\
	\}\
	removeUniqueMethods \{\
		if (uniqueMethods.notNil, \{ \
			uniqueMethods.removeAt(\cf2 this\cf0 );\
		\});\
	\}\
	removeUniqueMethod \{ \cf2 arg\cf0  selector;\
		\cf2 var\cf0  methodDict;\
		if (uniqueMethods.notNil, \{\
			methodDict = uniqueMethods.at(\cf2 this\cf0 );\
			if (methodDict.notNil, \{\
				methodDict.removeAt(selector);\
				if (methodDict.size < 1, \{\
					uniqueMethods.removeAt(\cf2 this\cf0 );\
				\});\
			\});\
		\});		\
	\}\
\
	mouseDown \{ ^\cf2 nil\cf0  \}\
	mouseOver \{ ^\cf2 nil\cf0  \}\
	keyDown \{ \}\
		\
	\cf3 // layout support\
\cf0 	getLayoutSize \{ ^\cf2 LayoutSize\cf0 .new \}\
	layout \{\}\
\
	inspect \{ \
		if (\cf2 this\cf0 .canInspect, \{ \cf2 this\cf0 .inspectorClass.new(\cf2 this\cf0 ) \}) \
	\}\
	canInspect \{ ^\cf2 true\cf0  \}\
	inspectorClass \{ ^\cf2 ObjectInspector\cf0  \}\
	inspector \{ \
		\cf3 // finds the inspector for this object, if any.\
\cf0 		^\cf2 Inspector\cf0 .inspectorFor(\cf2 this\cf0 ) \
	\}\
\
\
	\cf3 // virtual machine debugging...\
\cf0 	crash \{ \cf2 _HostDebugger\cf0  \} \cf3 // for serious problems..\
\cf0 	stackDepth \{ \cf2 _StackDepth\cf0  \}\
	dumpStack \{ \cf2 _DumpStack\cf0  \}\
	dumpDetailedBackTrace \{ \cf2 _DumpDetailedBackTrace\cf0  \}\
	\
	\cf3 // archiving\
\cf0 	writeArchive \{ \cf2 arg\cf0  pathname;\
		\cf2 _WriteArchive\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	*readArchive \{ \cf2 arg\cf0  pathname;\
		\cf2 _ReadArchive\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	asArchive \{\
		\cf2 _AsArchive\cf0 \
		^\cf2 this\cf0 .primitiveFailed;\
	\}\
	\
	freeze \{ \
		\cf2 _ObjectDeepFreeze\cf0  \
		^\cf2 this\cf0 .primitiveFailed\
	\}\
	\
	\cf3 // Math protocol support\
\cf0 	\cf3 // translate these operators to names the code generator can safely generate in C++	\
\cf0 	& \{ \cf2 arg\cf0  that; ^bitAnd(\cf2 this\cf0 , that) \}\
	| \{ \cf2 arg\cf0  that; ^bitOr(\cf2 this\cf0 , that) \}\
	% \{ \cf2 arg\cf0  that; ^mod(\cf2 this\cf0 , that) \}\
	** \{ \cf2 arg\cf0  that; ^pow(\cf2 this\cf0 , that) \}\
	<< \{ \cf2 arg\cf0  that; ^leftShift(\cf2 this\cf0 , that) \}\
	>> \{ \cf2 arg\cf0  that; ^rightShift(\cf2 this\cf0 , that) \}\
	+>> \{ \cf2 arg\cf0  that; ^unsignedRightShift(\cf2 this\cf0 , that) \}\
	<! \{ \cf2 arg\cf0  that; ^firstArg(\cf2 this\cf0 , that) \}\
	\
	blend \{ \cf2 arg\cf0  that, blendFrac = 0.5;\
		\cf3 // blendFrac should be from zero to one\
\cf0 		^\cf2 this\cf0  + (blendFrac * (that - \cf2 this\cf0 ));\
	\}\
	fuzzyEqual \{ \cf2 arg\cf0  that; ^max(0.0, abs(1.0 - (\cf2 this\cf0  - that))) \}		\
	isUGen \{ ^\cf2 false\cf0  \}\
\
	\cf3 // scheduling\
\cf0 	awake \{ \cf2 arg\cf0  inTime;\
		^\cf2 this\cf0 .value(inTime)\
	\}\
\
	\cf3 // catch binary operators failure\
\cf0 	performBinaryOpOnSomething \{ \cf2 arg\cf0  aSelector;\
		if (aSelector === \cf5 '=='\cf0 , \{\
			^\cf2 false\cf0 \
		\},\{\
		if (aSelector === \cf5 '!='\cf0 , \{\
			^\cf2 true\cf0 \
		\},\{\
			error(\cf4 "performBinaryOp failed.\\n"\cf0 );\
			\cf2 this\cf0 .dumpBackTrace;\
			\cf2 this\cf0 .flushErrors;\
			\cf2 this\cf0 .halt \
		\})\});\
	\}\
	performBinaryOpOnSimpleNumber \{ \cf2 arg\cf0  aSelector;\
		^\cf2 this\cf0 .performBinaryOpOnSomething(aSelector)\
	\}\
	performBinaryOpOnSignal \{ \cf2 arg\cf0  aSelector;\
		^\cf2 this\cf0 .performBinaryOpOnSomething(aSelector)\
	\}\
	performBinaryOpOnComplex \{ \cf2 arg\cf0  aSelector;\
		^\cf2 this\cf0 .performBinaryOpOnSomething(aSelector)\
	\}\
	performBinaryOpOnSeqColl \{ \cf2 arg\cf0  aSelector;\
		^\cf2 this\cf0 .performBinaryOpOnSomething(aSelector)\
	\}\
	performBinaryOpOnUGen \{ \cf2 arg\cf0  aSelector;\
		^\cf2 this\cf0 .performBinaryOpOnSomething(aSelector)\
	\}\
	performBinaryOpOnInfinitum \{ \cf2 arg\cf0  aSelector;\
		^\cf2 this\cf0 .performBinaryOpOnSomething(aSelector)\
	\}\
\}\
}