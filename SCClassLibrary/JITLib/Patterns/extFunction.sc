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
	reduceFuncProxy { arg args;
		^this.valueArray(args).valueFuncProxy(args)
	}

}

+ Function {
	postString {
		^this.asCompileString
	}
}
