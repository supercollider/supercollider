QMethodNotFoundError : MethodError {
	var <>method, <>arguments;

	*new { |receiver, method, arguments|
		^super.new("Failed to invoke Qt method", receiver).method_(method).arguments_(arguments);
	}

	reportError {
		var potentialMethods;

		if(receiver.isKindOf(QObject).not) {
			^this.prBad("Error came from a non-QObject object, something is very wrong!");
		};

		potentialMethods = receiver.class.meta.methods;
		potentialMethods = potentialMethods.select(_.beginsWith(method.asString ++ "("));
		post("Invocation of a Qt method was attempted, but the meta-object system failed to find it.\n");
		if(potentialMethods.isEmpty()) {
			postf("No method named '%' could be found.\n", method);
			postf("Use `%.meta.methods` to get a list of invokable methods.\n", receiver.class);
		} {
			if(potentialMethods.size() == 1) {
				post("Perhaps you meant to invoke this function, which requires the indicated types:\n");
				postf("\t%\n", this.prTranslateSignature(potentialMethods[0]));
			} {
				post("Perhaps you meant to invoke one of these functions, which require the indicated types:\n");
				potentialMethods.collect(this.prTranslateSignature(_)).do("\t%\n".postf(_));
			}
		};
	}

	// translate Qt method signature to SC
	prTranslateSignature { |sig|
		var methodName, paramList, params, translatedParams;
		var split = sig.drop(-1).split($();
		methodName = split[0];
		paramList = split[1];
		if(paramList.notEmpty) {
			params = paramList.split($,);
			translatedParams = params.collect(this.prTranslateType(_));
		} {
			translatedParams = [];
		}

		^"%(%)".format(methodName, translatedParams.join(", "))
	}

	// translate type name from Qt/C++ to SC
	// TODO: use C++ metatype system to make translations somehow
	prTranslateType { |type|
		^type
	}

	// Early exit to avoid infinite throwing
	prBad { |str|
		"ERROR: %\n".postf(str);
		^nil
	}

}
