// you must not make any change at all to the order or number of
// instance variables in these classes!
// You should also not muck with the contents of the instance
// variables unless you are sure you know what you are doing.
// You may add methods.

Class {
	var <name, <nextclass, superclass, <subclasses;
	var <methods, <instVarNames, <classVarNames;
	var <iprototype, <cprototype;
	var <constNames, <constValues;
	var instanceFormat, instanceFlags;
	var <classIndex, classFlags, <maxSubclassIndex;
	var <filenameSymbol, <charPos, <classVarIndex;

	classvar <>classesInited;

	// Every class has a metaclass which has 'Meta_' prepended to the name.
	// Though there is a class Meta_Class which is the class of Class, the
	// class of Meta_Class is Class. It is a loop, but a different one
	// than the structure in Smalltalk.

	superclass {
		// superclass is stored as a symbol to allow forward reference during compilation
		^superclass.asClass
	}
	asClass { ^this }
	isMetaClass { ^this.class === Class }

	initClass {   }


	// call Class.initClassTree(SomeClass) to force a class to init if you depend on its resources
	*initClassTree { arg aClass;
		var implementsInitClass;
		// sometimes you need a class to be inited before another class
		// start the process: Class.initClassTree(Object)
		if(classesInited.isNil, { classesInited = IdentitySet.new });
		if(classesInited.includes(aClass).not, {
			classesInited.add(aClass);

			if(aClass.isMetaClass.not and: { aClass.class.findMethod(\initClass).notNil }, {
				aClass.initClass;
			});

			if(aClass.subclasses.notNil,{
				aClass.subclasses.do({ arg class; this.initClassTree(class); });
			});
		});
	}

	*allClasses {
		_AllClasses
		^this.primitiveFailed
	}
	findMethod { arg methodName;
		if ( methods.notNil, {
			^methods.detect({ arg method; method.name == methodName });
		},{ ^nil });
	}
	findRespondingMethodFor { arg methodName;
		this.superclassesDo { arg class;
			var method = class.findMethod(methodName);
			method !? { ^method };
		};
		^nil
	}
	findOverriddenMethod { arg methodName;
		if(this.findMethod(methodName).isNil) { ^nil };
		this.superclass.superclassesDo { arg class;
			var method = class.findMethod(methodName);
			if(method.notNil) { ^method }
		};
		^nil
	}
	respondingMethods {
		var all = Array.new, selectors = IdentitySet.new;
		this.superclassesDo { arg class;
			class.methods.do { arg item;
				var name = item.name;
				if(selectors.includes(name).not) {
					all = all.add(item);
					selectors.add(name)
				}
			}
		};
		^all
	}
	superclassesDo { arg function;
		var class = this;
		while { class.notNil } {
			function.value(class);
			class = class.superclass;
		}
	}

	dumpByteCodes { arg methodName;
		var meth;
		meth = this.findMethod(methodName);
		if (meth.notNil, { meth.dumpByteCodes },{ Post << methodName << " not found.\n"; });
	}

	dumpClassSubtree {
		_DumpClassSubtree
		^this.primitiveFailed
	}
	dumpInterface {
		// show all methods and their arguments defined for this class
		// does not include methods defined in superclasses
		this.methods.do({ arg meth;
			var numargs;
			numargs = meth.argNames.size - 1;
			"   ".post;
			meth.name.post;
			" ( ".post;
			meth.argNames.do({ arg name, i;
				if (i > 0, { // skip 'this'
					name.post;
					if (i < numargs, {
						", ".post;
					});
				});
			});
			" )\n".post;
		});
	}

	asString {
		^name.asString
	}
	printOn { arg stream;
		stream << "class " << name;
	}
	storeOn { arg stream;
		stream << name;
	}
	archiveAsCompileString { ^true }

	hasHelpFile {
		//should cache this in Library or classvar
		//can't add instance variables to Class
		^this.name.asString.findHelpFile.notNil
	}
	helpFilePath {
		^this.name.asString.findHelpFile
	}
	help {
		this.openHelpFile
	}
	openHelpFile {
		// NOTE: because wslib provided the shortcut "Object:*help --> Object:*openHelpFile", we do the same
		// rather than moving the implementation to the future-compatible :help method.
		// This prevents infinite recursions for people with wslib installed.
		// In future (3.7) this method content should be moved to :help, but no sooner.
		this.name.asString.help
	}

	shallowCopy { ^this }
	//listInstances { _ListInstancesOf }
	//traceAnyPathToAllInstancesOf { _TraceAnyPathToAllInstancesOf }

	openCodeFile {
		this.filenameSymbol.asString.openDocument(this.charPos, -1);
	}
	classVars {
		var start, end;
		start = this.classVarIndex;
		end = start + this.classVarNames.size;
		^thisProcess.instVarAt(0).copyRange(start, end)
	}
	inspectorClass { ^ClassInspector }
	findReferences { arg aSymbol, references;
		methods.do({ arg meth;
			references = meth.findReferences(aSymbol, references)
		});
		^references
	}
	*findAllReferences { arg aSymbol;
		// this will not find method calls that are compiled with special byte codes such as 'value'.
		var references;
		Class.allClasses.do({ arg class;
			references = class.findReferences(aSymbol, references);
		});
		^references;
	}
	findRespondingSubclasses { arg selector;
		var list = [];
		subclasses.do { arg class;
			if(class.instancesRespondTo(selector)) {
				list = list.add(class);
			};
			list = list.addAll(class.findRespondingSubclasses(selector))
		};
		^list
	}
	findRespondingUpperSubclasses { arg selector;
		var list = [];
		if(this.instancesRespondTo(selector)) { ^list.add(this) };
		subclasses.do { arg class;
			if(class.instancesRespondTo(selector)) {
				list = list.add(class);
			} {
				list = list.addAll(class.findRespondingUpperSubclasses(selector))
			}
		};
		^list
	}
	findSimilarSelectors { arg selector, maxEditDistance = 2, minSimilarity = 0.5, prioritizeCapitalization = true;
		var names = this.respondingMethods.collect(_.name);
		^selector.asString.findSimilarIn(names, maxEditDistance, minSimilarity, prioritizeCapitalization)
	}
	instancesRespondTo { arg selector;
		_InstancesOfClassRespondTo
		^this.primitiveFailed
	}
	allSubclasses {
		var list;
		list = subclasses.copy;
		subclasses.do({ arg class; list = list ++ class.allSubclasses; });
		^list
	}
	superclasses {
		var list;
		this.superclass.superclassesDo { arg class; list = list.add(class) }
		^list
	}

}

