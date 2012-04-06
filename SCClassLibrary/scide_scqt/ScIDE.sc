ScIDE {
	classvar subListSorter;

	*initClass {
		subListSorter = { | a b | a[0].perform('<', b[0]) };
	}

	*connect {|ideName|
		_ScIDE_Connect
		this.primitiveFailed
	}

	*sendAllClasses {
		this.prSend(\classes, Class.allClasses.collectAs(_.asString, Array))
	}

	*sendSymbolTable {
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

		this.prSend(\symbolTable, result)
	}

	*getClassDefinitions {|name|
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

		this.prSend(\classDefinitions, result.asArray)
	}

	*prSend {|selector, data|
		_ScIDE_Send
		this.primitiveFailed
	}
}
