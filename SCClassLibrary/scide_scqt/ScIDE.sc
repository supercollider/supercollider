ScIDE {
	classvar subListSorter;
	classvar <>currentPath;
	classvar <defaultServer;
	classvar serverController;
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
		});
		defaultServer = server;
		this.prSend(\defaultServerRunningChanged, [
			server.serverRunning, server.addr.hostname, server.addr.port]);
	}

	*request { |id, command, data|
		this.tryPerform(command, id, data);
	}

	*connected {
		_ScIDE_Connected
	}

	*open { |path, charPos = 0, selectionLength = 0|
		this.prSend(\openFile, [path, charPos, selectionLength])
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
			this.prSend("openHelpUrl", processedUrl)
		});
	}

	*cmdPeriod { docRoutine.play(AppClock) }

	*processUrl { |url, doneAction, brokenAction|
		// NOTE: Copied and modified from HelpBrower:-goTo

		var newPath;

		brokenAction = brokenAction ? {SCDoc.helpTargetDir++"/BrokenLink.html#"++url};

		if (docRoutine.notNil) { docRoutine.stop };

		docRoutine = Routine {
			try {
				url = SCDoc.prepareHelpForURL(url) ?? brokenAction;
				newPath = url.findRegexp("(^\\w+://)?([^#]+)(#.*)?")[1..].flop[1][1];

				// detect old helpfiles and open them in OldHelpWrapper
				if(url.beginsWith("sc://")) {
					url = SCDoc.findHelpFile(newPath);
				} {
					if(
						/*
						// this didn't work for quarks due to difference between registered old help path
						// and the quarks symlink in Extensions.
						// we could use File.realpath(path) below but that would double the execution time,
						// so let's just assume any local file outside helpTargetDir is an old helpfile.
						block{|break|
						Help.do {|key, path|
						if(url.endsWith(path)) {
						break.value(true)
						}
						}; false
						}*/
						newPath.beginsWith(SCDoc.helpTargetDir).not and: { url.beginsWith("file://") }
					) {
						url = HelpBrowser.getOldWrapUrl(url)
					};
				};

				doneAction.value(url);
			} {|err|
				err.throw;
			};
			CmdPeriod.remove(this);
			docRoutine = nil;
		}.play(AppClock);
		CmdPeriod.add(this);
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