Process {
	// A Process is a runtime environment.
	var classVars, <interpreter;
	var curThread, <mainThread;
	var schedulerQueue;
	var <>nowExecutingPath;

	startup {
		var time;

		Class.initClassTree(AppClock); // AppClock first in case of error
		time = this.class.elapsedTime;
		Class.initClassTree(Object);
		Class.initClassTree(AbstractObjectExperimental);
		("Class tree inited in" + (this.class.elapsedTime - time).round(0.01) + "seconds").postln;
		Class.classesInited = nil;

		topEnvironment = Environment.new;
		currentEnvironment = topEnvironment;
		Archive.read;

		// This method is called automatically right after compiling.
		// Override in class 'Main' to do initialization stuff,
		// but make sure to call this superclass method.

		// the AppClock is not started until after this method is complete
	}
	run {
		// This method is called when 'Run Main' is chosen from the menu.
		// Override in class 'Main' to do whatever you want.
	}
	stop {
		// This method is called when 'Stop Main' is chosen from the menu.
		// Override in class 'Main' to do whatever you want.
	}
	shutdown {
		// This method is called before recompiling or quitting.
		// Override in class 'Main' to do whatever you want.
		ShutDown.run;
		NetAddr.disconnectAll;
		File.closeAll;
		Archive.write;
	}
	tick { // called repeatedly by SCVirtualMachine::doPeriodicTask
		^AppClock.tick;
	}

	*tailCallOptimize {
		_GetTailCallOptimize
		^this.primitiveFailed
	}
	*tailCallOptimize_ { arg bool;
		_SetTailCallOptimize
		^this.primitiveFailed
	}

	getCurrentSelection {
		var qt = \QtGUI.asClass;
		^if(qt.notNil and: {qt.focusView.notNil}) {
			qt.selectedText;
		} {
			interpreter.cmdLine;
		}
	}

	openCodeFile {
		var string, class, method, words;
		string = this.getCurrentSelection;
		if (string.includes($:), {
			string.removeAllSuchThat(_.isSpace);
			words = string.delimit({ arg c; c == $: });
			class = words.at(0).asSymbol.asClass;
			if (class.notNil, {
				method = class.findMethod(words.at(1).asSymbol);
				if (method.notNil, {
					method.filenameSymbol.asString.openDocument(method.charPos, -1);
				});
			});
		},{
			class = string.asSymbol.asClass;
			if (class.notNil, {
				class = class.classRedirect;
				class.filenameSymbol.asString.openDocument(class.charPos, -1);
			});
		});
	}

	openWinCodeFile {
		var string, class, method, words;
		string = this.getCurrentSelection;
		if (string.includes($:), {
			string.removeAllSuchThat(_.isSpace);
			words = string.delimit({ arg c; c == $: });
			class = words.at(0).asSymbol.asClass;
			if (class.notNil, {
				method = class.findMethod(words.at(1).asSymbol);
				if (method.notNil, {
					method.filenameSymbol.asString.openWinTextFile(method.charPos, -1);
				});
			});
		},{
			class = string.asSymbol.asClass;
			if (class.notNil, {
				class = class.classRedirect;
				class.filenameSymbol.asString.openWinTextFile(class.charPos, -1);
			});
		});
	}


	methodReferences {
		// this will not find method calls that are compiled with special byte codes such as 'value'.
		var name, out, references, nameString;
		out = CollStream.new;
		name = this.getCurrentSelection.asSymbol;
		references = Class.findAllReferences(name);
		if (references.notNil, {
			out << "References to '" << name << "' :\n";
			references.do({ arg ref;
				nameString = ref.ownerClass.name ++ ":" ++ ref.name;
				out << "   [" << nameString << "]\n"; });
			out.collection.newTextWindow(name.asString);
		},{
			Post << "\nNo references to '" << name << "'.\n";
		});
	}
	methodTemplates {
		// this constructs the method templates when cmd-Y is pressed in the Lang menu.
		var name, out, found = 0, namestring, text;
		out = CollStream.new;

		text = this.getCurrentSelection;

		if (text.isEmpty){
			Post << "\nNo implementations of ''.\n";
			^this
		};
		if (text[0].toLower != text[0]) {
			// user pressed the wrong key. DWIM.
			^this.openCodeFile;
		};
		name = text.asSymbol;
		out << "Implementations of '" << name << "' :\n";
		Class.allClasses.do({ arg class;
			class.methods.do({ arg method;
				if (method.name == name, {
					found = found + 1;
					namestring = class.name ++ ":" ++ name;
					out << "   [" << namestring << "] :     ";
					if (method.argNames.isNil or: { method.argNames.size == 1 }, {
						out << "this." << name;
						if (name.isSetter, { out << "(val)"; });
					},{
						out << method.argNames.at(0);
						if (name.asString.at(0).isAlpha, {
							out << "." << name << "(";
							method.argNames.do({ arg argName, i;
								if (i > 0, {
									if (i != 1, { out << ", " });
									out << argName;
								});
							});
							out << ")";
						},{
							out << " " << name << " ";
							out << method.argNames.at(1);
						});
					});
					out.nl;
				});
			});
		});
		case
		{ found == 0 }
		{
			Post << "\nNo implementations of '" << name << "'.\n";
		}
		{ found == 1 }
		{
			interpreter.cmdLine = namestring;
			this.openCodeFile;
		}
		{
			out.collection.newTextWindow(name.asString);
		};
	}

	interpretCmdLine {
		// interpret some text from the command line
		interpreter.interpretCmdLine;
	}

	interpretPrintCmdLine {
		// interpret some text from the command line and print result
		interpreter.interpretPrintCmdLine;
	}

	interpretPrintSelectedText {
		interpreter.cmdLine = this.getCurrentSelection;
		interpreter.interpretPrintCmdLine;
	}

	showHelp {
		this.getCurrentSelection.help
	}

	argv { ^[] }

	shallowCopy { ^this }

	*elapsedTime {
		_ElapsedTime
		^this.primitiveFailed
	}

	*monotonicClockTime {
		_monotonicClockTime
		^this.primitiveFailed
	}

	storeOn { arg stream;
		stream << "thisProcess";
	}
	archiveAsCompileString { ^true }

	prSchedulerQueue { ^schedulerQueue }
}

