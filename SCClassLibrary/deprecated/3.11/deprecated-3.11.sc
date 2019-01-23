+ AbstractFunction {
	asInt {
		this.deprecated(thisMethod, this.class.findMethod(\asInteger));
		^this.composeUnaryOp('asInteger');
	}
}

+ Object {
	asInt {
		this.deprecated(thisMethod, this.class.findMethod(\asInteger));
		^this.asInteger;
	}
}
