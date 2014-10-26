ScIDE {
	classvar subListSorter;
	classvar <>currentPath;
	classvar <defaultServer;
	classvar serverController;
	classvar volumeController, suppressAmpResponse = false;
	classvar docRoutine;

	*initClass {
		subListSorter = { | a b | a[0].perform('<', b[0]) };

		Class.initClassTree(Server);

		StartUp.add {
			if (ScIDE.connected) {
				this.handshake
			};
		}
	}

	*connect {|ideName|
		this.prConnect(ideName);
		defer {
			this.handshake
		}
	}

	*handshake {
		this.prSend(\classLibraryRecompiled);
		this.prSend(\requestDocumentList);
		this.prSend(\requestCurrentPath);

		this.defaultServer = Server.default;
		this.sendIntrospection;
	}

	*defaultServer_ {|server|
		serverController.remove;
		serverController = SimpleController(server)
		.put(\serverRunning, { | server, what, extraArg |
			this.prSend(\defaultServerRunningChanged, [
				server.serverRunning, server.addr.hostname, server.addr.port]);
		})
		.put(\default, { | server, what, newServer |
			("changed default server to:" + newServer.name).postln;
			this.defaultServer = newServer;
		})
		.put(\dumpOSC, { | volume, what, code |
			this.prSend( if(code.asBoolean, \dumpOSCStarted, \dumpOSCStopped) );
		});

		volumeController.remove;
		volumeController = SimpleController(server.volume)
		.put(\mute, { | volume, what, muted |
			this.prSend( if(muted, \serverMuted, \serverUnmuted) );
		})
		.put(\amp, { | volume, what, amp |
			if (not(suppressAmpResponse)) {
				this.prSend( \serverAmp, amp.asString );
			};
		})
		.put(\ampRange, { |volume, what, min, max|
			this.prSend( \serverAmpRange, "%,%".format(min, max) );
		});

		defaultServer = server;

		this.prSend(\defaultServerRunningChanged, [
			server.serverRunning, server.addr.hostname, server.addr.port]);
		this.prSend( if(server.volume.isMuted, \serverMuted, \serverUnmuted) );
		this.prSend( if(server.dumpMode.asBoolean, \dumpOSCStarted, \dumpOSCStopped) );
		this.prSend( \serverAmpRange, "%,%".format(server.volume.min, server.volume.max) );
		this.prSend( \serverAmp, server.volume.volume.asString );

	}

	*request { |id, command, data|
		this.tryPerform(command, id, data);
	}

	*connected {
		_ScIDE_Connected
	}


	*setServerVolume { arg volume;
		var suppressed = suppressAmpResponse;
		suppressAmpResponse = true;
		defaultServer.volume = volume;
		suppressAmpResponse = suppressed;
	}

	*sendIntrospection {
		var res = [];
		Class.allClasses.do { |class|
			var classData;
			classData = [
				class.name,
				class.class.name,
				class.superclass !? {class.superclass.name},
				class.filenameSymbol,
				class.charPos,
				class.methods.collect { |m| this.serializeMethodDetailed(m) };
			];
			res = res.add(classData);
		};
		this.prSend(\introspection, res);
	}

	*sendAllClasses { |id|
		this.prSend(id, Class.allClasses.collectAs(_.asString, Array))
	}

	*sendSymbolTable { |id|
		var result, dt;

		dt = {
			result = IdentitySet(16384);
			Class.allClasses.do { | class |
				if (class.isMetaClass.not) {
					result.add(class.name);
				};
				class.methods.do { | method |
					result.add(method.name);
				};
			};

			result = result.collectAs(_.asString, Array)
		}.bench(false);

		"ScIDE: Built symbol table in % seconds\n".postf(dt.asStringPrec(3));

		this.prSend(id, result)
	}

	*completeClass { |id, text|
		var res = [];
		Class.allClasses.do { |class|
			var name = class.name.asString;
			if (name.beginsWith(text)) {
				res = res.add(name);
			};
		};
		if (res.size > 0) {
			this.prSend(id, res);
		};
	}

	*completeClassMethod { |id, text|
		var class, methods, res;
		class = text.asSymbol.asClass;
		if (class.notNil) {
			methods = IdentityDictionary();
			class = class.class;
			while { class.notNil } {
				class.methods.do { |method|
					// methods include operators like "+", but those are
					// actually not valid in the method call syntax
					if (method.name.asString[0].isAlpha &&
						methods[method.name].isNil)
					{
						methods.put(method.name, method);
					};
				};
				class = class.superclass;
			};
			res = methods.values.collect { |m| this.serializeMethod(m) };
			if (res.size > 0) { this.prSend(id, res) };
		}
	}

	*completeMethod { |id, text|
		var res = [];
		Class.allClasses.do { |class|
			class.methods.do { |method|
				var signature;
				var definition;
				if (method.name.asString.beginsWith(text)) {
					res = res.add( this.serializeMethod(method) );
				};
			};
		};
		if (res.size > 0) { this.prSend(id, res) };
	}

	*findMethod { |id, text|
		var cname, mname, tokens, res;
		var class, method;

		tokens = text.split($.);
		if (tokens.size > 1) {
			cname = tokens[0];
			mname = tokens[1];
		}{
			mname = tokens[0];
		};
		if (mname.size < 1) { ^this };

		if (cname.size > 0) {
			class = cname.asSymbol.asClass;
			if (class.isNil) {
				warn("No class named" + cname.asString);
				^this;
			};
			method = class.class.findRespondingMethodFor(mname.asSymbol);
			if (method.isNil) {
				warn("No such method:" + cname.asString ++ "." ++ mname.asString);
				^this;
			};
			this.prSend(id, [this.serializeMethod(method)]);
		}{
			res = [];
			this.allMethodsDo { |method|
				if (method.name.asString == mname) {
					res = res.add( this.serializeMethod(method) );
				};
			};
			if (res.size > 0) {
				this.prSend(id, res)
			}{
				warn("No such method:" + mname.asString);
				^this;
			};
		}
	}

	*serializeMethod { arg method;
		var data = [method.ownerClass.name, method.name];
		if (method.argNames.size > 1) {
			data = data ++ [
				method.argNames.as(Array),
				method.prototypeFrame.collect { |val| val !? val.cs }
			].lace [2..];
		};
		^data;
	}

	*serializeMethodDetailed { arg method;
		var args, data;
		args = [];
		if (method.argNames.size > 1) {
			args = args ++ [
				method.argNames.as(Array),
				method.prototypeFrame.collect { |val|
					val !? {
						if (val.class === Float) { val.asString } { val.cs }
					}
				};
			].lace [2..];
		};
		data = [
			method.ownerClass.name,
			method.name,
			method.filenameSymbol,
			method.charPos,
			args
		];
		^data;
	}

	*allMethodsDo { arg func;
		Class.allClasses.do { |class|
			class.methods.do { |method|
				func.value(method);
			};
		};
	}

	*findReferencesToSymbol {|requestId, symbol|
		var methods;
		var result = SortedList(8, subListSorter);
		var references = Class.findAllReferences(symbol.asSymbol);

		if (references.notNil) {
			methods = IdentitySet.new;
			references.do { | funcDef |
				var homeContext;
				homeContext = if(funcDef.context.isNil) {funcDef} {funcDef.context.homeContext};
				if (homeContext.isKindOf(Method)) {
					methods.add(homeContext);
				};
			};
			methods.do { | method |
				result.add([
					method.ownerClass.name,
					method.name,
					method.filenameSymbol.asString,
					method.charPos + 1
				])
			}
		};

		ScIDE.prSend(requestId, [symbol, result.asArray])
	}

	*openHelpUrl { |url|
		ScIDE.processUrl(url, { |processedUrl|
			this.prSend("openHelpUrl", processedUrl.asString)
		});
	}

	*cmdPeriod { docRoutine.play(AppClock) }

	*processUrl { |urlString, doneAction|
		// NOTE: Copied and modified from HelpBrower:-goTo
		var url, brokenFunc;

		brokenFunc = { |fragment|
			var brokenUrl = URI.fromLocalPath( SCDoc.helpTargetDir++"/BrokenLink.html" );
			brokenUrl.fragment = fragment;
			brokenUrl;
		};

		url = URI(urlString);

		if (docRoutine.notNil) { docRoutine.stop };
		docRoutine = Routine {
			try {
				url = SCDoc.prepareHelpForURL(url) ?? { brokenFunc.(urlString) };
				doneAction.value(url);
			} {|err|
				err.throw;
			};
			CmdPeriod.remove(this);
			docRoutine = nil;
		}.play(AppClock);
		CmdPeriod.add(this);
	}

	// Document Support //////////////////////////////////////////////////

	*newDocument {|title="Untitled", string="", id|
		this.prSend(\newDocument, [title, string, id]);
	}


	*open { |path, charPos = 0, selectionLength = 0, id|
		this.prSend(\openFile, [path, charPos, selectionLength, id])
	}

	*getQUuid {
		_ScIDE_GetQUuid
		this.primitiveFailed
	}

	*getTextByQUuid {|quuid, funcID, start = 0, range = -1|
		this.prSend(\getDocumentText, [quuid, funcID, start, range]);
	}

	*setTextByQUuid {|quuid, funcID, text, start = 0, range = -1|
		this.prSend(\setDocumentText, [quuid, funcID, text, start, range]);
	}
    
    *setSelectionByQUuid {|quuid, start, length|
        this.prSend(\setDocumentSelection, [quuid, start, length]);
    }
	
	*setEditablebyQUuid {|quuid, editable|
		this.prSend(\setDocumentEditable, [quuid, editable]);
	}
	
	*setPromptsToSavebyQUuid {|quuid, prompts|
		this.prSend(\setDocumentPromptsToSave, [quuid, prompts]);
	}

	*setCurrentDocumentByQUuid {|quuid|
		this.prSend(\setCurrentDocument, [quuid]);
	}
	
	*removeDocUndoByQUuid {|quuid|
		this.prSend(\removeDocUndo, [quuid]);
	}

	*close {|quuid|
		this.prSend(\closeDocument, [quuid]);
	}

	*setDocumentTitle {|quuid, newTitle|
		this.prSend(\setDocumentTitle, [quuid, newTitle]);
	}

	*setDocumentKeyDownEnabled {|quuid, bool|
		this.prSend(\enableDocumentKeyDownAction, [quuid, bool]);
	}

	*setDocumentKeyUpEnabled {|quuid, bool|
		this.prSend(\enableDocumentKeyUpAction, [quuid, bool]);
	}
	
	*setDocumentGlobalKeyDownEnabled {|bool|
		this.prSend(\enableDocumentGlobalKeyDownAction, [bool]);
	}
	
	*setDocumentGlobalKeyUpEnabled {|bool|
		this.prSend(\enableDocumentGlobalKeyUpAction, [bool]);
	}

	*setDocumentMouseDownEnabled {|quuid, bool|
		this.prSend(\enableDocumentMouseDownAction, [quuid, bool]);
	}

	*setDocumentMouseUpEnabled {|quuid, bool|
		this.prSend(\enableDocumentMouseUpAction, [quuid, bool]);
	}

	*setDocumentTextChangedEnabled {|quuid, bool|
		this.prSend(\enableDocumentTextChangedAction, [quuid, bool]);
	}

	*setDocumentTextMirrorEnabled {|bool|
		this.prSend(\enableDocumentTextMirror, [bool]);
	}



	// PRIVATE ///////////////////////////////////////////////////////////

	*prSend {|id, data|
		_ScIDE_Send
		this.primitiveFailed
	}

	*prConnect {|ideName|
		_ScIDE_Connect
		this.primitiveFailed
	}
}