// A FunctionDef is defined by a code within curly braces {} (when it isn't inlined)
// When you use a FunctionDef in your code it gets pushed on the stack
// as an instance of Function (PyrClosure)
FunctionDef {
	var raw1, raw2;  // MethodRaw, stores lots of data with small sizes (chars and shorts).
	var <code; // Int8Array (bytes)
	var <selectors; // Array
	var <constants; // Array
	var <prototypeFrame; // Array of arg and var default values
	var <context; // where captured (closed over) variables live.
	var <argNames;  // SymbolArray
	var <varNames; // 
	var <isClosed; // Boolean, true if context is nil or Interpreter:functionCompiler, otherwise false
	var fileLocation; // either nil or an Array of: the line number, and the byte offset in line.
	var <sourceCodeFileOrSnippet; // String. Might be the whole file, or could just be a code snippet (text within a file)
	var <name; // Symbol. Method name, or if a function, an attempt to deduce the function name is made, example: (f = {}).def.name == 'f';
	var filePath; // Symbol. can be nil if file hasn't been saved.
	var <sourceCodeStartIndex, <sourceCodeEndIndex; // Integers. This are offsets into the sourceCodeFileOrSnippet.
	var <byteCodeLocations; // IntArray location of each byte code as offsets into sourceCode [start0, end0, start1, end1... startn, endn]
	var byteCodeSizes; // Int8Array size of each byte code in bytes. PyrInt8Array

	filenameSymbol { ^filePath }

	sourceCode {
		^sourceCodeFileOrSnippet[sourceCodeStartIndex..(sourceCodeEndIndex - 1)]
	}

	dumpByteCodes {
		_DumpByteCodes
		^this.primitiveFailed
	}

	numArgs {
		// return number of arguments to the function
		_FunDef_NumArgs
		^this.primitiveFailed
	}
	numVars {
		// return number of variables in the function
		_FunDef_NumVars
		^this.primitiveFailed
	}
	varArgs {
		// return boolean whether function has ellipsis argument
		_FunDef_VarArgs
		^this.primitiveFailed
	}
	varArgsValue {
		_FunDef_VarArgsValue
		^this.primitiveFailed
	}
	hasVarArgs {
		^this.varArgsValue > 0
	}
	hasKwArgs {
		^this.varArgsValue > 1
	}
	shallowCopy { ^this }

	asFunction {
		// this is only legal for closed functions.
		_FunctionDefAsFunction
		^this.primitiveFailed
	}

	dumpContexts {
		_FunctionDefDumpContexts
		^this.primitiveFailed
	}
	inspectorClass { ^FunctionDefInspector }

	findReferences { arg aSymbol, references;
		var lits;
		lits = selectors.asArray;
		if (lits.includes(aSymbol), {
			references = references.add(this);
		});
		lits.do({ arg item;
			if (item.isKindOf(FunctionDef), {
				references = item.findReferences(aSymbol, references)
			})
		});
		^references
	}
	storeOn { arg stream;
		stream << "nil"
	}
	checkCanArchive { "cannot archive FunctionDefs".warn }
	archiveAsCompileString { ^true }

	hasPartialApplication {
		^argNames.size > 0 and: { argNames[0] == \_ }
	}

	argumentString { arg withDefaultValues=true, withEllipsis=false, asArray=false;
		var res = "", pairs;
		var lastIndex, noVarArgs, varArgName;
		if(asArray) {
			withEllipsis = withDefaultValues = false;
		};
		if(this.hasPartialApplication) {
			^if(withEllipsis) { " ... args" } { "args" }
		};
		pairs = this.keyValuePairsFromArgs;
		if(pairs.isEmpty) { ^nil };
		if(this.varArgs) {
			varArgName = pairs.keep(-2).first;
			pairs = pairs.drop(-2);
		};
		lastIndex = pairs.lastIndex;
		pairs.pairsDo { |name, defaultValue, i|
			res = res ++ name;
			if(withDefaultValues and: { defaultValue.notNil }) {
				res = res ++ " = " ++ defaultValue.asCompileString;
			};
			if(i + 1 < lastIndex) { res = res ++ ", " };
		};
		^if(varArgName.notNil) {
			if(withEllipsis) {
				res ++ " ... " ++ varArgName
			} {
				if(asArray) {
					if(res == "") {
						varArgName
					} {
						"[%] ++ %".format(res, varArgName)
					}
				} {
					if(res == "") {
						res ++ varArgName
					} {
						res ++ ", " ++ varArgName
					}
				}
			}
		} {
			if(asArray) {
				"[%]".format(res)
			} {
				res
			}
		}
	}

	makeFuncModifierString { |modifier|
		// the modifier is a function that takes the string
		// which represents the array of all arguments
		var valueBlock, argBlock, i;
		if(this.argNames.isNil) { Error("a function without arguments needs no such string").throw };
		argBlock = this.argumentString(withDefaultValues: true, withEllipsis: true);
		valueBlock = this.argumentString(withDefaultValues: false, withEllipsis: false, asArray:true);
		if(modifier.notNil) { valueBlock = modifier.value(valueBlock) };
		^"{ arg %; % }".format(argBlock, valueBlock)
	}

	keyValuePairsFromArgs {
		var values;
		if(argNames.isNil) { ^[] };
		values = this.prototypeFrame.keep(argNames.size);
		^[argNames, values].flop.flatten
	}

	makeEnvirFromArgs {
		^().putPairs(this.keyValuePairsFromArgs)
	}

}

