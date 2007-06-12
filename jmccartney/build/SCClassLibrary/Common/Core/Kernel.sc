{\rtf1\mac\ansicpg10000\cocoartf100
{\fonttbl\f0\fnil\fcharset77 Monaco;}
{\colortbl;\red255\green255\blue255;\red191\green0\blue0;\red0\green0\blue191;\red96\green96\blue96;
}
\pard\tx560\tx1120\tx1680\tx2240\tx2800\tx3360\tx3920\tx4480\tx5040\tx5600\tx6160\tx6720\ql\qnatural

\f0\fs18 \cf2 // you must not make any change at all to the order or number of \
// instance variables in these classes! \
// You should also not muck with the contents of the instance \
// variables unless you are sure you know what you are doing.\
// You may add methods.\
\cf0 \
\cf3 Class\cf0  \{\
	\cf3 var\cf0  <name, <nextclass, superclass, <subclasses;\
	\cf3 var\cf0  <methods, <instVarNames, <classVarNames;\
	\cf3 var\cf0  <iprototype, <cprototype;\
	\cf3 var\cf0  instanceFormat, instanceFlags;\
	\cf3 var\cf0  <classIndex, classFlags, <maxSubclassIndex;\
	\cf3 var\cf0  <filenameSymbol, <charPos;\
\
	\cf2 // Every class has a metaclass which has 'Meta_' prepended to the name.\
\cf0 	\cf2 // Though there is a class Meta_Class which is the class of Class, the\
\cf0 	\cf2 // class of Meta_Class is Class. It is a loop, but a different one \
\cf0 	\cf2 // than the structure in Smalltalk.\
\cf0 	\
	superclass \{ \
		\cf2 // superclass is stored as a symbol to allow forward reference during compilation\
\cf0 		^superclass.asClass\
	\}\
	asClass \{ ^\cf3 this\cf0  \}\
	initClass \{\}\
	initClassTree \{\
		\cf3 this\cf0 .initClass;\
		subclasses.notNil.if(\{\
			subclasses.do(\{ \cf3 arg\cf0  class; class.initClassTree; \});\
		\})\
	\}\
\
	*allClasses \{ \cf3 _AllClasses\cf0  \}\
	\
	findMethod \{ \cf3 arg\cf0  methodName;\
		if ( methods.notNil, \{\
			^methods.detect(\{ \cf3 arg\cf0  method; method.name == methodName \});\
		\},\{ ^\cf3 nil\cf0  \});\
	\}\
	findRespondingMethodFor \{ \cf3 arg\cf0  methodName;\
		\cf3 var\cf0  class, method;\
		class = \cf3 this\cf0 ;\
		while (class != \cf3 Class\cf0 , \{\
			method = class.findMethod(methodName);\
			if (method.notNil, \{ ^method \});\
			class = class.superclass;\
		\});\
		^\cf3 nil\cf0 \
	\}\
	dumpByteCodes \{ \cf3 arg\cf0  methodName;\
		\cf3 var\cf0  meth;\
		meth = \cf3 this\cf0 .findMethod(methodName);\
		if (meth.notNil, \{ meth.dumpByteCodes \},\{ \cf3 Post\cf0  << methodName << \cf4 " not found.\\n"\cf0 ; \});\
	\}\
	\
	dumpClassSubtree \{ \cf3 _DumpClassSubtree\cf0  \}\
	dumpInterface \{\
		\cf2 // show all methods and their arguments defined for this class\
\cf0 		\cf2 // does not include methods defined in superclasses\
\cf0 		\cf3 this\cf0 .methods.do(\{ \cf3 arg\cf0  meth;\
			\cf3 var\cf0  numargs;\
			numargs = meth.argNames.size - 1;\
			\cf4 "   "\cf0 .post;\
			meth.name.post;\
			\cf4 " ( "\cf0 .post;\
			meth.argNames.do(\{ \cf3 arg\cf0  name, i;\
				if (i > 0, \{ \cf2 // skip 'this'\
\cf0 					name.post;\
					if (i < numargs, \{\
						\cf4 ", "\cf0 .post;\
					\});\
				\});\
			\});\
			\cf4 " )\\n"\cf0 .post;\
		\});\
	\}\
	\
	asString \{\
		^name.asString\
	\}\
	printOn \{ \cf3 arg\cf0  stream;\
		stream << \cf4 "class "\cf0  << name;\
	\}\
	storeOn \{ \cf3 arg\cf0  stream;\
		stream << name;\
	\}\
	\
	hasHelpFile \{ \cf3 _ClassHasHelpFile\cf0  \}\
	openHelpFile \{ \cf3 _ClassOpenHelpFile\cf0  \}\
	\
	shallowCopy \{ ^\cf3 this\cf0  \}\
	listInstances \{ \cf3 _ListInstancesOf\cf0  \}\
	traceAnyPathToAllInstancesOf \{ \cf3 _TraceAnyPathToAllInstancesOf\cf0  \}\
	\
	openCodeFile \{\
		\cf3 this\cf0 .filenameSymbol.asString.openTextFile(\cf3 this\cf0 .charPos, -1);\
	\}\
	classVars \{\
		^\cf3 thisProcess\cf0 .instVarAt(0).at(\cf3 this\cf0 .classIndex)\
	\}\
	inspectorClass \{ ^\cf3 ClassInspector\cf0  \}\
	findReferences \{ \cf3 arg\cf0  aSymbol, references; \
		methods.do(\{ \cf3 arg\cf0  meth; \
			references = meth.findReferences(aSymbol, references)\
		\});\
		^references\
	\}\
	*findAllReferences \{ \cf3 arg\cf0  aSymbol; \
		\cf2 // this will not find method calls that are compiled with special byte codes such as 'value'.\
\cf0 		\cf3 var\cf0  references;\
		\cf3 Class\cf0 .allClasses.do(\{ \cf3 arg\cf0  class;\
			references = class.findReferences(aSymbol, references);\
		\});\
		^references;\
	\}\
	allSubclasses \{\
		var list;\
		list = subclasses.copy;\
		subclasses.do(\{ arg class; list = list ++ class.allSubclasses; \});\
		^list\
	\}\
\}\
\
\cf3 Process\cf0  \{\
	\cf2 // A Process is a runtime environment.\
\cf0 	\cf3 var\cf0  classVars, <interpreter;\
	\cf3 var\cf0  curThread, mainThread;\
	\cf3 var\cf0  <processID; \cf2 // 0 = application, 1 = audio engine\
\cf0 	\cf3 var\cf0  schedulerQueue;\
	\
	startUp \{\
		\cf3 Object\cf0 .initClassTree;\
		topEnvironment = \cf3 Environment\cf0 .new;\
		currentEnvironment = topEnvironment;\
\
		\cf2 // This method is called automatically right after compiling.\
\cf0 		\cf2 // Override in class 'Main' to do initialization stuff,\
\cf0 		\cf2 // but make sure to call this superclass method.\
\cf0 	\}\
	run \{\
		\cf2 // This method is called when 'Run Main' is chosen from the menu.\
\cf0 		\cf2 // Override in class 'Main' to do whatever you want.\
\cf0 	\}\
	stop \{\
		\cf2 // This method is called when 'Stop Main' is chosen from the menu.\
\cf0 		\cf2 // Override in class 'Main' to do whatever you want.\
\cf0 	\}\
	shutDown \{\
		\cf2 // This method is called before recompiling or quitting.\
\cf0 		\cf2 // Override in class 'Main' to do whatever you want.\
\cf0 		\cf3 SCWindow\cf0 .closeAll;\
		\cf3 OSCPort\cf0 .closeAll;\
		\cf3 File\cf0 .closeAll;\
	\}\
	tick \{\
		\cf3 AppClock\cf0 .tick;\
	\}\
	openCodeFile \{\
		\cf3 var\cf0  string, class, method, words;\
		string = interpreter.cmdLine;\
		if (string.includes($-), \{\
			words = string.delimit(\{ \cf3 arg\cf0  c; c == $- \});\
			class = words.at(0).asSymbol.asClass;\
			if (class.notNil, \{\
				method = class.findMethod(words.at(1).asSymbol);\
				if (method.notNil, \{\
					class.filenameSymbol.asString.openTextFile(method.charPos, -1);\
				\});\
			\});\
		\},\{\
			class = string.asSymbol.asClass;\
			if (class.notNil, \{\
				class.filenameSymbol.asString.openTextFile(class.charPos, -1);\
			\});\
		\});\
	\}\
	methodReferences \{\
		\cf2 // this will not find method calls that are compiled with special byte codes such as 'value'.\
\cf0 		\cf3 var\cf0  name, out, references;\
		out = \cf3 CollStream\cf0 .new;\
		name = interpreter.cmdLine.asSymbol;\
		references = \cf3 Class\cf0 .findAllReferences(name);\
		if (references.notNil, \{\
			out << \cf4 "References to '"\cf0  << name << \cf4 "' :\\n"\cf0 ;\
			references.do(\{ \cf3 arg\cf0  ref; out << \cf4 "   "\cf0  << ref.asString << \cf4 "\\n"\cf0 ; \});\
			out.collection.newTextWindow(name.asString);\
		\},\{\
			\cf3 Post\cf0  << \cf4 "\\nNo references to '"\cf0  << name << \cf4 "'.\\n"\cf0 ;\
		\});\
	\}\
	methodTemplates \{\
		\cf2 // this constructs the method templates when cmd-Y is pressed in the Lang menu.\
\cf0 		\cf3 var\cf0  name, out, found = \cf3 false\cf0 ;\
		out = \cf3 CollStream\cf0 .new;\
		name = interpreter.cmdLine.asSymbol;\
		out << \cf4 "Implementations of '"\cf0  << name << \cf4 "' :\\n"\cf0 ;\
		\cf3 Class\cf0 .allClasses.do(\{ \cf3 arg\cf0  class;\
			class.methods.do(\{ \cf3 arg\cf0  method;\
				if (method.name == name, \{\
					found = \cf3 true\cf0 ;\
					out << \cf4 "   "\cf0  << class.name << \cf4 "-"\cf0  << name << \cf4 " :     "\cf0 ;\
					if (method.argNames.isNil or: \{ method.argNames.size == 1 \}, \{\
						out << \cf4 "this."\cf0  << name;\
						if (name.isSetter, \{ out << \cf4 "(val)"\cf0 ; \});\
					\},\{\
						out << method.argNames.at(0);\
						if (name.asString.at(0).isAlpha, \{\
							out << \cf4 "."\cf0  << name << \cf4 "("\cf0 ;\
							method.argNames.do(\{ \cf3 arg\cf0  argName, i;\
								if (i > 0, \{\
									if (i != 1, \{ out << \cf4 ", "\cf0  \});\
									out << argName;\
								\});\
							\});\
							out << \cf4 ")"\cf0 ;\
						\},\{\
							out << \cf4 " "\cf0  << name << \cf4 " "\cf0 ;\
							out << method.argNames.at(1);\
						\});\
					\});\
					out.nl;\
				\});\
			\});\
		\});\
		if (found, \{\
			out.collection.newTextWindow(name.asString);\
			\cf2 //out.collection.post;\
\cf0 		\},\{\
			\cf3 Post\cf0  << \cf4 "\\nNo implementations of '"\cf0  << name << \cf4 "'.\\n"\cf0 ;\
		\});\
	\}\
	\
	interpretCmdLine \{\
		\cf2 // interpret some text from the command line\
\cf0 		interpreter.interpretCmdLine;\
	\}\
\
	interpretPrintCmdLine \{\
		\cf2 // interpret some text from the command line and print result\
\cf0 		interpreter.interpretPrintCmdLine;\
	\}\
\
	showHelp \{\
		\cf3 var\cf0  string;\
		string = interpreter.cmdLine;\
		\cf2 // should put a script to find appropriate help here.\
\cf0 		\cf3 Post\cf0  << \cf4 "\\nuser asked for help for '"\cf0  << string << \cf4 "'.\\n"\cf0 ;\
	\}	\
\
	isDemo \{ \cf3 _IsDemo\cf0  \}\
\
	shallowCopy \{ ^\cf3 this\cf0  \}\
	\
	*elapsedTime \{ \cf3 _ElapsedTime\cf0  \}\
\}\
\
\
\cf3 FunctionDef\cf0  \{\
	\cf3 var\cf0  raw1, raw2, <code, <literals, <prototypeFrame, <context, <argNames, <varNames;\
	\
	\cf2 // a FunctionDef is defined by a code within curly braces \{\}\
\cf0 	\cf2 // When you use a FunctionDef in your code it gets pushed on the stack\
\cf0 	\cf2 // as an instance of Function\
\cf0 	\
	dumpByteCodes \{ \cf3 _DumpByteCodes\cf0  \}\
	\
	numArgs \{ \cf3 _FunDef_NumArgs\cf0  \}		\cf2 // return number of arguments to the function\
\cf0 	numVars \{ \cf3 _FunDef_NumVars\cf0  \}		\cf2 // return number of arguments to the function\
\cf0 	varArgs \{ \cf3 _FunDef_VarArgs\cf0  \}		\cf2 // return boolean whether function has ellipsis argument\
\cf0 \
	shallowCopy \{ ^\cf3 this\cf0  \}\
	\
	asFunction \{ \
		\cf2 // this is only legal for closed functions.\
\cf0 		\cf3 _FunctionDefAsFunction\cf0  \
		^\cf3 this\cf0 .primitiveFailed \
	\}\
	\
	dumpContexts \{\
		\cf3 _FunctionDefDumpContexts\cf0 \
	\}\
	inspectorClass \{ ^\cf3 FunctionDefInspector\cf0  \}\
	\
	findReferences \{ \cf3 arg\cf0  aSymbol, references; \
		\cf3 var\cf0  lits;\
		lits = literals.asArray;\
		if (lits.includes(aSymbol), \{\
			references = references.add(\cf3 this\cf0 );\
		\});\
		lits.do(\{ \cf3 arg\cf0  item;\
			if (item.isKindOf(\cf3 FunctionDef\cf0 ), \{\
				references = item.findReferences(aSymbol, references)\
			\})\
		\});\
		^references\
	\}\
	\
\}\
\
\cf3 Method\cf0  : \cf3 FunctionDef\cf0  \{\
	\cf3 var\cf0  <ownerClass, <name, <primitiveName;\
	\cf3 var\cf0  <filenameSymbol, <charPos;\
\
	openCodeFile \{\
		\cf3 this\cf0 .filenameSymbol.asString.openTextFile(\cf3 this\cf0 .charPos, -1);\
	\}\
	inspectorClass \{ ^\cf3 MethodInspector\cf0  \}\
\}\
\
\cf3 Frame\cf0  \{\
	\cf2 // since Frames are often created on the stack, it is too \
\cf0 	\cf2 // dangerous to allow access to them. Dangling pointers\
\cf0 	\cf2 // could result.\
\cf0 	shallowCopy \{ ^\cf3 this\cf0  \}\
	canInspect \{ ^\cf3 false\cf0  \}\
\}\
\
\cf3 DebugFrame\cf0  \{\
	\cf3 var\cf0  <functionDef, <args, <vars, <caller, <context, <address;\
	\cf2 // Object.getBackTrace returns one of these.\
\cf0 	\cf2 // 'functionDef' is the FunctionDef for this function or method.\
\cf0 	\cf2 // 'args' the values of the arguments to the function call.\
\cf0 	\cf2 // 'vars' the values of the local variables.\
\cf0 	\cf2 // 'caller' points to another DebugFrame for the caller to this function.\
\cf0 	\cf2 // 'context' points to another DebugFrame for the frame lexically enclosing this one.\
\cf0 	\cf2 // 'address' memory address of the actual frame object.\
\cf0 \}\
\
\cf3 RawPointer\cf0  \{\
	\cf2 // class used to hold raw pointers from the \
\cf0 	\cf2 // host environment. \
\cf0 	\cf2 // no instance variables, no methods\
\cf0 \}\
\
\cf3 Interpreter\cf0  \{	\
	\cf2 // The interpreter defines a context in which interactive commands\
\cf0 	\cf2 // are compiled.\
\cf0 	\
	\cf3 var\cf0  <cmdLine; \cf2 // place holder for text executed from a worksheet\
\cf0 	\cf3 var\cf0  context; \cf2 // faked interpreter context frame. Don't mess with it.\
\cf0 	\
	\cf2 // a-z are predefined variables for use by the interactive context.\
\cf0 	\cf2 // They are read+write so that programmatic methods can\
\cf0 	\cf2 // get and alter the values that the interpreter has access to.\
\cf0 	\cf3 var\cf0  <>a, <>b, <>c, <>d, <>e, <>f, <>g, <>h, <>i, <>j;\
	\cf3 var\cf0  <>k, <>l, <>m, <>n, <>o, <>p, <>q, <>r, <>s, <>t;\
	\cf3 var\cf0  <>u, <>v, <>w, <>x, <>y, <>z;\
	\
	*new \{ ^\cf3 this\cf0 .shouldNotImplement \}\
	*newClear \{ ^\cf3 this\cf0 .shouldNotImplement \}\
	\
	\cf2 //interpretCmdLine \{ ^this.interpret(cmdLine) \}\
\cf0 	\cf2 //interpretPrintCmdLine \{ ^this.interpretPrint(cmdLine) \}\
\cf0 	\
	interpretCmdLine \{\
		^\cf3 this\cf0 .compile(cmdLine).value;\
	\}\
	interpretPrintCmdLine \{\
         \cf2 //if (cmdLine.last != $\\n , \{ "\\n".post; \});\
\cf0 		\cf4 "\\n"\cf0 .post;\
		^\cf3 this\cf0 .compile(cmdLine).value.postln;\
	\}\
	\
	interpret \{ \cf3 arg\cf0  string;\
		\cf2 // compile, evaluate\
\cf0 		cmdLine = string;\
		^\cf3 this\cf0 .compile(string).value;\
	\}\
	interpretPrint \{ \cf3 arg\cf0  string;\
		\cf2 // compile, evaluate, print\
\cf0 		cmdLine = string;\
		^\cf3 this\cf0 .compile(string).value.postln;\
	\}\
	compile \{ \cf3 arg\cf0  string;\
		\cf3 _CompileExpression\cf0 \
		\cf2 // compiles string and returns a Function.\
\cf0 		\cf2 // the string must be a valid expression.\
\cf0 		\cf2 // You cannot compile a class definition this way.\
\cf0 		\cf2 // This method is not implemented in SCPlay.\
\cf0 		^\cf3 nil\cf0 \
	\}\
	\
	clearAll \{\
		a = b = c = d = e = f = g = h = i = j = k = l = m = \
		n = o = p = q = r = s = t = u = v = w = x = y = z = \cf3 nil\cf0 ;\
	\}\
	\
	executeFile \{ \cf3 arg\cf0  pathName;\
		^\cf3 this\cf0 .compileFile(pathName).value;\
	\}\
	\
	compileFile \{ \cf3 arg\cf0  pathName;\
		\cf3 var\cf0  file, text;\
		file = \cf3 File\cf0 .new(pathName, \cf4 "r"\cf0 );\
		if (file.isNil, \{ \
			error(\cf4 "file open failed\\n"\cf0 );\
			^\cf3 nil\cf0  \
		\});\
		text = file.readAllString;\
		file.close;\
		^\cf3 this\cf0 .compile(text)\
	\}\
		\
	\cf2 // PRIVATE\
\cf0 	functionCompileContext \{ \
		\cf2 // compiler uses this method as a fake context in which to compile \
\cf0 		\cf2 // the user's function.\
\cf0 		\cf2 // Do not edit this method!\
\cf0 		\
		\{\}	\cf2 // this forces the compiler to generate a heap allocated frame rather than \
\cf0 			\cf2 // a frame on the stack\
\cf0 	\}\
	shallowCopy \{ ^\cf3 this\cf0  \}\
\}\
\
}