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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA

EmacsSlot {
	var action;

	*new { arg anObject, aFunction;
		var slot;
		slot = this.newCopyArgs(aFunction);
		anObject.addDependant(slot);
		^slot
	}

	update { arg who, what;
		action.value(what);
	}
}

EmacsInterface {
	symbolTable {
		var result;
		result = IdentitySet.new;
		Class.allClasses.do { arg class;
			result.add(class.name);
			class.methods.do { arg method;
				result.add(method.name);
			};
		};
		^result.collectAs({ arg symbol; symbol.asString }, Array)
	}

	classDefinitions { arg name;
		var result, class, files;

		result = List.new;
		class = name.asSymbol.asClass;

		if (class.notNil) {
			files = IdentitySet.new;
			result.add(["  " ++ name, class.filenameSymbol.asString, class.charPos + 1]);
			files.add(class.filenameSymbol);
			class.methods.do { arg method;
				if (files.includes(method.filenameSymbol).not) {
					result.add(["+ " ++ name, method.filenameSymbol.asString, nil]);
					files.add(method.filenameSymbol);
				}
			}
		};
		
		^name -> result
	}

	methodDefinitions { arg name;
		var result, symbol, getter, setter;
		
		result = List.new;
		symbol = name.asSymbol;
		getter = symbol.asGetter;
		setter = symbol.asSetter;

		Class.allClasses.do { arg class;
			class.methods.do { arg method;
				if ((method.name === getter).or { method.name === setter }) {
					result.add([
						class.name ++ "-" ++ name,
						method.filenameSymbol.asString,
						method.charPos + 1
					])
				}
			}
		};
		
		^name -> result
	}

	methodReferences { arg name;
		var references, methods, result;

		references = Class.findAllReferences(name.asSymbol);
		if (references.notNil) {
			methods = IdentitySet.new;
			references.do { arg funcDef;
				methods.add(funcDef.homeContext);
			};
			result = List.new;
			methods.do { arg method;
				result.add([
					method.ownerClass.name.asString ++ "-" ++ method.name.asString,
					method.filenameSymbol.asString,
					method.charPos + 1
				])
			}
		};

		^name -> result
	}
}

Emacs {
	classvar outFile, <interface, <lib, requestHandlers, requestAllocator, slots, <menu, <>keys;

	// initialization
	*initClass {
		interface = EmacsInterface.new;
		lib = IdentityDictionary.new;
// 		varLib = IdentityDictionary.new;
		requestHandlers = IdentityDictionary.new;
		requestAllocator = LRUNumberAllocator(0, 1024);
		slots = List.new;
		// menu = EmacsMenu.new;
		keys = IdentityDictionary.new;
		Class.initClassTree(Server);
	}

	*initMenu {
		menu.put([\Library, \Main, \run], "Run Main", { thisProcess.run });
		menu.put([\Library, \Main, \stop], "Stop Main", { thisProcess.stop });
	}

	*initSlots {
		slots.add(
			EmacsSlot(Server, {
				Server.set.do { arg server;
					EmacsSlot(server, { this.updateServer });
				};
				Server.resumeThreads;
				this.updateServer;
			})
		);
		slots.add(
			EmacsSlot(menu, {
				this.sendToLisp(\_updateMenu, menu)
			})
		);
	}

	// Lisp interface
	*lispInit { arg outFileName;
		outFile = File(outFileName, "w");
		this.sendToLisp(\_init);
		// this.initMenu;
		this.initSlots;
		slots.doMsg(\update);
	}

	*lispPerformCommand { arg cmdName, args;
		var result;
		if (interface.respondsTo(cmdName)) {
			result = interface.performList(cmdName, args ? []);
			this.sendToLisp(cmdName, result);
		}
		^result
	}

	*lispHandleCommandResult { arg id, obj;
		var handler;
		handler = requestHandlers.at(id);
		if (handler.notNil) {
			requestAllocator.free(id);
			handler.value(obj);
		}
	}

	// sclang interface
	*sendToLisp { arg handlerName, obj;
		if (outFile.notNil) {
			(handlerName.asSymbol -> obj).storeLispOn(outFile);
			outFile.nl.flush;
		}
		^obj
	}

	*evalLispExpression { arg expression, handlerFunc;
		var id;
		if (handlerFunc.notNil) {
			id = requestAllocator.alloc;
			if (id.notNil) {
				requestHandlers.put(id, handlerFunc);
			}
		};
		this.sendToLisp('_eval', (id -> expression));
	}

	*updateServer {
		var result;
		Server.set.do { arg server;
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
						((server.avgCPU ? 0.0) * 100.0).round.clip(0, 100),
						((server.peakCPU ? 0.0) * 100.0).round.clip(0, 100),
						server.numUGens ? 0,
						server.numSynths ? 0,
						server.numGroups ? 0,
						server.numSynthDefs ? 0
					]
				]);
		};
		this.sendToLisp(\_updateServer, result)
	}

	*readFileName { arg handler;
		this.evalLispExpression("(read-file-name \"Enter file name: \")", handler);
	}

	// private interface
	*prAddSlot { arg slot;
		slots.add(slot);
	}
}

// EOF