Method : FunctionDef {
	var <ownerClass, <primitiveName;

	charPos {
		// Because methods are always compiled with a whole file, we can access this directly.
		^sourceCodeStartIndex
	 }
	openCodeFile {
		this.filenameSymbol.asString.openDocument(this.charPos, -1);
	}
	hasHelpFile {
		//should cache this in Library or classvar
		//can't add instance variables to Class
		^this.name.asString.findHelpFile.notNil
	}
	help {
		HelpBrowser.openHelpForMethod(this);
	}
	inspectorClass { ^MethodInspector }
	storeOn { arg stream;
		stream << ownerClass.name << ".findMethod(" << name.asCompileString << ")"
	}
	archiveAsObject { ^true }
	checkCanArchive {}
	findReferences { arg aSymbol, references;
		var lits, functionRefs;
		lits = selectors.asArray;
		if (lits.includes(aSymbol), {
			references = references.add(this);
			^references // we only need to be listed once
		});
		lits.do({ arg item;
			if (item.isKindOf(FunctionDef), {
				functionRefs = item.findReferences(aSymbol, functionRefs);
			})
		});
		functionRefs.notNil.if({references = references.add(this)});
		^references
	}

	keyValuePairsFromArgs {
		var names, values;
		if(argNames.isNil, { ^[] });
		names = argNames.drop(1); // first argName is "this"
		values = this.prototypeFrame.drop(1).keep(names.size);
		^[names, values].flop.flatten
	}

}

