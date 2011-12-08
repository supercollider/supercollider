// copyright 2003 stefan kersten <steve@k-hornz.de>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
// USA

EmacsInterface {
	classvar handlers;

	*initClass {
		handlers = IdentityDictionary.new;
		this.initDefaultHandlers;
	}

	*put { | name, function |
		handlers.put(name.asSymbol, function);
	}
	*at { | name |
		^handlers.at(name)
	}

	*performCommand { | name, args |
// 		[\performCommand, name, args].postln;
		^handlers.atFail(name, { ^nil }).valueArray(args)
	}

	*makeSubListSorter { | index selector('<') |
		^{ | a b | a[index].perform(selector, b[index]) }
	}

	*initDefaultHandlers {
		this
		.put(\symbolTable, { | fileName |
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

			"Emacs: Built symbol table in % seconds\n".postf(dt.asStringPrec(3));

			result
		})
		.put(\openDefinition, { | name |
			var class, method, res;
			#class, method = name.split($-);
			class = class.asSymbol.asClass;
			if (class.notNil) {
				if (method.isNil) {
					res = [class.filenameSymbol.asString, class.charPos + 1];
				} {
					method = class.findMethod(method.asSymbol);
					if (method.notNil) {
						res = [method.filenameSymbol.asString, method.charPos + 1];
					};
				};
			};
			name -> res
		})
		.put(\classDefinitions, { | name |
			var result, class, files;

			result = SortedList(8, this.makeSubListSorter(0, '<'));

			if ((class = name.asSymbol.asClass).notNil) {
				files = IdentitySet.new;
				result.add([
					"  " ++ name,
					class.filenameSymbol.asString,
					class.charPos + 1
				]);
				files.add(class.filenameSymbol);
				class.methods.do { | method |
					if (files.includes(method.filenameSymbol).not) {
						result = result.add([
							"+ " ++ name,
							method.filenameSymbol.asString,
							method.charPos + 1
						]);
						files.add(method.filenameSymbol);
					}
				}
			};

			name -> result
		})
		.put(\methodDefinitions, { | name |
			var result, symbol, getter, setter;

			result = SortedList(8, this.makeSubListSorter(0, '<'));
			symbol = name.asSymbol;

			Class.allClasses.do { | class |
				class.methods.do { | method |
					if (method.name === symbol) {
						result.add([
							class.name ++ "-" ++ name,
							method.filenameSymbol.asString,
							method.charPos + 1
						])
					}
				}
			};

			name -> result
		})
		.put(\methodReferences, { | name |
			var result, references, methods;

			result = SortedList(8, this.makeSubListSorter(0, '<'));
			references = Class.findAllReferences(name.asSymbol);

			if (references.notNil) {
				methods = IdentitySet.new;
				references.do { | funcDef |
					var homeContext;
					homeContext = funcDef.homeContext;
					if (homeContext.isKindOf(Method)) {
						methods.add(homeContext);
					};
				};
				methods.do { | method |
					result.add([
						method.ownerClass.name ++ "-" ++ method.name,
						method.filenameSymbol.asString,
						method.charPos + 1
					])
				}
			};

			name -> result
		})
		.put(\methodArgs, { | className, methodName |
			var stream, class, method;
			var args, varArgs, lastIndex;

			stream = CollStream.new;
			class = className.asSymbol.asClass;

			if (class.notNil) {
				method = class.class.findRespondingMethodFor(methodName.asSymbol);
				if (method.notNil and: { method.argNames.notNil }) {
					args = method.argNames.copyToEnd(1);
					varArgs = method.varArgs;
					lastIndex = args.lastIndex;

					args.do({ | name, i |
						var default;
						if (varArgs and: { i == lastIndex }) {
							stream << " ... ";
						}{
							if (i != 0) { stream << ", " };
						};
						stream << name;
						default = method.prototypeFrame[i+1];
						if (default.notNil) {
							stream << "=" << default;
						};
					});
				}
			};

			stream.collection;
		})
		.put('evalSCLang', #{|expr, time|
			{[time, \ok, expr.interpret]}
			.try{|e|
				[time, \error, e.errorString]
			}
		})
		.put( \openDevSource, { | name |
			var devpath = thisProcess.platform.devLoc( name );
			//	devpath.postln;
			Document.open( devpath ).front;
			name -> devpath
		})
		.put( \helpSymbols, {
			var result, dt;

			dt = {
				result = IdentitySet.new(16384);
				SCDoc.helpSourceDirs.do {|dir|
					("find -L"+dir.escapeChar($ )+"-type f -name '*.schelp' -not -name '*.ext.schelp'")
					.unixCmdGetStdOutLines.do {|file|
						result.add(file[dir.size+1 ..].drop(-7))
					}
				};

				Class.allClasses.do { | class |
					if (class.isMetaClass.not) {
						result.add(class.name.asString);
					};
					class.methods.do { | method |
						result.add(method.name.asString);
					};
				};

				result = result.asArray
			}.bench(false);
			"Emacs: Index help topics in % seconds\n".postf(dt.asStringPrec(3));
			result
		});
	}
}

