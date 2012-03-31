ScIDE {
	*connect {|ideName|
		_SCIdeConnect
		this.primitiveFailed
	}

	*getAllClasses {
		this.prSend(\classes, Class.allClasses.collectAs(_.asString, Array))
	}

	*getSymbolTable {
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

	*prSend {|selector, data|
		_SCIdeSend
		this.primitiveFailed
	}
}
