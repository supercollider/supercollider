ScIDE {
	classvar subListSorter;

	*initClass {
		subListSorter = { | a b | a[0].perform('<', b[0]) };
	}

	*connect {|ideName|
		_ScIDE_Connect
		this.primitiveFailed
	}

	*request { |id, command, data|
		this.tryPerform(command, id, data);
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

	*sendClassDefinitions { |id, name|
		var result, class, files;

		result = SortedList(8, subListSorter);

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

		this.prSend(id, result.asArray)
	}

	*sendMethodDefinitions { |id, name|
		var out;
		if (name.isEmpty) {
			^this;
		};
		if (name[0].toLower != name[0]) {
			// This is not a valid method name
			^this;
		};
		out = [];
		name = name.asSymbol;
		Class.allClasses.do({ arg class;
			class.methods.do({ arg method;
				var signature;
				var definition;
				if (method.name == name, {
					signature = [class.name, name];
					if (method.argNames.size > 1) {
						signature = signature ++ method.argNames[1..];
					};
					definition = [method.filenameSymbol.asString, method.charPos];
					out = out.add( [signature, definition] );
				});
			});
		});

		this.prSend(id, out);
	}

	*prSend {|id, data|
		_ScIDE_Send
		this.primitiveFailed
	}
}
