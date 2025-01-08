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
		^a.reduceFuncProxy(args, protect, kwargs).performList(selector, arglist.collect(_.reduceFuncProxy(args, protect, kwargs)))
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
	composeNAryOp { | aSelector, anArgList |
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}

	// behave like a pattern
	embedInStream { | inval |
		^this.value.embedInStream(inval)
	}

}

// this proxy is built on function proxiy composition
NAryValueProxy : NAryOpFunctionProxy {

	*new { | receiver, args |
		^super.new(nil, receiver, args ? [])
	}
	reduceFuncProxy { | args, protect = true, kwargs |
		// kwargs are only passed to each proxy: keyword compositon isn't supported
		^a.reduceFuncProxy(
			arglist.collect(_.reduceFuncProxy(args, protect, kwargs))
		)
	}
	storeOn { | stream |
		stream << "o(" <<< a << "," <<<* arglist << ")"
	}
}