Document {
	classvar <dir="", <allDocuments, <>current;
	classvar <globalKeyDownAction, <globalKeyUpAction, <>initAction;
	classvar <>autoRun = true;
	classvar <asyncActions;
	var <quuid, <title, <isEdited = false, <path;
	var <keyDownAction, <keyUpAction, <mouseUpAction, <mouseDownAction;
	var <>toFrontAction, <>endFrontAction, <>onClose, <textChangedAction;

	var <envir, savedEnvir;
	var <editable = true, <promptToSave = true;

	*initClass{
		asyncActions = IdentityDictionary.new;
	}

	*new {|title = "untitled", string = "", envir|
		var quuid = ScIDE.getQUuid, doc;
		ScIDE.newDocument(title, string, quuid);
		doc = super.new.init(quuid, title, string);
		if (envir.notNil and: { doc.notNil }) { doc.envir_(envir) };
		doc.prAdd;
		^doc
	}

	*open { | path, selectionStart=0, selectionLength=0, envir |
		var doc;
		path = this.standardizePath(path);
		allDocuments.do{ |d|
			if(d.path == path.absolutePath){
				doc = d
			};
		};
		if(doc.notNil, { ^doc.front });
		doc = super.new.initFromPath(path, selectionStart, selectionLength);
		if (envir.notNil and: { doc.notNil }) { doc.envir_(envir) };
		^doc
	}

	*syncFromIDE {|quuid, title, chars, isEdited, path, selStart, selSize|
		var doc;
		isEdited = isEdited.booleanValue;
		chars = String.fill(chars.size, {|i| chars[i].asAscii});
		title = String.fill(title.size, {|i| title[i].asAscii});
		path = String.fill(path.size, {|i| path[i].asAscii});
		if((doc = this.findByQUuid(quuid)).isNil, {
			doc = super.new.initFromIDE(quuid, title, chars, isEdited, path, selStart, selSize);
			allDocuments = allDocuments.add(doc);
		}, {doc.initFromIDE(quuid, title, chars, isEdited, path)});
	}

	*syncDocs {|docInfo| // [quuid, title, string, isEdited, path, selStart, selSize]
		docInfo.do({|info| this.syncFromIDE(*info) });
	}

	*executeAsyncResponse {|funcID ...args|
		var func;
		func = asyncActions[funcID];
		asyncActions[funcID] = nil;
		func.value(*args);
	}

	*findByQUuid {|quuid|
		^allDocuments.detect({|doc| doc.quuid == quuid });
	}

	*setActiveDocByQUuid {|quuid|
		var newCurrent;
		newCurrent = this.findByQUuid(quuid);
		this.prCurrent_(newCurrent);
	}

	*prCurrent_ {|newCurrent|
		current = this.current;
		if((newCurrent === current).not, {
			if(current.notNil, {current.didResignKey});
			newCurrent.didBecomeKey;
		});
	}

	*dir_ { | path |
		path = path.standardizePath;
		if(path == "") { dir = path } {
			if(pathMatch(path).isEmpty) { ("there is no such path:" + path).postln } {
				dir = path ++ "/"
			}
		}
	}

	*standardizePath { | p |
		var pathName;
		pathName = PathName(p.standardizePath);
		^if(pathName.isRelativePath) {
			dir  ++ pathName.fullPath
		} {
			pathName.fullPath
		}
	}

	*abrevPath { | path |
		if(path.size < dir.size) { ^path };
		if(path.copyRange(0,dir.size - 1) == dir) {
			^path.copyRange(dir.size, path.size - 1)
		};
		^path
	}

	*openDocuments {
		^allDocuments
	}

	*hasEditedDocuments {
		allDocuments.do { | doc |
			if(doc.isEdited) {
				^true;
			}
		}
		^false
	}

	*closeAll {
		allDocuments.do { | doc | doc.close }
	}

	*closeAllUnedited {
		var listenerWindow;
		allDocuments.do({ | doc |
			if(doc.isEdited.not, {
				doc.close;
			});
		});
	}

	closed {
		onClose.value(this); // call user function
		this.restoreCurrentEnvironment;
		allDocuments.remove(this);
	}

	front {
		this.class.prCurrent_(this);
		ScIDE.setCurrentDocumentByQUuid(quuid);
	}

	init {|id, argtitle, argstring, argisEdited|
		quuid = id;
		title = argtitle;
		this.text = argstring;
		isEdited = argisEdited;
	}

    initFromIDE {|id, argtitle, argstring, argisEdited, argPath, selStart, selSize|
		quuid = id;
		title = argtitle;
		this.prSetTextMirror(id, argstring, 0, -1);
        this.prSetSelectionMirror(id, selStart, selSize);
		isEdited = argisEdited;
		path = argPath;
	}

	initFromPath { | argpath, selectionStart, selectionLength |
		quuid = ScIDE.getQUuid;
		this.prReadTextFromFile(argpath);
		this.propen(argpath, selectionStart, selectionLength);
		path = argpath;
		title = path.basename;
		isEdited = false;
		this.prAdd;
	}

	textChanged {|index, numCharsRemoved, addedChars|
		addedChars = String.fill(addedChars.size, {|i| addedChars[i].asAscii});
		textChangedAction.value(this, index, numCharsRemoved, addedChars);
	}

	propen {|path, selectionStart, selectionLength, envir|
		^ScIDE.open(path, selectionStart, selectionLength, quuid)
	}

	close { ScIDE.close(quuid); }

/*	// asynchronous get
	// range -1 means to the end of the Document
	getText {|action, start = 0, range -1|
		var funcID;
		funcID = ScIDE.getQUuid; // a unique id for this function
		asyncActions[funcID] = action; // pass the text
		ScIDE.getTextByQUuid(quuid, funcID, start, range);
	}*/

	getText {|action, start = 0, range -1|
		^prGetTextFromMirror(quuid, start, range);
	}

	prGetTextFromMirror {|id, start=0, range = -1|
		_ScIDE_GetDocTextMirror
		this.primitiveFailed
	}

	// asynchronous set
	prSetText {|text, action, start = 0, range -1|
		var funcID;
		// first set the back end mirror
		this.prSetTextMirror(quuid, text, start, range);
		// set the SCIDE Document
		ScIDE.setTextByQUuid(quuid, funcID, text, start, range);
	}

	// set the backend mirror
	prSetTextMirror {|quuid, text, start, range|
		_ScIDE_SetDocTextMirror
		this.primitiveFailed
	}
    
    prSetSelectionMirror {|quuid, start, size|
		_ScIDE_SetDocSelectionMirror
		this.primitiveFailed
	}

	text_ {|string|
		this.prSetText(string);
	}

	text { ^this.prGetTextFromMirror(quuid, 0, -1); }

	rangeText { | rangestart=0, rangesize=1 |
		^this.prGetTextFromMirror(quuid, rangestart, rangesize);
	}

	insertText {|string, index = 0|
		this.prSetText(string, nil, index, 0);
	}

	getChar {|index = 0|
		^this.prGetTextFromMirror(quuid, index, 1);
	}

	setChar {|char, index = 0|
		this.prSetText(char.asString, nil, index, 1);
	}

	== { |that| ^(this.quuid === that.quuid);}

	hash {
		^quuid.hash
	}

	didBecomeKey {
		this.class.current = this;
		this.saveCurrentEnvironment;
		toFrontAction.value(this);
	}

	didResignKey {
		endFrontAction.value(this);
		this.restoreCurrentEnvironment;
	}

	keyDown { | modifiers, unicode, keycode, key |
		var character = unicode.asAscii;
		this.class.globalKeyDownAction.value(this,character, modifiers, unicode, keycode);
		keyDownAction.value(this,character, modifiers, unicode, keycode, key);
	}

	keyUp { | modifiers, unicode, keycode, key |
		var character = unicode.asAscii;
		this.class.globalKeyUpAction.value(this,character, modifiers, unicode, keycode);
		keyUpAction.value(this,character, modifiers, unicode, keycode, key);
	}

	mouseDown { | x, y, modifiers, buttonNumber, clickCount |
		mouseDownAction.value(this, x, y, modifiers, buttonNumber, clickCount)
	}

	mouseUp { | x, y, modifiers, buttonNumber |
		mouseUpAction.value(this, x, y, modifiers, buttonNumber)
	}

	keyDownAction_ {|action|
		keyDownAction = action;
		ScIDE.setDocumentKeyDownEnabled(quuid, action.notNil);
	}

	keyUpAction_ {|action|
		keyUpAction = action;
		ScIDE.setDocumentKeyUpEnabled(quuid, action.notNil);
	}

	mouseDownAction_ {|action|
		mouseDownAction = action;
		ScIDE.setDocumentMouseDownEnabled(quuid, action.notNil);
	}

	mouseUpAction_ {|action|
		mouseUpAction = action;
		ScIDE.setDocumentMouseUpEnabled(quuid, action.notNil);
	}

	textChangedAction_ {|action|
		textChangedAction = action;
		ScIDE.setDocumentTextChangedEnabled(quuid, action.notNil);
	}

	*globalKeyDownAction_ {|action|
		globalKeyDownAction = action;
		ScIDE.setDocumentGlobalKeyDownEnabled(action.notNil);
	}

	*globalKeyUpAction_ {|action|
		globalKeyUpAction = action;
		ScIDE.setDocumentGlobalKeyUpEnabled(action.notNil);
	}

	title_ {|newTitle|
		title = newTitle;
		ScIDE.setDocumentTitle(quuid, newTitle);
	}

	prSetEdited {|flag|
		isEdited = flag.booleanValue;
	}

	// this initialises the lang side text mirror
	prReadTextFromFile {|path|
		var file;
		file = File.new(path, "r");
		if (file.isNil, {
			error("Document open failed\n");
		});
		this.prSetTextMirror(quuid, file.readAllString, 0, -1);
		file.close;
	}

	prAdd {
		allDocuments = allDocuments.add(this);
		if (autoRun) {
			if (this.rangeText(0,7) == "/*RUN*/")
			{
				this.text.interpret;
			}
		};
		current = this;
		initAction.value(this);
	}

	isFront {
		^Document.current === this
	}

	selectionStart {
		^this.prGetSelectionStart(quuid)
	}

	prGetSelectionStart {|id|
		_ScIDE_GetDocSelectionStart
	}

	selectionSize {
		^this.prGetSelectionRange(quuid)
	}

	prGetSelectionRange {|id|
		_ScIDE_GetDocSelectionRange
	}

	string { | rangestart, rangesize = 1 |
		if(rangestart.isNil,{
		^this.text;
		});
		^this.rangeText(rangestart, rangesize);
	}

	string_ { | string, rangestart = -1, rangesize = 1 |
		this.prSetText(string, nil, rangestart, rangesize);
	}

	selectedString {
		^this.rangeText(this.selectionStart, this.selectionSize);
	}

	selectedString_ { | txt |
		this.prSetText(txt, nil, this.selectionStart, this.selectionSize);
	}

	currentLine {
		^this.getSelectedLines(this.selectionStart, 0);
	}

	getSelectedLines { | rangestart = -1, rangesize = 0 |
		var start, end, str, max;
		str = this.string;
		max = str.size;
		start = rangestart;
		end = start + rangesize;
		while {
			str[start] !== Char.nl and: { start >= 0 }
		} { start = start - 1 };
		while {
			str[end] !== Char.nl and: { end < max }
		} { end = end + 1 };
		^str.copyRange(start + 1, end);
	}

	// document setup

	path_ { |apath|
		this.notYetImplemented
	}

	dir {
		var path = this.path;
		^path !? { path.dirname }
	}

	name {
		this.deprecated(thisMethod, Document.findMethod(\title));
		^this.title
	}

	name_ { |aname|
		this.deprecated(thisMethod, Document.findMethod(\title_));
		this.title_(aname)
	}

	// envir stuff

	envir_ { | ev |
		envir = ev;
		if (this.class.current == this) {
			if(envir.isNil) {
				this.restoreCurrentEnvironment
			} {
				if (savedEnvir.isNil) {
					this.saveCurrentEnvironment
				}
			}
		}
	}

	restoreCurrentEnvironment {
		if (savedEnvir.notNil) {
			currentEnvironment = savedEnvir;
			savedEnvir = nil;
		}
	}

	saveCurrentEnvironment {
		if (envir.notNil) {
			savedEnvir = currentEnvironment;
			currentEnvironment = envir;
		}
	}

	selectLine { | line |
		var text, breaks, numLines, start = 0, end;
		if(line < 1, { line = 1 });
		text = this.text;
		breaks = text.findAll("\n");
		numLines = breaks.size + 1;
		line = min(line, numLines);
		if(line > 1, { start = breaks[line - 2] + 1});
		end = breaks[line - 1] ?? { text.size -1 };
		this.selectRange(start, end - start);
	}

	selectRange { | start=0, length=0 |
		this.prSetSelectionMirror(quuid, start, length); // set the backend mirror
        ScIDE.setSelectionByQUuid(quuid, start, length); // set the IDE doc
    }
    
	editable_ { | bool=true |
		editable = bool;
		ScIDE.setEditablebyQUuid(quuid, bool);
	}
	
	promptToSave_ { | bool |
		promptToSave = bool;
		ScIDE.setPromptsToSavebyQUuid(quuid, bool);
	}

	removeUndo {
		ScIDE.removeDocUndoByQUuid(quuid);
	}

	// probably still needed for compatibility
	*implementationClass { ^this }
}
