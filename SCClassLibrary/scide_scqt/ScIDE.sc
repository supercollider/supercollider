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
			if (this.connected) {
				this.handshake
			}
		}
	}

	*connect {|ideName|
		this.prConnect(ideName);
		this.handshake
	}

	*handshake {
		this.send(\classLibraryRecompiled);
		this.send(\requestDocumentList);
		this.send(\requestCurrentPath);

		this.defaultServer = Server.default;
		this.sendIntrospection;
	}

	*defaultServer_ {|server|
		serverController.remove;
		serverController = SimpleController(server)
		.put(\serverRunning, { | server, what, extraArg |
			this.send(\defaultServerRunningChanged, [
				server.serverRunning, server.addr.hostname, server.addr.port, server.unresponsive]);
		})
		.put(\default, { | server, what, newServer |
			("changed default server to:" + newServer.name).postln;
			this.defaultServer = newServer;
		})
		.put(\dumpOSC, { | server, what, code |
			this.send( if(code.asBoolean, \dumpOSCStarted, \dumpOSCStopped) );
		})
		.put(\recording, { | theChanger, what, flag |
			this.send( if(flag.asBoolean, \recordingStarted, \recordingStopped) );
		})
		.put(\pausedRecording, { | theChanger, what |
			this.send(\recordingPaused);
		})
		.put(\recordingDuration, { | theChanger, what, duration |
			this.send(\recordingDuration, duration.asString);
		})
		.put(\dumpOSC, { | volume, what, code |
			this.send( if(code.asBoolean, \dumpOSCStarted, \dumpOSCStopped) );
		});

		volumeController.remove;
		volumeController = SimpleController(server.volume)
		.put(\mute, { | volume, what, muted |
			this.send( if(muted, \serverMuted, \serverUnmuted) );
		})
		.put(\amp, { | volume, what, amp |
			if (not(suppressAmpResponse)) {
				this.send( \serverAmp, amp.asString );
			};
		})
		.put(\ampRange, { |volume, what, min, max|
			this.send( \serverAmpRange, "%,%".format(min, max) );
		});

		defaultServer = server;

		this.send(\defaultServerRunningChanged, [
			server.serverRunning, server.addr.hostname, server.addr.port, server.unresponsive]);
		this.send( if(server.volume.isMuted, \serverMuted, \serverUnmuted) );
		this.send( if(server.dumpMode.asBoolean, \dumpOSCStarted, \dumpOSCStopped) );
		this.send( \serverAmpRange, "%,%".format(server.volume.min, server.volume.max) );
		this.send( \serverAmp, server.volume.volume.asString );

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
		this.send(\introspection, res);
	}

	*sendAllClasses { |id|
		this.send(id, Class.allClasses.collectAs(_.asString, Array))
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

		this.send(id, result)
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
			this.send(id, res);
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
			if (res.size > 0) { this.send(id, res) };
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
		if (res.size > 0) { this.send(id, res) };
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
			this.send(id, [this.serializeMethod(method)]);
		}{
			res = [];
			this.allMethodsDo { |method|
				if (method.name.asString == mname) {
					res = res.add( this.serializeMethod(method) );
				};
			};
			if (res.size > 0) {
				this.send(id, res)
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

		this.send(requestId, [symbol, result.asArray])
	}

	*openHelpUrl { |url|
		this.processUrl(url, { |processedUrl|
			this.send("openHelpUrl", processedUrl.asString)
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
		this.send(\newDocument, [title, string, id]);
	}


	*open { |path, charPos = 0, selectionLength = 0, id|
		this.send(\openFile, [path, charPos, selectionLength, id])
	}

	*getQUuid {
		_ScIDE_GetQUuid
		this.primitiveFailed
	}

	*getTextByQUuid {|quuid, funcID, start = 0, range = -1|
		this.send(\getDocumentText, [quuid, funcID, start, range]);
	}

	*setTextByQUuid {|quuid, funcID, text, start = 0, range = -1|
		this.send(\setDocumentText, [quuid, funcID, text, start, range]);
	}

	*setSelectionByQUuid {|quuid, start, length|
		this.send(\setDocumentSelection, [quuid, start, length]);
	}

	*setEditablebyQUuid {|quuid, editable|
		this.send(\setDocumentEditable, [quuid, editable]);
	}

	*setPromptsToSavebyQUuid {|quuid, prompts|
		this.send(\setDocumentPromptsToSave, [quuid, prompts]);
	}

	*setCurrentDocumentByQUuid {|quuid|
		this.send(\setCurrentDocument, [quuid]);
	}

	*removeDocUndoByQUuid {|quuid|
		this.send(\removeDocUndo, [quuid]);
	}

	*close {|quuid|
		this.send(\closeDocument, [quuid]);
	}

	*setDocumentTitle {|quuid, newTitle|
		this.send(\setDocumentTitle, [quuid, newTitle]);
	}

	*setDocumentKeyDownEnabled {|quuid, bool|
		this.send(\enableDocumentKeyDownAction, [quuid, bool]);
	}

	*setDocumentKeyUpEnabled {|quuid, bool|
		this.send(\enableDocumentKeyUpAction, [quuid, bool]);
	}

	*setDocumentGlobalKeyDownEnabled {|bool|
		this.send(\enableDocumentGlobalKeyDownAction, [bool]);
	}

	*setDocumentGlobalKeyUpEnabled {|bool|
		this.send(\enableDocumentGlobalKeyUpAction, [bool]);
	}

	*setDocumentMouseDownEnabled {|quuid, bool|
		this.send(\enableDocumentMouseDownAction, [quuid, bool]);
	}

	*setDocumentMouseUpEnabled {|quuid, bool|
		this.send(\enableDocumentMouseUpAction, [quuid, bool]);
	}

	*setDocumentTextChangedEnabled {|quuid, bool|
		this.send(\enableDocumentTextChangedAction, [quuid, bool]);
	}

	*setDocumentTextMirrorEnabled {|bool|
		this.send(\enableDocumentTextMirror, [bool]);
	}

	*send { |id, data|
		defer {
			this.prSend(id, data)
		}
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