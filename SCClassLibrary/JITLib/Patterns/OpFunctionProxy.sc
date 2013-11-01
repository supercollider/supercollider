UnaryOpFunctionProxy : UnaryOpFunction {

	valueFuncProxy { arg args;
		^this.reduceFuncProxy(args)
	}
	reduceFuncProxy { arg args;
		^a.reduceFuncProxy(args).perform(selector)
	}
	value { arg ... args;
			^this.reduceFuncProxy(args)
	}
	valueArray { arg args;
			^this.reduceFuncProxy(args)
	}


	composeUnaryOp { arg aSelector;
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { arg aSelector, anArgList;
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}

	// behave like a pattern
	embedInStream { arg inval;
		^this.value.embedInStream(inval)
	}



}
BinaryOpFunctionProxy : BinaryOpFunction {

	valueFuncProxy { arg args;
		^this.reduceFuncProxy(args)
	}
	reduceFuncProxy { arg args;
		^a.reduceFuncProxy(args)
			.perform(selector, b.reduceFuncProxy(args), adverb)
	}
	value { arg ... args;
			^this.reduceFuncProxy(args)
	}
	valueArray { arg args;
			^this.reduceFuncProxy(args)
	}


	composeUnaryOp { arg aSelector;
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { arg aSelector, anArgList;
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}

	// behave like a pattern
	embedInStream { arg inval;
		^this.value.embedInStream(inval)
	}

}
NAryOpFunctionProxy : NAryOpFunction {

	reduceFuncProxy { arg args;
		^a.reduceFuncProxy(args).performList(selector, arglist.collect(_.reduceFuncProxy(args)))
	}
	valueFuncProxy { arg args;
		^this.reduceFuncProxy(args)
	}
	value { arg ... args;
			^this.reduceFuncProxy(args)
	}
	valueArray { arg args;
			^this.reduceFuncProxy(args)
	}


	composeUnaryOp { arg aSelector;
		^UnaryOpFunctionProxy.new(aSelector, this)
	}
	composeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, this, something, adverb);
	}
	reverseComposeBinaryOp { arg aSelector, something, adverb;
		^BinaryOpFunctionProxy.new(aSelector, something, this, adverb);
	}
	composeNAryOp { arg aSelector, anArgList;
		^NAryOpFunctionProxy.new(aSelector, this, anArgList)
	}

	// behave like a pattern
	embedInStream { arg inval;
		^this.value.embedInStream(inval)
	}

}

// maybe make it an abstract function object.
NAryValueProxy : NAryOpFunctionProxy {
	*new { arg receiver, args;
		^super.new(nil, receiver, args ? [])
	}
	reduceFuncProxy { arg args;
		^a.reduceFuncProxy(arglist.collect(_.reduceFuncProxy(args)))
	}
	storeOn { arg stream;
		stream << "o(" <<< a << "," <<<* arglist << ")" // is it always so?
	}
}
