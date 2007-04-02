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

}