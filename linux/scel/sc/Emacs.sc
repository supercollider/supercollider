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

EmacsInterface {
	symbolTable {
		var result;

		"Emacs: Building symbol table ...".post;

		result = IdentitySet.new;

		Class.allClasses.do { | class |
			result.add(class.name);
			class.methods.do { | method |
				result.add(method.name);
			};
		};

		result = result.collectAs({ | symbol | symbol.asString }, Array);

		" done.".postln;

		^result
	}

	classDefinitions { | name |
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
		
		^name -> result
	}

	methodDefinitions { | name |
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
		
		^name -> result
	}

	methodReferences { | name |
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

		^name -> result
	}
}

Emacs {
	classvar outFile, <interface;
	classvar requestHandlers, requestAllocator;
	classvar <menu, <>keys;

	// initialization
	*initClass {
		var outFileName;
		interface = EmacsInterface.new;
		requestHandlers = IdentityDictionary.new;
		requestAllocator = LRUNumberAllocator(0, 1024);
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
	*lispPerformCommand { | cmdName, args |
		var result;
		if (interface.respondsTo(cmdName)) {
			result = interface.performList(cmdName, args);
			this.sendToLisp(cmdName, result);
		}
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
	*sendToLisp { | cmdName, obj |
		var str;
		if (outFile.notNil) {
			str = (cmdName.asSymbol -> obj).asLispString;
			outFile.putInt32(str.size);
			outFile.write(str);
			outFile.flush;
		}
	}
	*evalLispExpression { | expression, handler |
		var id;
		if (handler.notNil) {
			id = requestAllocator.alloc;
			if (id.notNil) {
				requestHandlers.put(id, handler);
			}
		};
		this.sendToLisp('_eval', (id -> expression));
	}

	// utilities
	*readFileName { | handler |
		this.evalLispExpression("(read-file-name \"Enter file name: \")", handler);
	}
}

// EOF