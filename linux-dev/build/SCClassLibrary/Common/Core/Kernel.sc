// you must not make any change at all to the order or number of 
// instance variables in these classes! 
// You should also not muck with the contents of the instance 
// variables unless you are sure you know what you are doing.
// You may add methods.

Class {
	var <name, <nextclass, superclass, <subclasses;
	var <methods, <instVarNames, <classVarNames;
	var <iprototype, <cprototype;
	var instanceFormat, instanceFlags;
	var <classIndex, classFlags, <maxSubclassIndex;
	var <filenameSymbol, <charPos;
	
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
		// sometimes you need a class to be inited before another class
		// start the process: Class.initClassTree(Object)
		if((classesInited ?? {classesInited = IdentitySet.new}).includes(aClass).not,{
			aClass.initClass;
			classesInited.add(aClass);			
			if(aClass.subclasses.notNil,{
				aClass.subclasses.do({ arg class; this.initClassTree(class); });
			});
		});
	}

	*allClasses { _AllClasses }
	
	findMethod { arg methodName;
		if ( methods.notNil, {
			^methods.detect({ arg method; method.name == methodName });
		},{ ^nil });
	}
	findRespondingMethodFor { arg methodName;
		var class, method;
		class = this;
		while ({class.notNil && (class != Class)}, {
			method = class.findMethod(methodName);
			if (method.notNil, { ^method });
			class = class.superclass;
		});
		^nil
	}
	dumpByteCodes { arg methodName;
		var meth;
		meth = this.findMethod(methodName);
		if (meth.notNil, { meth.dumpByteCodes },{ Post << methodName << " not found.\n"; });
	}
	
	dumpClassSubtree { _DumpClassSubtree }
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
	
	hasHelpFile { 
		//should cache this in Library or classvar
		//can't add instance variables to Class
		^this.name.asString.findHelpFile.notNil
	}
	helpFilePath {
		^this.name.asString.findHelpFile
	}
	openHelpFile { 
		(this.helpFilePath ? "Help/Help.help.rtf").openTextFile
	}
	
	shallowCopy { ^this }
	//listInstances { _ListInstancesOf }
	//traceAnyPathToAllInstancesOf { _TraceAnyPathToAllInstancesOf }
	
	openCodeFile {
		this.filenameSymbol.asString.openTextFile(this.charPos, -1);
	}
	classVars {
		^thisProcess.instVarAt(0).at(this.classIndex)
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
	allSubclasses {
		var list;
		list = subclasses.copy;
		subclasses.do({ arg class; list = list ++ class.allSubclasses; });
		^list
	}
	superclasses {
		var list, class;
		class = this;
		while ({ class = class.superclass; class.notNil },{ 
			list = list.add(class) 
		});
		^list
	}
		
}

