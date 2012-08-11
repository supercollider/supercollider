ScIDE {
	classvar subListSorter;

	*initClass {
		subListSorter = { | a b | a[0].perform('<', b[0]) };

		SimpleController(Server.default)
		.put(\serverRunning, { | server, what, extraArg |
			this.prSend(\defaultServerRunning, server.serverRunning)
		});
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

	*completeClass { |id, text|
		var out = [];
		Class.allClasses.do { |class|
			var name = class.name.asString;
			if (name.beginsWith(text)) {
				out = out.add(name);
			};
		};
		if (out.size > 0) {
			this.prSend(id, out);
		};
	}

	*completeClassMethod { |id, text|
		var class, methods, out;
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
			out = methods.values.collect { |m| this.serializeMethod(m) };
			if (out.size > 0) { this.prSend(id, out) };
		}
	}

	*completeMethod { |id, text|
		var out;
		out = [];
		Class.allClasses.do { |class|
			class.methods.do { |method|
				var signature;
				var definition;
				if (method.name.asString.beginsWith(text)) {
					out = out.add( this.serializeMethod(method) );
				};
			};
		};
		if (out.size > 0) { this.prSend(id, out) };
	}

	*findMethod { |id, text|
		var cname, mname, tokens, out;
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
			out = [];
			this.allMethodsDo { |method|
				if (method.name.asString == mname) {
					out = out.add( this.serializeMethod(method) );
				};
			};
			if (out.size > 0) {
				this.prSend(id, out)
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

	*allMethodsDo { arg func;
		Class.allClasses.do { |class|
			class.methods.do { |method|
				func.value(method);
			};
		};
	}

	*prSend {|id, data|
		_ScIDE_Send
		this.primitiveFailed
	}
}
