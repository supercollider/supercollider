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

EmacsInterface
{
	classvar handlers, showClassMethods;

	*initClass {
		handlers = IdentityDictionary.new;
		this.initDefaultHandlers;
		showClassMethods = IdentitySet.new;
		showClassMethods.addAll(#[
			\new, \ar, \kr
		]);
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

	*initDefaultHandlers {
		this
		.put(\symbolTable, { | fileName |
			var result, file;
			var t;

			t = Main.elapsedTime;

			result = IdentitySet.new;
			
			Class.allClasses.do { | class |
				if (class.isMetaClass.not) {
					result.add(class.name);
				};
				class.methods.do { | method |
					result.add(method.name);
				};
			};
			
			file = File(fileName, "w");
			result.collectAs({ |s| s.asString }, Array).storeLispOn(file);
			file.close;

			t = Main.elapsedTime - t;
			Post << "Emacs: Built symbol table in " << t << " seconds\n";

			true
		})
		.put(\classDefinitions, { | name |
			var result, class, files;
			
			if ((class = name.asSymbol.asClass).notNil) {
				files = IdentitySet.new;
				result = result.add([
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
		
			symbol = name.asSymbol;

			Class.allClasses.do { | class |
				class.methods.do { | method |
					if (method.name === symbol) {
						result = result.add([
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
			var references, methods, result;

			references = Class.findAllReferences(name.asSymbol);
			if (references.notNil) {
				methods = IdentitySet.new;
				references.do { | funcDef |
					methods.add(funcDef.homeContext);
				};
				methods.do { | method |
					result = result.add([
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
	}
}

Emacs
{
	classvar outFile, requestHandlers, requestAllocator;
	classvar <menu, <>keys;

	// initialization
	*initClass {
		var outFileName;
		Class.initClassTree(EmacsInterface);
		Class.initClassTree(EmacsDocument);
		requestHandlers = IdentityDictionary.new;
		requestAllocator = StackNumberAllocator(0, 128);
		keys = IdentityDictionary.new;
		outFileName = "SCLANG_COMMAND_FIFO".getenv;
		if (outFileName.isNil) {
			"Emacs: no communication FIFO available.".postln;
		}{
			outFile = File(outFileName, "w");
			UI.registerForShutdown({
				if (outFile.notNil) {
					outFile.close;
					outFile = nil
				};
			});
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
		this.sendToLisp('_eval', expression, handler);
	}

	// utilities
	*readFileName { | handler |
		this.evalLispExpression("(read-file-name \"Enter file name: \")", handler);
	}
}

// EOF