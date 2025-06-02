UnaryOpFunctionProxy : UnaryOpFunction {

	valueFuncProxy { | args, kwargs |
		^this.reduceFuncProxy(args, true, kwargs)
	}
	reduceFuncProxy { | args, protect=true, kwargs |
		^a.reduceFuncProxy(args, protect, kwargs).perform(selector)
	}
	value { | ... args, kwargs |
		^this.reduceFuncProxy(args, true, kwargs)
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

	valueFuncProxy { | args, kwargs |
		^this.reduceFuncProxy(args, true, kwargs)
	}
	reduceFuncProxy { | args, protect=true, kwargs |
		^perform(a.reduceFuncProxy(args, protect, kwargs), selector, b.reduceFuncProxy(args, protect, kwargs), adverb)
	}
	value { | ... args, kwargs |
		^this.reduceFuncProxy(args, true, kwargs)
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

	reduceFuncProxy { | args, protect = true, kwargs |
		^a.reduceFuncProxy(args, protect, kwargs).performArgs(
			selector,
			arglist.collect(_.reduceFuncProxy(args, protect, kwargs)),
			kwarglist.collect(_.reduceFuncProxy(args, protect, kwargs))
		)
	}
	valueFuncProxy { | args |
		^this.reduceFuncProxy(args)
	}
	value { | ... args, kwargs |
		^this.reduceFuncProxy(args, true, kwargs)
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
	composeNAryOp { | aSelector, anArgList, aKeywordArgList |
		^NAryOpFunctionProxy.new(aSelector, this, anArgList, aKeywordArgList)
	}

	// behave like a pattern
	embedInStream { | inval |
		^this.value.embedInStream(inval)
	}

}

// this proxy is built on function proxy composition

NAryValueProxy : NAryOpFunctionProxy {

	*new { | receiver, args |
		^super.new(nil, receiver, args ? [])
	}
	reduceFuncProxy { | args, protect = true, kwargs |

		// kwargs are only passed to each proxy individually:
		// in function compositon we have no way to specify a keyword
		// to which argument of the next function the output value
		// of the previous function in the chain is passed

		^a.reduceFuncProxy(
			arglist.collect(_.reduceFuncProxy(args, protect, kwargs))
		)
	}
	storeOn { | stream |
		stream << "o(" <<< a << "," <<<* arglist << ")"
	}
}
