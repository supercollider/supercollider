
Object {
	classvar <dependantsDictionary, currentEnvironment, topEnvironment, <uniqueMethods; 
	classvar <nl="\n";
	
	*new { arg maxSize = 0; 
		_BasicNew 
		^this.primitiveFailed
		// creates a new instance that can hold up to maxSize 
		// indexable slots. the indexed size will be zero.
		// to actually put things in the object you need to
		// add them.
	}
	*newClear { arg indexedSize = 0; 
		_BasicNewClear 
		^this.primitiveFailed
		// creates a new instance with indexedSize indexable slots.
		// the slots are filled with nil, zero or something else
		// appropriate to the type of indexable slots in the
		// object.
	}
	*newCopyArgs { arg ... args; 
		_BasicNewCopyArgsToInstVars 
		^this.primitiveFailed
		// creates a new instance that can hold up to maxSize 
		// indexable slots. the indexed size will be zero.
		// to actually put things in the object you need to
		// add them.
	}
		
	// debugging and diagnostics
	dump { _ObjectDump }
	post { this.asString.post }
	postln { this.asString.postln; }
	postc { this.asString.postc }
	postcln { this.asString.postcln; }
	totalFree { _TotalFree }
	largestFreeBlock { _LargestFreeBlock }
	macTotalFree { _MacTotalFree }
	macLargestFreeBlock { _MacLargestFreeBlock }
	gcAll { _GCAll }
	gcInfo { _GCInfo }
	gcSanity { _GCSanity }
	
		
	//accessing
	size { ^0 }
	
	instVarSize { _InstVarSize; ^this.primitiveFailed }
	instVarAt { arg index; 
		_InstVarAt;
		^this.primitiveFailed;
	}
	instVarPut { arg index, item; 
		_InstVarPut;
		^this.primitiveFailed;
	}
	
	// see counterparts to these in ArrayedCollection 
	slotSize {
		^this.instVarSize;
	}
	slotAt { arg index;
		^this.instVarAt(index);
	}
	slotPut { arg index, value;
		^this.instVarPut(index, value);
	}
	slotKey { arg index;
		^this.class.instVarNames.at(index)
	}
	
	do { arg function; function.value(this, 0) }
	//reverseDo { arg function; function.value(this, 0) }
	
	// class membership
	class { _ObjectClass; ^this.primitiveFailed }
	isKindOf { arg aClass; _ObjectIsKindOf; ^this.primitiveFailed }
	isMemberOf { arg aClass; _ObjectIsMemberOf; ^this.primitiveFailed }
	respondsTo { arg aSymbol; _ObjectRespondsTo; ^this.primitiveFailed }
	perform { arg selector ... args;
		_ObjectPerform; 
		^this.primitiveFailed 
	}
	performMsg { arg msg;
		_ObjectPerformMsg; 
		^this.primitiveFailed 
	}
	
	performList { arg selector, arglist;
		_ObjectPerformList; 
		^this.primitiveFailed 
	}
	tryPerform { arg selector ... args;
		^if(this.respondsTo(selector),{
			this.performList(selector,args)
		},{
			nil // you can use ?? for an else clause
		})
	}

	// copying
	copy { ^this.shallowCopy }
	contentsCopy { ^this.shallowCopy }
	shallowCopy { _ObjectShallowCopy; ^this.primitiveFailed }
	copyImmutable { 
		// if object is immutable then return a shallow copy, else return receiver.
		_ObjectCopyImmutable; 
		^this.primitiveFailed 
	}
	
	deepCopy {
		_ObjectDeepCopy
		^this.primitiveFailed
	}
			
	// evaluation
	poll { ^this.value }
	value { ^this }
	valueArray { ^this }
	
	// equality, identity
	== { arg obj; ^this === obj }
	!= { arg obj; ^this !== obj }
	=== { arg obj; _Identical; ^this.primitiveFailed }
	!== { arg obj;_NotIdentical; ^this.primitiveFailed }
	
	basicHash { _ObjectHash; ^this.primitiveFailed }
	hash { _ObjectHash; ^this.primitiveFailed }
	identityHash { _ObjectHash; ^this.primitiveFailed }
	//uniqueID { _ObjectID; ^this.primitiveFailed }
	//objPtr { _ObjectPtr; ^this.primitiveFailed }
		
	// create an association
	-> { arg obj; ^Association.new(this, obj) }
	
	// stream
	next { ^this }
	reset { ^this }
	embedInStream { this.yield; }
	asStream { ^this }
	eventAt { ^nil }
	finishEvent {}
	atLimit { ^false }
	
	// testing
	? { arg obj; ^this }
	?? { arg obj; ^this }
	isNil { ^false }
	notNil { ^true }
	isNumber { ^false }
	isInteger { ^false }
	isFloat { ^false }	
	isSequenceableCollection { ^false }
	isArray { ^false }
	isString { ^false }
	isValidUGenInput { ^false }
	pointsTo { arg obj; _ObjectPointsTo; ^this.primitiveFailed }
	mutable { _ObjectIsMutable; ^this.primitiveFailed }
	frozen { _ObjectIsPermanent; ^this.primitiveFailed }
	
	// errors
	halt { 
		currentEnvironment = topEnvironment; // restore top Environment
		CocoaDialog.clear;
		this.prHalt 
	}
	prHalt { _Halt }
	primitiveFailed { 
		error("Primitive '" 
			++ thisThread.failedPrimitiveName.asString 
			++ "' failed.\n"); 
		Thread.primitiveErrorString.postln; 
		this.verboseHalt;
	}
	verboseHalt {
		"RECEIVER:\n".post;
		this.dump;
		this.dumpBackTrace;
		this.halt 
	}
	
	subclassResponsibility { arg method;
		error( "'" ++ method.name.asString ++ "' should have been implemented by "
			++ this.class.name.asString ++ ".\n");
		this.dumpBackTrace;
		this.halt;
	}
	doesNotUnderstand { arg selector ... args;
		/*
		// BUILT INTO VIRTUAL MACHINE
		var methodDict, methodFunc;
		// check for an instance specific method
		if (uniqueMethods.notNil 
			and: { (methodDict = uniqueMethods.at(this)).notNil }
			and: { (methodFunc = methodDict.at(selector)).notNil },{
			^methodFunc.valueArray(args)
		});
		*/
		
		error("Message '" 
			++ selector.asString
			++ "' not understood.\n"); 
		"RECEIVER:\n".post;
		this.dump;
		"ARGS:\n".post;
		args.dump;
		this.dumpBackTrace;
		this.halt  
	}
	shouldNotImplement { arg method;
		error(method.ownerClass.name.asString ++ "::" ++ method.name.asString 
			++ " : Message not valid for this subclass\n");
		this.dumpBackTrace;
		this.halt;
	} 
	mustBeBoolean {
		error("Non Boolean in test:\n");
		this.dump;
		this.dumpBackTrace;
		this.halt;
	}
	nonIntegerIndex { error("non Integer index used.\n") }
	indexOutOfRange { error("Index out of range.\n") }
	notYetImplemented { inform(this.asString + "Not yet implemented.\n") }
	dumpBackTrace { _DumpBackTrace }
	getBackTrace { _GetBackTrace }
		
	// conversion
	species { ^this.class }
	asCollection { ^[this] }
	asSymbol { ^this.asString.asSymbol }
	asString { arg limit = 150;
		var string;
		_ObjectString
		string = String.streamContentsLimit({ arg stream; this.printOn(stream); }, limit);
		if (string.size >= limit, { string = string ++ "...etc..."; });
		^string
	}
	asCompileString {
		_ObjectCompileString 
		^String.streamContents({ arg stream; this.storeOn(stream); });
	}
	printOn { arg stream;
		var title;
		title = this.class.name.asString;
		stream << if((title @ 0).isVowel, { "an " }, { "a " }) << title ;
	}
	storeOn { arg stream;
		var title;
		stream << this.class.name << ".new";
		this.storeParamsOn(stream);
		this.storeModifiersOn(stream);
	}
	storeParamsOn { arg stream;
		var args;
		args = this.storeArgs;
		if(args.notEmpty,{
			stream << "(" <<<* args << ")";
		})
	}
	storeArgs { ^#[] }
	storeModifiersOn { arg stream;}

	
	as { arg aSimilarClass; ^aSimilarClass.newFrom(this) }
	dereference { ^this } // see Ref::dereference
	asRef { ^Ref.new(this) }
	asArray { ^Array.with(this) }
	
	// looping
	while { arg body;
		// compiler magic: the compiler inlines the following loop
		// thus an uninlinable while can be implemented using while itself
		while({ this.value }, {
			body.value
		});
	}
	
	// coroutine support
	yield { arg val;
		_RoutineYield
		^this.primitiveFailed
	}
	alwaysYield { arg val;
		_RoutineAlwaysYield
		^this.primitiveFailed
	}
	yieldAndReset { arg val, reset = true;
		_RoutineYieldAndReset
		^this.primitiveFailed
	}
	
	// dependancy support
	dependants {
		if (dependantsDictionary.isNil, { 
			^IdentitySet.new 
		},{
			^dependantsDictionary.atFail(this, { ^IdentitySet.new });
		})
	}
	changed { arg theChanger;
		var theDependants;
		theDependants = this.dependants;
		theDependants.do({ arg item;
			item.update(this, theChanger);
		});
	}
	addDependant { arg dependant;
		var theDependants;
		if (dependantsDictionary.isNil, {
			dependantsDictionary = IdentityDictionary.new(4);
		});
		theDependants = this.dependants;
		dependantsDictionary.put(this, theDependants.add(dependant));
	}
	removeDependant { arg dependant;
		var theDependants;
		if (dependantsDictionary.notNil, { 
			theDependants = dependantsDictionary.atFail(this);
			if (theDependants.notNil, {
				theDependants.remove(dependant);
				if (theDependants.size == 0, {
					dependantsDictionary.removeAt(this);
				});
			});
		})
	}
	release {
		if (dependantsDictionary.notNil, {
			dependantsDictionary.removeAt(this);
		})
	}
	update { arg theChanged, theChanger;	// respond to a change in a model
	}

	
	// instance specific method support
	addUniqueMethod { arg selector, function;
		var methodDict;
		if (uniqueMethods.isNil, { uniqueMethods = IdentityDictionary.new });
		methodDict = uniqueMethods.at(this);
		if (methodDict.isNil, {
			methodDict = IdentityDictionary.new;
			uniqueMethods.put(this, methodDict);
		});
		methodDict.put(selector, function);
	}
	removeUniqueMethods {
		if (uniqueMethods.notNil, { 
			uniqueMethods.removeAt(this);
		});
	}
	removeUniqueMethod { arg selector;
		var methodDict;
		if (uniqueMethods.notNil, {
			methodDict = uniqueMethods.at(this);
			if (methodDict.notNil, {
				methodDict.removeAt(selector);
				if (methodDict.size < 1, {
					uniqueMethods.removeAt(this);
				});
			});
		});		
	}

	mouseDown { ^nil }
	mouseOver { ^nil }
	keyDown { }
		
	// layout support
	getLayoutSize { ^LayoutSize.new }
	layout {}

	inspect { ^this.inspectorClass.new(this) }
	inspectorClass { ^ObjectInspector }
	inspector { 
		// finds the inspector for this object, if any.
		^Inspector.inspectorFor(this) 
	}


	// virtual machine debugging...
	crash { _HostDebugger } // for serious problems..
	stackDepth { _StackDepth }
	dumpStack { _DumpStack }
	dumpDetailedBackTrace { _DumpDetailedBackTrace }
	
	// archiving
	writeArchive { arg pathname;
		_WriteArchive
		^this.primitiveFailed;
	}
	*readArchive { arg pathname;
		_ReadArchive
		^this.primitiveFailed;
	}
	asArchive {
		_AsArchive
		^this.primitiveFailed;
	}
	
	writeAsPlist { arg pathname;
		_Cocoa_SaveAsPlist
		^this.primitiveFailed;
	}
	
	freeze { 
		_ObjectDeepFreeze 
		^this.primitiveFailed
	}
	
	// Math protocol support
	// translate these operators to names the code generator can safely generate in C++	
	& { arg that; ^bitAnd(this, that) }
	| { arg that; ^bitOr(this, that) }
	% { arg that; ^mod(this, that) }
	** { arg that; ^pow(this, that) }
	<< { arg that; ^leftShift(this, that) }
	>> { arg that; ^rightShift(this, that) }
	+>> { arg that; ^unsignedRightShift(this, that) }
	<! { arg that; ^firstArg(this, that) }
	
	blend { arg that, blendFrac = 0.5;
		// blendFrac should be from zero to one
		^this + (blendFrac * (that - this));
	}
	
	blendAt { arg index;
		var iMin;
		iMin = index.asInteger;
		^blend(this.at(iMin), this.clipAt(iMin+1), index - iMin);
	}

	fuzzyEqual { arg that, precision=1.0; ^max(0.0, 1.0 - (abs(this - that)/precision)) }
		
	isUGen { ^false }

	// scheduling
	awake { arg inTime;
		var time;
		time = inTime; // prevent optimization
		^this.next(inTime)
	}
	time_ {  } // for PauseStream

	// catch binary operators failure
	performBinaryOpOnSomething { arg aSelector;
		if (aSelector === '==', {
			^false
		},{
		if (aSelector === '!=', {
			^true
		},{
			error("performBinaryOp failed.\n");
			this.dumpBackTrace;
			this.halt 
		})});
	}
	performBinaryOpOnSimpleNumber { arg aSelector;
		^this.performBinaryOpOnSomething(aSelector)
	}
	performBinaryOpOnSignal { arg aSelector;
		^this.performBinaryOpOnSomething(aSelector)
	}
	performBinaryOpOnComplex { arg aSelector;
		^this.performBinaryOpOnSomething(aSelector)
	}
	performBinaryOpOnSeqColl { arg aSelector;
		^this.performBinaryOpOnSomething(aSelector)
	}
	performBinaryOpOnUGen { arg aSelector;
		^this.performBinaryOpOnSomething(aSelector)
	}
	performBinaryOpOnInfinitum { arg aSelector;
		^this.performBinaryOpOnSomething(aSelector)
	}
	
	writeDefFile { arg name, dir="synthdefs/";
		var file;
		
		if (name.isNil, { error("no file name"); ^nil });
		
		file = File(dir ++ name ++ ".scsyndef", "w");
		
		this.asArray.writeDef(file);
		
		//file.length.postln;
		file.close;
	}

}
