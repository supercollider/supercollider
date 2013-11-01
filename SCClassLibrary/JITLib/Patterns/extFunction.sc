+Object {
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

+AbstractFunction {
	reduceFuncProxy { arg args;
		^this.valueArray(args).valueFuncProxy(args)
	}

}

+Function {
	postString {
		^this.asCompileString
	}
}
+UnaryOpFunctionProxy {
	postString {
		^a.postString ++ "." ++ selector
	}
}
+BinaryOpFunctionProxy {
	postString {
		^a.postString + selector.asBinOpString + b.postString
	}
}
