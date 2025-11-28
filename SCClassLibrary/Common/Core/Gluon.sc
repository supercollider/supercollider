PRGluonCallbackWrapper {
	var func;
	*new { |f| ^super.newCopyArgs(func: f) }
	value { |...args, kwargs| func.performArgs(\value, args, kwargs) }
}

Gluon {
	classvar openLibraries;
	var <id;

	*initClass { openLibraries = () }
	*new { Error("Do not create an Gluon manually, use Gluon.open or Gluon.with").throw }
	*prNew { |id| ^super.newCopyArgs(id) }

	*inbuilt { |name|
		// inbuilt library do not need to be closed.
		var id = Gluon.prInbuilt(name);
		^Gluon.prNew(id);
	}

	*prInbuilt { |name|
		_GetInbuiltGluonID
		^this.primitiveFailed;
	}

	*open { |pathToLibrary ...args|
		var id, lib;
		if (PathName(pathToLibrary).isFile.not) {
			Error("Could not find gluon at path: " ++ pathToLibrary).throw
		} {
			id = Gluon.prOpenGetID(pathToLibrary.asString, *args);
			lib = Gluon.prNew(id);
			openLibraries[lib.id] = lib;
			^lib
		}
	}

	*close { |library|
		if (library.isKindOf(Gluon).not) {
			Error("Can only close an Gluon").throw
		};
		Gluon.prCloseLibrary(library.id);
		openLibraries[library.id] = nil;
	}

	*with { |pathToLibrary, func|
		var l = Gluon.open(pathToLibrary);
		protect {
			func.(l);
		} {
			Gluon.close(l);
		}
	}

	*prOpenGetID { |pathToLibrary ...args|
		_OpenGluon
		^this.primitiveFailed;
	}

	*prCloseLibrary { |id|
		_CloseGluon
		^this.primitiveFailed;
	 }

	close { ^Gluon.close(this) }

	value { |functionNameSymbol ...args, kwargs|
		if (kwargs.size() == 2 and: {kwargs[0] == \callback}) {
			^this.prValueWithCallback(id, functionNameSymbol, PRGluonCallbackWrapper(kwargs[1]), *args);
		} {
			if (kwargs.size() != 0) {
				Error("Cannot call a gluon ffi function with keyword arguments").throw
			}
			^this.prValue(id, functionNameSymbol, *args);
		}
	}

	prValue { |library_id, functionNameSymbol ...args|
		_CallGluon
		^this.primitiveFailed;
	}

	prValueWithCallback { |library_id, functionNameSymbol, callback ...args|
		_CallGluonWithCallback
		^this.primitiveFailed;
	}


	doesNotUnderstand { |selector... args, kwargs|
		^this.performArgs(\value, [selector] ++ args, kwargs);
	}
}