Emacs {
	classvar outStream, outFile, requestHandlers, requestAllocator;
	classvar <menu, <>keys;
	classvar <initialized = false;

	// initialization
	*initClass {
		var outFileName, newServer;
		Class.initClassTree(EmacsInterface);
		Class.initClassTree(EmacsDocument);
		Class.initClassTree(AbstractResponderFunc);
		Class.initClassTree(OSCresponder);
		Class.initClassTree(Server);

		Platform.makeServerWindowAction = {|aServer, aWindow| aServer.makeEmacsWindow };
		Platform.makeSynthDescWindowAction = {|aSynthDesc| aSynthDesc.makeEmacsWindow };
		Platform.openHelpFileAction = {|aString| aString.openHelpFileEmacs };
		Platform.openHTMLFileAction = {|aString| aString.openHTMLFileEmacs };

		requestHandlers = IdentityDictionary.new;
		requestAllocator = StackNumberAllocator(0, 128);
		keys = IdentityDictionary.new;
		outFileName = "SCLANG_COMMAND_FIFO".getenv;
		if (outFileName.isNil) {
			"Emacs: No communication FIFO available.".postln;
		}{
			initialized = true;
			Document.implementationClass = ScelDocument;
			StartUp.add( { Document.implementationClass.startup; } );
			thisProcess.platform.declareFeature( \emacs );
			outStream = CollStream.on(String.new);
			outFile = File(outFileName, "w");
			ShutDown.add {
				Emacs.evalLispExpression( "(sclang-on-library-shutdown)" );
				if (outFile.notNil) {
					outFile.close;
					outFile = nil
				};
			};
			// initialize servers
			newServer = { | server update |
				SimpleController(server)
				.put(\serverRunning, { this.updateServer })
				.put(\counts, { this.updateServer });
				server.startAliveThread;
				if (update) { this.updateServer };
			};
			SimpleController(Server)
				.put(\serverAdded, { | serverClass what server | newServer.value(server, true) });
			Server.named.do(newServer.value(_, false));
			this.updateServer;
			// initialize lisp
			"Emacs: Initializing lisp interface.".postln;
			this.sendToLisp(\_init);
		};
	}

	// lisp interface
	*lispPerformCommand { | cmdName, args, send |
		var result;
		result = EmacsInterface.performCommand(cmdName, args);
		if (send) {
			this.sendToLisp(cmdName, result);
		};
		^result
	}
	*lispHandleCommandResult { | id, obj |
		var handler;
		handler = requestHandlers.at(id);
		if (handler.notNil) {
			requestAllocator.free(id);
			handler.value(obj);
		}
	}

	// sclang interface
	*sendToLisp { | cmd, obj, handler=nil |
		var id, str;
		if (outFile.notNil) {
			if (handler.notNil) {
				id = requestAllocator.alloc;
				if (id.notNil) {
					requestHandlers.put(id, handler);
				};
			};
			str = [cmd.asSymbol, obj, id].asLispString;
			outFile.putInt32(str.size);
			outFile.write(str);
			outFile.flush;
		}
	}
	*evalLispExpression { | expression, handler |
		this.sendToLisp('_eval', expression.asLispExpression, handler);
	}

	// utilities
	*readFileName { | handler |
		this.evalLispExpression("(read-file-name \"Enter file name: \")", handler);
	}
	*message { | format ... args |
		this.evalLispExpression(([\message, format] ++ args).asLispString);
	}

	*updateServer {
		var result;
		result = [Server.default.name];
		Server.named.do { arg server;
			result = result.add(
				server.name -> [
					'running', server.serverRunning,
					'type',
					if (server.inProcess) {\internal} {if (server.isLocal) {\local} {\remote}},
					'address', server.addr.ip,
					'client-id', server.clientID,
					'latency', server.latency,
					'notified', server.notified,
					'dump-mode', server.dumpMode,
					'info', [
						(server.avgCPU ? 0.0).round(0.1),
						(server.peakCPU ? 0.0).round(0.1),
						server.numUGens ? 0,
						server.numSynths ? 0,
						server.numGroups ? 0,
						server.numSynthDefs ? 0
					]
				]);
		};
		this.sendToLisp(\_updateServer, result)
	}
}

// EOF