String[char] : RawArray {
	classvar <>unixCmdActions;

	*initClass {
		unixCmdActions = IdentityDictionary.new;
	}

	*doUnixCmdAction {
		arg res, pid;
		unixCmdActions[pid].value(res, pid);
		unixCmdActions.removeAt(pid);
	}

	prUnixCmd {
		arg postOutput = true;
		_String_POpen
		^this.primitiveFailed
	}

	// runs a unix command and sends stdout to the post window
	unixCmd {
		arg action, postOutput = true;
		var pid;
		pid = this.prUnixCmd(postOutput);
		if(action.notNil) {
			unixCmdActions.put(pid, action);
		};
		^pid;
	}

	// Like unixCmd but gets the result into a string
	unixCmdGetStdOut {
		var pipe, lines, line;

		pipe = Pipe.new(this, "r");
		lines = "";
		line = pipe.getLine;
		while({line.notNil}, {lines = lines ++ line ++ "\n"; line = pipe.getLine; });
		pipe.close;

		^lines;
	}

	asSymbol {
		_StringAsSymbol
		^this.primitiveFailed
	}
	asInteger {
		_String_AsInteger
		^this.primitiveFailed
	}
	asFloat {
		_String_AsFloat
		^this.primitiveFailed
	}
	ascii {
		^Array.fill(this.size, { |i| this[i].ascii })
	}

	stripRTF {
		_StripRtf
		^this.primitiveFailed
	}

	stripHTML {
		_StripHtml
		^this.primitiveFailed
	}

	getSCDir {
		_String_GetResourceDirPath
		^this.primitiveFailed
	}

	*scDir {
		^"".getSCDir
	}

	compare { arg aString, ignoreCase=false; _StringCompare }
	< { arg aString; ^this.compare(aString, true) < 0 }
	> { arg aString; ^this.compare(aString, true) > 0 }
	<= { arg aString; ^this.compare(aString, true) <= 0 }
	>= { arg aString; ^this.compare(aString, true) >= 0 }
	== { arg aString; ^this.compare(aString, true) == 0 }
	!= { arg aString; ^this.compare(aString, true) != 0 }
	hash { _StringHash }

	// no sense doing collect as per superclass collection
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber;
		^aNumber.asString.perform(aSelector, this);
	}
	performBinaryOpOnComplex { arg aSelector, aComplex;
		^aComplex.asString.perform(aSelector, this);
	}

	isString { ^true }
	asString { ^this }
	asCompileString { _String_AsCompileString; }
	species { ^String }

	postln { _PostLine }
	post { _PostString }
	postcln { "// ".post; this.postln; }
	postc { "// ".post; this.post; }

	postf { arg ... items;  ^this.prFormat( items.collect(_.asString) ).post }
	format { arg ... items; ^this.prFormat( items.collect(_.asString) ) }
	prFormat { arg items; _String_Format ^this.primitiveFailed }
	matchRegexp { arg string, start = 0, end; _String_Regexp ^this.primitiveFailed }

	fformat { arg ... args;
		var str, resArgs, val, func;
		var suffixes, sig = false;

		this.do { |char|
			if(sig) {
				val = args.removeAt(0);
				func = Post.formats[char];
				if(func.isNil) {
					resArgs = resArgs.add(val);
					str = str ++ char
				} {
					resArgs = resArgs.add(func.value(val))
				};
				sig = false;
			} {
				str = str ++ char
			};
			if(char == $%) { sig = true };
		};
		^str.format(*resArgs)
	}

	die { arg ... culprits;
		if(culprits.notEmpty,{
			("\n\nFATAL ERROR: ").postln;
			culprits.do({ arg c; if(c.isString,{c.postln},{c.dump}) });
		});
		Error(this).throw;
	}
	error { "ERROR:\n".post; this.postln; }
	warn { "WARNING:\n".post; this.postln }
	inform { ^this.postln }
	++ { arg anObject; ^this prCat: anObject.asString; }
	+ { arg anObject; ^this prCat: " " prCat: anObject.asString; }
	catArgs { arg ... items; ^this.catList(items) }
	scatArgs { arg ... items; ^this.scatList(items) }
	ccatArgs { arg ... items; ^this.ccatList(items) }
	catList { arg list;
		// concatenate this with a list as a string
		var string = this.copy;
		list.do({ arg item, i;
			string = string ++ item;
		});
		^string
	}
	scatList { arg list;
		var string = this.copy;
		list.do({ arg item, i;
			string = string prCat: " " ++ item;
		});
		^string
	}
	ccatList { arg list;
		var string = this.copy;
		list.do({ arg item, i;
			string = string prCat: ", " ++ item;
		});
		^string
	}
	split { arg separator=$/;
		var word="";
		var array=[];
		separator=separator.ascii;

		this.do({arg let,i;
			if(let.ascii != separator ,{
				word=word++let;
			},{
				array=array.add(word);
				word="";
			});
		});
		^array.add(word);
	}

	containsStringAt { arg index, string;
		^compare( this[index..index + string.size-1], string, false) == 0
	}

	icontainsStringAt { arg index, string;
		^compare( this[index..index + string.size-1], string, true) == 0
	}


	contains { arg string, offset = 0;
		^this.find(string, false, offset).notNil
	}
	containsi { arg string, offset = 0;
		^this.find(string, true, offset).notNil
	}

	findRegexp { arg regexp, offset = 0;
       _String_FindRegexp
       ^this.primitiveFailed
	}

	findAllRegexp { arg string, offset = 0;
		var indices = [], i=[];
		while {
			i = this.findRegexp(string, offset);
			i.notNil and: {i.size != 0}
		}{
			indices = indices.add(i);
			offset = i[0][0] + 1;
		}
		^indices
	}

	find { arg string, ignoreCase = false, offset = 0;
		_String_Find
		^this.primitiveFailed
	}
	findBackwards { arg string, ignoreCase = false, offset = 0x7FFFFFFE;
		_String_FindBackwards
		^this.primitiveFailed
	}
	endsWith { arg string;
		^this.contains(string, this.size - string.size)
	}
	beginsWith { arg string;
		^this.containsStringAt(0, string)
	}
	findAll { arg string, ignoreCase = false, offset=0;
		var indices, i=0;
		while {
			i = this.find(string, ignoreCase, offset);
			i.notNil
		}{
			indices = indices.add(i);
			offset = i + 1;
		}
		^indices
	}
	replace { arg find, replace;
		^super.replace(find, replace).join
	}


	escapeChar { arg charToEscape; // $"
		^this.class.streamContents({ arg st;
			this.do({ arg char;
				if(char == charToEscape,{
					st << $\\
				});
				st << char;
			})
		})
	}
	quote {
		^"\"" ++ this ++ "\""
	}
	tr { arg from,to;
		^this.collect({ arg char;
			if(char == from,{to},{char})
		})
	}

	insert { arg index, string;
		^this.keep(index) ++ string ++ this.drop(index)
	}

	zeroPad {
		^" " ++ this ++ " "
	}
	
	underlined { arg char = $-;
		^this ++ "\n" ++ String.fill(this.size, char)
	}

	scramble {
		^this.as(Array).scramble.as(String)
	}

	rotate { |n|
		^this.as(Array).rotate(n).as(String)
	}

	compile { ^thisProcess.interpreter.compile(this); }
	interpret { ^thisProcess.interpreter.interpret(this); }
	interpretPrint { ^thisProcess.interpreter.interpretPrint(this); }

	*readNew { arg file;
		^file.readAllString;
	}
	prCat { arg aString; _ArrayCat }

	printOn { arg stream;
		stream.putAll(this);
	}
	storeOn { arg stream;
		stream.putAll(this.asCompileString);
	}

	inspectorClass { ^StringInspector }

	/// unix

	standardizePath {
		_String_StandardizePath
		^this.primitiveFailed
	}
	withTrailingSlash {
		var sep = thisProcess.platform.pathSeparator;
		if(this.last != sep, {
			^this ++ sep
		},{
			^this
		})
	}
	withoutTrailingSlash {
		var sep = thisProcess.platform.pathSeparator;
		if(this.last == sep,{
			^this.copyRange(0, this.size-2)
		},{
			^this
		})
	}

	absolutePath {
		var first, sep;
		sep = thisProcess.platform.pathSeparator;
		first = this[0];
		if(first == sep){^this};
		if(first == $~){^this.standardizePath};
		^File.getcwd ++ sep ++ this;
	}

	pathMatch { _StringPathMatch ^this.primitiveFailed } // glob
	load {
		^thisProcess.interpreter.executeFile(this);
	}
	loadPaths { |warn=true|
		var paths = this.pathMatch;
		if(warn and:{paths.isEmpty}) { ("no files found for this path:" + this.quote).warn };
		^paths.collect({ arg path;
			thisProcess.interpreter.executeFile(path);
		});
	}
	loadRelative {
		var path = thisProcess.nowExecutingPath;
		if(path.isNil) { Error("can't load relative to an unsaved file").throw};
		if(path.basename == this) { Error("should not load a file from itself").throw };
		^(path.dirname ++ thisProcess.platform.pathSeparator ++ this).loadPaths
	}
	resolveRelative {
		var path = thisProcess.nowExecutingPath;
		if(this[0] == thisProcess.platform.pathSeparator, {^this});
		if(path.isNil) { Error("can't resolve relative to an unsaved file").throw};
		^(path.dirname ++ thisProcess.platform.pathSeparator ++ this)
	}
	include {
		if(Quarks.isInstalled(this).not) {
			Quarks.install(this);
			"... the class library may have to be recompiled.".postln;
			// maybe check later whether there are .sc files included.
		}
	}
	exclude {
		if(Quarks.isInstalled(this)) {
			Quarks.uninstall(this);
			"... the class library may have to be recompiled.".postln;
		}
	}
	basename {
		_String_Basename;
		^this.primitiveFailed
	}
	dirname {
		_String_Dirname;
		^this.primitiveFailed
	}
	splitext {
		this.reverseDo({ arg char, i;
			if (char == $\., {
				^[this.copyFromStart(this.size - 2 - i), this.copyToEnd(this.size - i)]
			});
		});
		^[this, nil]
	}

	// path concatenate
	+/+ { arg path;
		var pathSeparator = thisProcess.platform.pathSeparator;

		if (path.respondsTo(\fullPath)) {
			^PathName(this +/+ path.fullPath)
		};

		if (this.last == pathSeparator or: { path.first == pathSeparator }) {
			^this ++ path
		};

		^this ++ pathSeparator ++ path
	}

	asRelativePath { |relativeTo|
		^PathName(this).asRelativePath(relativeTo)
	}
	asAbsolutePath {
			// changed because there is no need to create a separate object
			// when String already knows how to make an absolute path
		^this.absolutePath;  // was ^PathName(this).asAbsolutePath
	}

	// runs a unix command and returns the result code.
	systemCmd { _String_System ^this.primitiveFailed }

	gethostbyname { _GetHostByName ^this.primitiveFailed }

	// gets value of environment variable
	getenv {
		_String_Getenv
		^this.primitiveFailed
	}
	// sets value of environment variable
	// value may be nil to unset the variable
	setenv { arg value;
		_String_Setenv
		^this.primitiveFailed
	}
	unsetenv {
		^this.setenv(nil)
	}

	/// code gen
	codegen_UGenCtorArg { arg stream;
		stream << this.asCompileString;
	}
	ugenCodeString { arg ugenIndex, isDecl, inputNames=#[], inputStrings=#[];
		_UGenCodeString
		^this.primitiveFailed
	}

	asSecs { |maxDays = 365| // assume a timeString of ddd:hh:mm:ss.sss. see asTimeString.
		var time = 0, sign = 1, str = this;

		if (this.first == $-, {
			str = this.drop(1);
			sign = -1
		});

		str.split($:).reverseDo { |num, i|
			num = num.interpret;
			if (num > [60, 60, 24, maxDays][i]) { ("number greater than allowed:" + this).warn; ^this };
			if (num < 0) { ("negative numbers within slots not supported:" + this).warn; ^this };
			time = time + (num * [1.0, 60.0, 3600.0, 86400.0][i]);
		};
		^time * sign;
	}

	speak { arg channel = 0, force = false;
		var speech = GUI.current.speech;
		if( speech.initialized.not, { speech.init });
		speech.channels[ channel ].speak( this, force );
	}

	toLower {
		^this.collect(_.toLower)
	}
	toUpper {
		^this.collect(_.toUpper)
	}
}