Process {
	// A Process is a runtime environment.
	var classVars, <interpreter;
	var curThread, mainThread;
	var <processID; // 0 = application, 1 = audio engine
	var schedulerQueue;
	
	startUp {
		Class.initClassTree(AppClock); // AppClock first in case of error
		Class.initClassTree(Object);
		"Class tree inited".inform;
		Class.classesInited = nil;
		
		topEnvironment = Environment.new;
		currentEnvironment = topEnvironment;

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
	shutDown {
		// This method is called before recompiling or quitting.
		// Override in class 'Main' to do whatever you want.
		SCWindow.closeAll;
		//OSCPort.closeAll;
		File.closeAll;
	}
	tick { // called repeatedly by SCVirtualMachine::doPeriodicTask
		AppClock.tick;
	}
	openCodeFile {
		var string, class, method, words;
		string = interpreter.cmdLine;
		if (string.includes($-), {
			words = string.delimit({ arg c; c == $- });
			class = words.at(0).asSymbol.asClass;
			if (class.notNil, {
				method = class.findMethod(words.at(1).asSymbol);
				if (method.notNil, {
					method.filenameSymbol.asString.openTextFile(method.charPos, -1);
				});
			});
		},{
			class = string.asSymbol.asClass;
			if (class.notNil, {
				class.filenameSymbol.asString.openTextFile(class.charPos, -1);
			});
		});
	}
	methodReferences {
		// this will not find method calls that are compiled with special byte codes such as 'value'.
		var name, out, references;
		out = CollStream.new;
		name = interpreter.cmdLine.asSymbol;
		references = Class.findAllReferences(name);
		if (references.notNil, {
			out << "References to '" << name << "' :\n";
			references.do({ arg ref; out << "   " << ref.asString << "\n"; });
			out.collection.newTextWindow(name.asString);
		},{
			Post << "\nNo references to '" << name << "'.\n";
		});
	}
	methodTemplates {
		// this constructs the method templates when cmd-Y is pressed in the Lang menu.
		var name, out, found = false;
		out = CollStream.new;
		name = interpreter.cmdLine.asSymbol;
		out << "Implementations of '" << name << "' :\n";
		Class.allClasses.do({ arg class;
			class.methods.do({ arg method;
				if (method.name == name, {
					found = true;
					out << "   " << class.name << "-" << name << " :     ";
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
		if (found, {
			out.collection.newTextWindow(name.asString);
			//out.collection.post;
		},{
			Post << "\nNo implementations of '" << name << "'.\n";
		});
	}
	
	interpretCmdLine {
		// interpret some text from the command line
		interpreter.interpretCmdLine;
	}

	interpretPrintCmdLine {
		// interpret some text from the command line and print result
		interpreter.interpretPrintCmdLine;
	}

	showHelp {
		interpreter.cmdLine.openHelpFile
	}	

	shallowCopy { ^this }
	
	*elapsedTime { _ElapsedTime }
}


FunctionDef {
	var raw1, raw2, <code, <literals, <prototypeFrame, <context, <argNames, <varNames;
	
	// a FunctionDef is defined by a code within curly braces {}
	// When you use a FunctionDef in your code it gets pushed on the stack
	// as an instance of Function
	
	dumpByteCodes { _DumpByteCodes }
	
	numArgs { _FunDef_NumArgs }		// return number of arguments to the function
	numVars { _FunDef_NumVars }		// return number of arguments to the function
	varArgs { _FunDef_VarArgs }		// return boolean whether function has ellipsis argument

	shallowCopy { ^this }
	
	asFunction { 
		// this is only legal for closed functions.
		_FunctionDefAsFunction 
		^this.primitiveFailed 
	}
	
	dumpContexts {
		_FunctionDefDumpContexts
	}
	inspectorClass { ^FunctionDefInspector }
	
	findReferences { arg aSymbol, references; 
		var lits;
		lits = literals.asArray;
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
	
}

Method : FunctionDef {
	var <ownerClass, <name, <primitiveName;
	var <filenameSymbol, <charPos;

	openCodeFile {
		this.filenameSymbol.asString.openTextFile(this.charPos, -1);
	}
	hasHelpFile { 
		//should cache this in Library or classvar
		//can't add instance variables to Class
		^this.name.asString.findHelpFile.notNil
	}
	openHelpFile {
		this.name.asString.openHelpFile
	}
	inspectorClass { ^MethodInspector }
}

Frame {
	// since Frames are often created on the stack, it is too 
	// dangerous to allow access to them. Dangling pointers
	// could result.
	shallowCopy { ^this }
	inspectorClass { ^FrameInspector }
}

DebugFrame {
	var <functionDef, <args, <vars, <caller, <context, <address;
	// Object.getBackTrace returns one of these.
	// 'functionDef' is the FunctionDef for this function or method.
	// 'args' the values of the arguments to the function call.
	// 'vars' the values of the local variables.
	// 'caller' points to another DebugFrame for the caller to this function.
	// 'context' points to another DebugFrame for the frame lexically enclosing this one.
	// 'address' memory address of the actual frame object.
}

RawPointer {
	// class used to hold raw pointers from the 
	// host environment. 
	// no instance variables, no methods
}

Interpreter {	
	// The interpreter defines a context in which interactive commands
	// are compiled.
	
	var <cmdLine; // place holder for text executed from a worksheet
	var context; // faked interpreter context frame. Don't mess with it.
	
	// a-z are predefined variables for use by the interactive context.
	// They are read+write so that programmatic methods can
	// get and alter the values that the interpreter has access to.
	var <>a, <>b, <>c, <>d, <>e, <>f, <>g, <>h, <>i, <>j;
	var <>k, <>l, <>m, <>n, <>o, <>p, <>q, <>r, <>s, <>t;
	var <>u, <>v, <>w, <>x, <>y, <>z;
	
	*new { ^this.shouldNotImplement(thisMethod) }
	*newClear { ^this.shouldNotImplement(thisMethod) }
	
	//interpretCmdLine { ^this.interpret(cmdLine) }
	//interpretPrintCmdLine { ^this.interpretPrint(cmdLine) }
	
	interpretCmdLine {
		^this.compile(cmdLine).value;
	}
	interpretPrintCmdLine {
         //if (cmdLine.last != $\n , { "\n".post; });
		"\n".post;
		^this.compile(cmdLine).value.postln;
	}
	
	interpret { arg string;
		// compile, evaluate
		cmdLine = string;
		^this.compile(string).value;
	}
	interpretPrint { arg string;
		// compile, evaluate, print
		cmdLine = string;
		^this.compile(string).value.postln;
	}
	compile { arg string;
		_CompileExpression
		// compiles string and returns a Function.
		// the string must be a valid expression.
		// You cannot compile a class definition this way.
		// This method is not implemented in SCPlay.
		^nil
	}
	
	clearAll {
		a = b = c = d = e = f = g = h = i = j = k = l = m = 
		n = o = p = q = r = s = t = u = v = w = x = y = z = nil;
	}
	
	executeFile { arg pathName;
		^this.compileFile(pathName).value;
	}
	
	compileFile { arg pathName;
		var file, text;
		file = File.new(pathName, "r");
		if (file.isNil, { 
			error("file open failed\n");
			^nil 
		});
		text = file.readAllString;
		file.close;
		^this.compile(text)
	}
		
	// PRIVATE
	functionCompileContext { 
		// compiler uses this method as a fake context in which to compile 
		// the user's function.
		// Do not edit this method!
		
		{}	// this forces the compiler to generate a heap allocated frame rather than 
			// a frame on the stack
	}
	shallowCopy { ^this }
}

