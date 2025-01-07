UnaryOpFunctionProxy : UnaryOpFunction {

	valueFuncProxy { | args |
		^this.reduceFuncProxy(args)
	}
	reduceFuncProxy { | args |
		^a.reduceFuncProxy(args).perform(selector)
	}
	value { | ... args |
		^this.reduceFuncProxy(args)
	}
	valueArray { | args |
		^this.reduceFuncProxy(args)
	}

	composeUnaryOp { | aSelector |
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { | aSelector, something, adverb |
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { | aSelector, something, adverb |
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { | aSelector, anArgList |
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}

	// behave like a pattern
	embedInStream { | inval |
		^this.value.embedInStream(inval)
	}

}

BinaryOpFunctionProxy : BinaryOpFunction {

	valueFuncProxy { | args |
		^this.reduceFuncProxy(args)
	}
	reduceFuncProxy { | args |
		^a.reduceFuncProxy(args)
		.perform(selector, b.reduceFuncProxy(args), adverb)
	}
	value { | ... args |
		^this.reduceFuncProxy(args)
	}
	valueArray { | args |
		^this.reduceFuncProxy(args)
	}

	composeUnaryOp { | aSelector |
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { | aSelector, something, adverb |
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { | aSelector, something, adverb |
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { | aSelector, anArgList |
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}

	// behave like a pattern
	embedInStream { | inval |
		^this.value.embedInStream(inval)
	}

}

NAryOpFunctionProxy : NAryOpFunction {

	reduceFuncProxy { | args |
		^a.reduceFuncProxy(args).performList(selector, arglist.collect(_.reduceFuncProxy(args)))
	}
	valueFuncProxy { | args |
		^this.reduceFuncProxy(args)
	}
	value { | ... args |
		^this.reduceFuncProxy(args)
	}
	valueArray { | args |
		^this.reduceFuncProxy(args)
	}

	composeUnaryOp { | aSelector |
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { | aSelector, something, adverb |
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { | aSelector, something, adverb |
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { | aSelector, anArgList |
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}

	// behave like a pattern
	embedInStream { | inval |
		^this.value.embedInStream(inval)
	}

}

// maybe make it an abstract function object.
NAryValueProxy : NAryOpFunctionProxy {

	*new { | receiver, args |
		^super.new(nil, receiver, args ? [])
	}
	reduceFuncProxy { | args |
		^a.reduceFuncProxy(arglist.collect(_.reduceFuncProxy(args)))
	}
	storeOn { | stream |
		stream << "o(" <<< a << "," <<<* arglist << ")" // is it always so?
	}
}