Frame {
	// frames contain the local variables, context and continuation of a function or method invocation.
	// since some Frames are deleted instead of garbage collected, it is too
	// dangerous to allow access to them. Dangling pointers could result.
	shallowCopy { ^this }
	inspectorClass { ^FrameInspector }

	storeOn { arg stream; stream << "nil"; }
	archiveAsCompileString { ^true }
	checkCanArchive { "cannot archive Frames".warn }
}

DebugFrame {
	var <functionDef, <args, <vars, <caller, <context, <address, <ipIndex;
	// Object.getBackTrace returns one of these.
	// 'functionDef' is the FunctionDef for this function or method.
	// 'args' the values of the arguments to the function call.
	// 'vars' the values of the local variables.
	// 'caller' points to another DebugFrame for the caller to this function.
	// 'context' points to another DebugFrame for the frame lexically enclosing this one.
	// 'address' memory address of the actual frame object.
	asString { ^"DebugFrame of " ++ functionDef.asString }

	// Turns whole backtrace into string 
	backtracePrintOnto { |stream, prefix(""), callFrameAnnotations(#[]), oneBeforeBeginMethod, endMethod, maxVerboseFrames(3)|  
		var stack = {
			var f = this; 
			while { f.notNil } {
				f.yield;
				f = f.caller;
			};
		}.r.all;
		var newPrefix = prefix ++ thisPrefix;

		var begin = oneBeforeBeginMethod !? { this.prLastIndexOf(stack, oneBeforeBeginMethod) } ?? { -1 } + 1;
		var end = endMethod !? { this.prFirstIndexOf(stack, endMethod) } ?? { stack.size };
		var thisPrefix = "     ";
		var count = 0;

		var stackToPrint = stack[begin..(end - 1)];

		var dups=0;
		var prevDef;
		var stackSkipMask = stackToPrint.collect {|s|
			if (s.functionDef === prevDef) {
				dups = dups + 1
			} {
				dups = 0
			};
			prevDef = s.functionDef;
			dups < 3
		};

		stream << "\n";

		[stackToPrint, stackSkipMask].flop.reverseDo { |f, j|
			var d = f[0];
			var mask = f[1];
			stream << prefix << (stackToPrint.size - j).asString.padRight(4) << ": ";
			d.printOntoBacktrace(stream, newPrefix, callFrameAnnotations[stackToPrint.size - count - 1], (stackToPrint.size - j - 1) < maxVerboseFrames, mask);
			count = count + 1;
		};


		^stream;
	 }

	// Turns this frame into a formatted string
	printOntoBacktrace { |stream, prefix(""), annotation, printArgsAndVars, printSource| 
		stream << this.prAsErrorString(prefix, annotation, printArgsAndVars, printSource)
	}

	prAsErrorString {|prefix, annotation, printArgsAndVars, printSource| _DebugFrame_asErrorString }

	prLastIndexOf { |collection, predicate({})|
		var last = 0;
		collection.do{ 
			|v,i| 
			if (v.functionDef.isKindOf(Method) and: {predicate.(v.functionDef) ?? { false }}) { last = i } 
		};
		^last
	}
	prFirstIndexOf { |collection, predicate({})|
		var first = 0;
		collection.reverseDo{ 
			|v,i| 
			if (v.functionDef.isKindOf(Method) and: {predicate.(v.functionDef) ?? { false }}) { first = i } 
		};
		^(collection.size - first) - 1;
	}
}

RawPointer {
	// class used to hold raw pointers from the
	// host environment.
}

Interpreter {
	// The interpreter defines a context in which interactive commands
	// are compiled.


	// NOTE: all instance variables defined here (or in Object) will be accessible from every single function evaluated in the repl,
	// ... yes, even the members without a readwrite accessor, see method 'compile' for more information.

	// These 3 members are provided by the language client.
	// It reaches into the supercollider runtime and sets these.
	var cmdLine;
	var filePath;
	var lineNumber;
	var column;

	var context; // faked interpreter context frame. Don't mess with it.

	// a-z are predefined variables for use by the interactive context.
	// They are read+write so that programmatic methods can
	// get and alter the values that the interpreter has access to.
	var <>a, <>b, <>c, <>d, <>e, <>f, <>g, <>h, <>i, <>j;
	var <>k, <>l, <>m, <>n, <>o, <>p, <>q, <>r, <>s, <>t;
	var <>u, <>v, <>w, <>x, <>y, <>z;

	var <>codeDump, <>preProcessor;

	*new { ^this.shouldNotImplement(thisMethod) }

	interpretCmdLine {
		^this.prCompileUsingMembers.()
	}

	interpretPrintCmdLine {
		var res, func, code = cmdLine, doc, ideClass = \ScIDE.asClass;
		preProcessor !? { cmdLine = preProcessor.value(cmdLine, this) };
		func = this.prCompileUsingMembers;
		if (ideClass.notNil) {
			thisProcess.nowExecutingPath = ideClass.currentPath
		} {
			if(\Document.asClass.notNil and: {(doc = Document.current).tryPerform(\dataptr).notNil}) {
				thisProcess.nowExecutingPath = doc.tryPerform(\path);
			}
		};
		res = func.value;
		thisProcess.nowExecutingPath = nil;
		codeDump.value(code, res, func, this);
		("-> " ++ res).postln;
	}

	// compile, evaluate
	interpret { |string ... args, kwargs|
		kwargs = kwargs.asEvent;
		^this.compile(string, kwargs[\filePath], kwargs[\lineNumber], kwargs[\column]).valueArray(args);
	}
	// compile, evaluate, print
	interpretPrint { arg string ... args, kwargs;
		kwargs = kwargs.asEvent;
		^this.compile(string, kwargs[\filePath], kwargs[\lineNumber], kwargs[\column]).valueArray(args).postln;
	}

	// Compiles a string into Function object (closure).
	// This is done *as-if* it was a function written inside of the method Interpreter:functionCompileContext,
	// therefore, 'this' will return an instance of this class, and you can access all the instance variables.
	// This is also true for *closed* functions. All valid code:  #{ context }, #{ a }, #{cmdLine}...
	// This is also why you can access these straight from the repl... `context`, `cmdLine`, all valid!
	// Yes, even the 'private' instance variables can be accessed this way.
	// This also applied to everything inside object, e.g., `currentEnvironment` and `nl`.
	compile  { |string, filePath_, lineNumber_, column_|
		cmdLine = string;
		filePath = filePath_;
		lineNumber = lineNumber_;
		column = column_;
		^this.prCompileUsingMembers
	}

	prCompileUsingMembers {
		^this.prCompile(cmdLine, filePath, lineNumber, column)
	}

	prCompile { |string, filePath, lineNumber, column|
		_CompileExpression
		// TODO: this is rather bad, returning nil if the compilation fails means we can't tell if it failed because returning nil is valid.
		^nil
	}

	clearAll {
		a = b = c = d = e = f = g = h = i = j = k = l = m =
		n = o = p = q = r = s = t = u = v = w = x = y = z = nil;
	}

	executeFile { arg pathName ... args;
		var	result, saveExecutingPath = thisProcess.nowExecutingPath;
		if (File.exists(pathName).not) {
			"file \"%\" does not exist.\n".postf(pathName);
			^nil
		};
		thisProcess.nowExecutingPath = pathName;
		protect {
			result = this.compileFile(pathName).valueArray(args)
		} { |exception|
			exception !? { exception.path = pathName };
			thisProcess.nowExecutingPath = saveExecutingPath
		};
		^result
	}

	compileFile { arg pathName;
		var file, text;
		file = File.new(pathName, "r");
		if (file.isOpen.not, {
			error("file open failed\n");
			^nil
		});
		text = file.readAllString;
		file.close;
		preProcessor !? { text = preProcessor.value(text, this) };
		if (text.beginsWith("#!"), {
			// comment out shebang to preserve line count
			text.overWrite("//");
		});
		^this.compile(text, pathName.asSymbol, 0, 0);
	}

	shallowCopy { ^this }

	// PRIVATE
	// All repl invocations and 'closed' functions `#{...}`} are evaluated *as-if* they were written inside this method during class compilation.
	// Essentially, all 'top-level' non-class-library code takes place in here.
	// This is how we 'magically' have access to the variables 'a', 'b', ... everywhere `#{ a }`
	functionCompileContext { }

}
