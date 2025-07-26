+ Object {
	valueFuncProxy {
		^this
	}

	reduceFuncProxy {
		^this
	}

	postString {
		^this.asString
	}
}

+ AbstractFunction {
	reduceFuncProxy { | args, protect=true, kwargs |
		^this.valueArgs(args, kwargs).valueFuncProxy(args, kwargs)
	}

}

+ Function {
	postString {
		^this.asCompileString
	}
}

+ UnaryOpFunctionProxy {
	postString {
		^a.postString ++ "." ++ selector
	}
}

+ BinaryOpFunctionProxy {
	postString {
		^a.postString + selector.asBinOpString + b.postString
	}
}
