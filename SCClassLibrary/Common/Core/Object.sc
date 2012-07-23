Object  {
	classvar <dependantsDictionary, currentEnvironment, topEnvironment, <uniqueMethods;

	const nl = "\n";

	*new { arg maxSize = 0;
		_BasicNew
		^this.primitiveFailed
		// creates a new instance that can hold up to maxSize
		// indexable slots. the indexed size will be zero.
		// to actually put things in the object you need to
		// add them.
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
	postcs { this.asCompileString.postln }
	totalFree { _TotalFree }
	largestFreeBlock { _LargestFreeBlock }
	gcDumpGrey { _GCDumpGrey }
	gcDumpSet { arg set; _GCDumpSet }
	gcInfo { _GCInfo }
	gcSanity { _GCSanity }
	canCallOS { _CanCallOS }


	//accessing
	size { ^0 }
	indexedSize { ^0 }
	flatSize { ^1	}

	do { arg function; function.value(this, 0) }
	generate { arg function, state; this.do(function); ^state }
	//reverseDo { arg function; function.value(this, 0) }

	// class membership
	class { _ObjectClass; ^this.primitiveFailed }
	isKindOf { arg aClass; _ObjectIsKindOf; ^this.primitiveFailed }
	isMemberOf { arg aClass; _ObjectIsMemberOf; ^this.primitiveFailed }
	respondsTo { arg aSymbol; _ObjectRespondsTo; ^this.primitiveFailed }

	performMsg { arg msg;
		_ObjectPerformMsg;
		^this.primitiveFailed
	}

	perform { arg selector ... args;
		_ObjectPerform;
		^this.primitiveFailed
	}
	performList { arg selector, arglist;
		_ObjectPerformList;
		^this.primitiveFailed
	}
	functionPerformList {
		// perform only if Function. see Function-functionPerformList
		^this
	}

	// super.perform(selector,arg) doesn't do what you might think.
	// \perform would be looked up in the superclass, not the selector you are interested in.
	// Hence these methods, which look up the selector in the superclass.
	// These methods must be called with this as the receiver.
	superPerform { arg selector ... args;
		_SuperPerform;
		^this.primitiveFailed
	}
	superPerformList { arg selector, arglist;
		_SuperPerformList;
		^this.primitiveFailed
	}

	tryPerform { arg selector ... args;
		^if(this.respondsTo(selector),{
			this.performList(selector,args)
		})
	}
	multiChannelPerform { arg selector ... args;
		^flop([this, selector] ++ args).collect { |item|
			performList(item[0], item[1], item[2..])
		}
	}

	performWithEnvir { |selector, envir|
		var argNames, args;
		var method = this.class.findRespondingMethodFor(selector);
		
		if(method.isNil) { ^this.doesNotUnderstand(selector) };
		
		argNames = method.argNames.drop(1);
		args = method.prototypeFrame.drop(1);
		argNames.do { |name, i|
			var val = envir[name];
			val !? { args[i] = val };
		};
		
		^this.performList(selector, args)
	}

	performKeyValuePairs { |selector, pairs|
		^this.performWithEnvir(selector, ().putPairs(pairs))
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
	dup { arg n = 2;
		^Array.fill(n, { this.copy });
	}
	! { arg n;
		^this.dup(n)
	}

	// evaluation
	poll { ^this.value }
	value { ^this }
	valueArray { ^this }
	valueEnvir { ^this }
	valueArrayEnvir { ^this }

	// equality, identity
	== { arg obj; ^this === obj }
	!= { arg obj; ^not(this == obj) }
	=== { arg obj; _Identical; ^this.primitiveFailed }
	!== { arg obj;_NotIdentical; ^this.primitiveFailed }
	equals { arg that, properties;
		^that.respondsTo(properties) and: {
			properties.every { |selector| this.perform(selector) == that.perform(selector) }
		}
	}
	compareObject { arg that, instVarNames;
		if(this === that,{ ^true });
		// possibly ok if one of us isKindOf the other
		if(this.class !== that.class,{ ^false });
		if(instVarNames.notNil,{
			instVarNames.do({ |varname|
				if(this.instVarAt(varname) != that.instVarAt(varname),{
					^false
				})
			});
		},{
			this.instVarSize.do({ arg i;
				if(this.instVarAt(i) != that.instVarAt(i),{ ^false });
			});
		});
		^true
	}
	instVarHash { arg instVarNames;
		var indices, res = this.class.hash;
		if(this.instVarSize == 0) {
			^res
		};
		indices = if(instVarNames.notNil) {
			instVarNames.collect(this.slotIndex(_))
		} {
			(0..this.instVarSize-1)
		};
		indices.do { |i|
			var obj = this.instVarAt(i);
			res = res << 1 bitXor: obj.hash;  // encode slot order by left shifting
		};
		^res
	}

	basicHash { _ObjectHash; ^this.primitiveFailed }
	hash { _ObjectHash; ^this.primitiveFailed }
	identityHash { _ObjectHash; ^this.primitiveFailed }

	// create an association
	-> { arg obj; ^Association.new(this, obj) }

	// stream
	next { ^this }
	reset { ^this }
	first { arg inval; this.reset; ^this.next(inval) }
	iter { ^OneShotStream(this) }
	stop { ^this }
	free { ^this }
	clear { ^this }
	removedFromScheduler { ^this }
	isPlaying { ^false }
	embedInStream { ^this.yield; }
	cyc { arg n = inf;
		^r {|inval|
			n.do {
				inval = this.embedInStream(inval);
				this.reset;
			}
		}
	}
	fin { arg n = 1;
		^r {|inval|
			var item;
			n.do {
				item = this.next(inval);
				if (item.isNil) { nil.alwaysYield };
				inval = item.yield
			}
		}
	}

	repeat { arg repeats = inf; ^Pn(this, repeats).asStream }
	loop { ^this.repeat(inf) }

	asStream { ^this }

	eventAt { ^nil }
	composeEvents { arg event; ^event.copy }

	finishEvent {}
	atLimit { ^false }

	isRest { ^false }

	threadPlayer {}
	threadPlayer_ {}

	// testing
	? { arg obj; ^this }
	?? { arg obj; ^this }
	!? { arg obj; ^obj.value(this) }

	isNil { ^false }
	notNil { ^true }
	isNumber { ^false }
	isInteger { ^false }
	isFloat { ^false }
	isSequenceableCollection { ^false }
	isCollection { ^false }
	isArray { ^false }
	isString { ^false }
	containsSeqColl { ^false }
	isValidUGenInput { ^false }
	isException { ^false }
	isFunction { ^false }

	matchItem {|item| ^this === item }
	trueAt { ^false }
	falseAt { arg key;
		^this.trueAt(key).not
	}

	pointsTo { arg obj; _ObjectPointsTo; ^this.primitiveFailed }
	mutable { _ObjectIsMutable; ^this.primitiveFailed }
	frozen { _ObjectIsPermanent; ^this.primitiveFailed }

	// errors
	halt {
		thisProcess.nowExecutingPath = nil;
		OnError.run;
		this.prHalt
	}
	prHalt { _Halt }
	primitiveFailed {
		PrimitiveFailedError(this).throw;
	}
	reportError {
		error(this.asString);
		this.dumpBackTrace;
	}

	subclassResponsibility { arg method;
		SubclassResponsibilityError(this, method, this.class).throw;
	}
	doesNotUnderstand { arg selector ... args;
		DoesNotUnderstandError(this, selector, args).throw;
	}
	shouldNotImplement { arg method;
		ShouldNotImplementError(this, method, this.class).throw;
	}
	outOfContextReturn { arg method, result;
		OutOfContextReturnError(this, method, result).throw;
	}
	immutableError { arg value;
		ImmutableError(this, value).throw;
	}

	deprecated { arg method, alternateMethod;
		DeprecatedError(this, method, alternateMethod, this.class).throw;
	}

	mustBeBoolean { MustBeBooleanError(nil, this).throw; }
	notYetImplemented { NotYetImplementedError(nil, this).throw; }

	dumpBackTrace { _DumpBackTrace }
	getBackTrace { _GetBackTrace }
	throw {
		if (Error.handling) {
			error("throw during error handling!\n");
			this.dump;
			^this
		};
		thisThread.handleError(this);
	}


	// conversion
	species { ^this.class }
	asCollection { ^[this] }
	asSymbol { ^this.asString.asSymbol }
	asString { arg limit = 512;
		var string;
		_ObjectString
		string = String.streamContentsLimit({ arg stream; this.printOn(stream); }, limit);
		if (string.size >= limit, { ^(string ++ "...etc..."); });
		^string
	}
	asCompileString {
		_ObjectCompileString
		^String.streamContents({ arg stream; this.storeOn(stream); });
	}

	cs { ^this.asCompileString }

	printClassNameOn { arg stream;
		var title;
		title = this.class.name.asString;
		stream << if((title @ 0).isVowel, { "an " }, { "a " }) << title;
	}
	printOn { arg stream;
		this.printClassNameOn(stream);
	}
	storeOn { arg stream;
		stream << this.class.name;
		this.storeParamsOn(stream);
		this.storeModifiersOn(stream);
	}
	storeParamsOn { arg stream;
		var args = this.storeArgs;
		if(args.notEmpty) {
			stream << "(" <<<* this.simplifyStoreArgs(args) << ")";
		} {
			stream << ".new"
		}
	}
	simplifyStoreArgs { arg args;
		var res = Array.new, newMethod, methodArgs;
		newMethod = this.class.class.findRespondingMethodFor(\new);
		methodArgs = newMethod.prototypeFrame.drop(1);
		args.size.reverseDo { |i|
			if(methodArgs[i] != args[i]) {
				^args.keep(i + 1)
			}
		}
		^[]
	}
	storeArgs { ^#[] }
	storeModifiersOn { arg stream;}


	as { arg aSimilarClass; ^aSimilarClass.newFrom(this) }
	dereference { ^this } // see Ref::dereference
	reference { ^Ref.new(this) }
	asRef { ^Ref.new(this) }
	// asArray { ^Array.with(this) }
	asArray { ^this.asCollection.asArray }
	asSequenceableCollection { ^this.asArray }

	// arrays
	rank { ^0 }
	deepCollect { arg depth, function, index = 0, rank = 0; ^function.value(this, index, rank) }
	deepDo { arg depth, function, index = 0, rank = 0; function.value(this, index, rank) }
	slice { ^this }
	shape { ^nil }
	unbubble { ^this }
	bubble { arg depth=0, levels=1;
		if (levels <= 1) { ^[this] };
		^[this.bubble(depth,levels-1)]
	}

	// compatibility with sequenceable collection

	obtain { arg index, default;  ^if(index == 0) { this } { default } }

	instill { arg index, item, default;
		^if(index == 0) { item } {
			this.asArray.instill(index, item, default)
		}
	}

	// FunctionList support
	addFunc { arg ... functions;
		^FunctionList([this] ++ functions)
	}
	removeFunc { arg function; if(this === function) { ^nil } }
	replaceFunc { arg find, replace; if(this === find) { ^replace } }
	addFuncTo { arg variableName ... functions;
		this.perform(variableName.asSetter, this.perform(variableName).addFunc(*functions))
	}
	removeFuncFrom { arg variableName, function;
		this.perform(variableName).removeFunc(function)
	}

	// looping
	while { arg body;
		// compiler magic: the compiler inlines the following loop
		// thus an uninlinable while can be implemented using while itself
		while({ this.value }, {
			body.value
		});
	}
	switch { arg ... cases;
		cases.pairsDo { | test, trueFunc |
			if (this == test.value) { ^trueFunc.value };
		};
		if (cases.size.odd) { ^cases.last.value };
		^nil
	}

	// coroutine support
	yield {
		_RoutineYield
		^this.primitiveFailed
	}
	alwaysYield {
		_RoutineAlwaysYield
		^this.primitiveFailed
	}
	yieldAndReset { arg reset = true;
		_RoutineYieldAndReset
		^this.primitiveFailed
	}
	idle { arg val;
		var time = thisThread.beats;
		while { thisThread.beats - time < val } { this.value.yield }
	}

	// dependancy support
	*initClass { dependantsDictionary = IdentityDictionary.new(4); }
	dependants {
		^dependantsDictionary.at(this) ?? { IdentitySet.new };
	}
	changed { arg what ... moreArgs;
		dependantsDictionary.at(this).copy.do({ arg item;
			item.update(this, what, *moreArgs);
		});
	}
	addDependant { arg dependant;
		var theDependants;
		theDependants = dependantsDictionary.at(this);
		if(theDependants.isNil,{
			theDependants = IdentitySet.new.add(dependant);
			dependantsDictionary.put(this, theDependants);
		},{
			theDependants.add(dependant);
		});
	}
	removeDependant { arg dependant;
		var theDependants;
		theDependants = dependantsDictionary.at(this);
		if (theDependants.notNil, {
			theDependants.remove(dependant);
			if (theDependants.size == 0, {
				dependantsDictionary.removeAt(this);
			});
		});
	}
	release {
		this.releaseDependants;
	}
	releaseDependants {
		dependantsDictionary.removeAt(this);
	}
	update { arg theChanged, theChanger;	// respond to a change in a model
	}


	// instance specific method support
	addUniqueMethod { arg selector, function;
		var methodDict;
		if(function.isKindOf(Function).not) {
			Error("A method must be defined using a function").throw
		};
		if(uniqueMethods.isNil, { uniqueMethods = IdentityDictionary.new });
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

	asInt { ^this.asInteger }

	blend { arg that, blendFrac = 0.5;
		// blendFrac should be from zero to one
		^this + (blendFrac * (that - this));
	}

	blendAt { arg index, method='clipAt';
		var iMin = index.roundUp.asInteger - 1;
		^blend(this.perform(method, iMin), this.perform(method, iMin+1), absdif(index, iMin));
	}

	blendPut { arg index, val, method='wrapPut';
		var iMin = index.floor.asInteger;
		var ratio = absdif(index, iMin);
		this.perform(method, iMin, val * (1-ratio));
		this.perform(method, iMin + 1, val * ratio);
	}


	fuzzyEqual { arg that, precision=1.0; ^max(0.0, 1.0 - (abs(this - that)/precision)) }

	isUGen { ^false }
	numChannels { ^1 }

	pair { arg that; ^[this, that] }
	pairs { arg that;
		var list;
		list = [];
		this.asArray.do {|a|
			that.asArray.do {|b|
				list = list.add(a.asArray ++ b)
			};
		};
		^list;
	}


	// scheduling
	awake { arg beats, seconds, clock;
		var time;
		time = seconds; // prevent optimization
		^this.next(beats)
	}
	beats_ {  } // for PauseStream
	clock_ {  } // for Clock

	// catch binary operators failure
	performBinaryOpOnSomething { arg aSelector, thing, adverb;
		if (aSelector === '==', {
			^false
		},{
		if (aSelector === '!=', {
			^true
		},{
			BinaryOpFailureError(this, aSelector, [thing, adverb]).throw;
		})});
	}
	performBinaryOpOnSimpleNumber { arg aSelector, thing, adverb;
		^this.performBinaryOpOnSomething(aSelector, thing, adverb)
	}
	performBinaryOpOnSignal { arg aSelector, thing, adverb;
		^this.performBinaryOpOnSomething(aSelector, thing, adverb)
	}
	performBinaryOpOnComplex { arg aSelector, thing, adverb;
		^this.performBinaryOpOnSomething(aSelector, thing, adverb)
	}
	performBinaryOpOnSeqColl { arg aSelector, thing, adverb;
		^this.performBinaryOpOnSomething(aSelector, thing, adverb)
	}
	performBinaryOpOnUGen { arg aSelector, thing, adverb;
		^this.performBinaryOpOnSomething(aSelector, thing, adverb)
	}

	writeDefFile { arg name, dir, overwrite = (true);

		StartUp.defer { // make sure the synth defs are written to the right path
			var file;
			dir = dir ? SynthDef.synthDefDir;
			if (name.isNil) { Error("missing SynthDef file name").throw } {
				name = dir +/+ name ++ ".scsyndef";
				if(overwrite or: { pathMatch(name).isEmpty })
					{
					file = File(name, "w");
					protect {
						AbstractMDPlugin.clearMetadata(name);
						this.asArray.writeDef(file);
					}{
						file.close;
					}
				}
			}
		}

	}

	isInputUGen { ^false }
	isOutputUGen { ^false }
	isControlUGen { ^false }
	source { ^this }
	asUGenInput { ^this }
	asControlInput { ^this }
	asAudioRateInput { ^if(this.rate != \audio) { K2A.ar(this) } { this } }


	// these are the same as new and newCopyArgs, but should not be overridden by any class.
	*prNew { arg maxSize = 0;
		_BasicNew
		^this.primitiveFailed
		// creates a new instance that can hold up to maxSize
		// indexable slots. the indexed size will be zero.
		// to actually put things in the object you need to
		// add them.
	}
	*prNewCopyArgs { arg ... args;
		_BasicNewCopyArgsToInstVars
		^this.primitiveFailed
		// creates a new instance which holds the args as slots
	}

	//////
	// these are dangerous operations as they break encapsulation and
	// can allow access to slots that should not be accessed because they are private to the
	// virtual machine, such as Frame objects.
	// Use with caution.
	// see counterparts to these in ArrayedCollection
	slotSize {
		^this.instVarSize;
	}
	slotAt { arg index;
		// index can be an integer or symbol.
		^this.instVarAt(index);
	}
	slotPut { arg index, value;
		// index can be an integer or symbol.
		^this.instVarPut(index, value);
	}
	slotKey { arg index;
		// index must be an integer.
		^this.class.instVarNames.at(index)
	}
	slotIndex { arg key;
		// key must be a symbol.
		^this.class.instVarNames.indexOf(key)
	}
	slotsDo { arg function;
		this.slotSize.do {|i|
			function.value(this.slotKey(i), this.slotAt(i), i);
		};
	}
	slotValuesDo { arg function;
		this.slotSize.do {|i|
			function.value(this.slotAt(i), i);
		};
	}

	// getSlots and setSlots will be used for a new implementation of asCompileString.
	// getSlots stores the keys and values so that if the instance
	// variable order changes, setSlots they will still set the right one.
	getSlots {
		var array;
		array = Array.new(this.slotSize * 2);
		this.slotSize.do {|i|
			array.add(this.slotKey(i));
			array.add(this.slotAt(i));
		};
		^array;
	}
	setSlots { arg array;
		array.pairsDo {|key, value|
			this.slotPut(key, value);
		}
	}

	instVarSize { _InstVarSize; ^this.primitiveFailed }
	instVarAt { arg index;
		// index can be an integer or symbol.
		_InstVarAt;
		^this.primitiveFailed;
	}
	instVarPut { arg index, item;
		// index can be an integer or symbol.
		_InstVarPut;
		^this.primitiveFailed;
	}

	//////////// ARCHIVING ////////////

	writeArchive { arg pathname;
		^this.writeTextArchive(pathname)
	}
	*readArchive { arg pathname;
		^this.readTextArchive(pathname)
	}
	asArchive {
		^this.asTextArchive;
	}

	initFromArchive {}

	archiveAsCompileString { ^false }
	archiveAsObject { ^this.archiveAsCompileString.not }
	checkCanArchive {}

	// archiving
	writeTextArchive { arg pathname;
		var text = this.asTextArchive;
		var file = File(pathname, "w");
		if(file.isOpen) {
			protect {
				file.write(text);
			} { file.close };
		} {
			MethodError("Could not open file % for writing".format(pathname.asCompileString), this).throw;
		}
	}
	*readTextArchive { arg pathname;
		^pathname.load
	}
	asTextArchive {
		var objects, list, stream, firsttime = true;

		if (this.archiveAsCompileString) {
			this.checkCanArchive;
			^this.asCompileString ++ "\n"
		};

		objects = IdentityDictionary.new;

		this.getContainedObjects(objects);

		stream = CollStream.new;
		stream << "var o, p;\n";

		list = List.newClear(objects.size);
		objects.keysValuesDo {|obj, index| list[index] = obj };

		stream << "o = [";
		list.do {|obj, i|
			var size;
			if (i != 0) { stream << ",  "; };
			if ((i & 3) == 0) { stream << "\n\t" };
			obj.checkCanArchive;
			if (obj.archiveAsCompileString) {
				stream << obj.asCompileString;
			}{
				size = obj.indexedSize;
				stream << obj.class.name << ".prNew";
				if (size > 0) {
					stream << "(" << size << ")"
				};
			};
		};
		stream << "\n];\np = [";
		// put in slots
		firsttime = true;
		list.do {|obj, i|
			var slots;
			if (obj.archiveAsCompileString.not) {
				slots = obj.getSlots;
				if (slots.size > 0) {
					if (firsttime.not) { stream << ",  "; };
					firsttime = false;
					stream << "\n\t// " << obj.class.name;
					stream << "\n\t";
					stream << i << ", [ ";
					if (obj.isKindOf(ArrayedCollection)) {
						slots.do {|slot, j|
							var index;
							if (j != 0) { stream << ",  "; };
							if ((j != 0) && ((j & 3) == 0)) { stream << "\n\t\t" };
							index = objects[slot];
							if (index.isNil) {
								stream << slot.asCompileString;
							}{
								stream << "o[" << index << "]";
							};
						};
					}{
						slots.pairsDo {|key, slot, j|
							var index;
							if (j != 0) { stream << ",  "; };
							if ((j != 0) && ((j & 3) == 0)) { stream << "\n\t\t" };
							stream << key << ": ";
							index = objects[slot];
							if (index.isNil) {
								stream << slot.asCompileString;
							}{
								stream << "o[" << index << "]";
							};
						};
					};
					stream << " ]";
				};
			};
		};
		stream << "\n];\n";

		stream << "prUnarchive(o,p);\n";
		^stream.contents
	}
	getContainedObjects { arg objects;
		if (objects[this].notNil) {^this};
		objects[this] = objects.size;

		if (this.archiveAsCompileString.not) {
			this.slotsDo {|key, slot|
				if (slot.archiveAsObject) {
					slot.getContainedObjects(objects);
				};
			};
		};

	}
	// old binary archiving
	// this will break if the instance vars change !
	// not recommended
	writeBinaryArchive { arg pathname;
		_WriteArchive
		^this.primitiveFailed;
	}
	*readBinaryArchive { arg pathname;
		_ReadArchive
		^this.primitiveFailed;
	}
	asBinaryArchive {
		_AsArchive
		^this.primitiveFailed;
	}
	// support for Gen
	genNext { ^nil }
	genCurrent { ^this }

	// support for ViewRedirect
	*classRedirect { ^this }

	help {
		this.class.asString.help
	}
}
