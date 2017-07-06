Operand : AbstractFunction {
	var <>value;

	*new { |value|
		^super.newCopyArgs(value.dereferenceOperand)
	}

	dereferenceOperand {
		^value
	}

	// MATH SUPPORT
	composeUnaryOp { arg aSelector;
		^this.class.new(value.perform(aSelector))
	}
	composeBinaryOp { arg aSelector, something, adverb;
		^this.class.new(value.perform(aSelector, something, adverb))
	}
	reverseComposeBinaryOp { arg aSelector, something, adverb;
		^this.class.new(something.perform(aSelector, value, adverb))
	}
	composeNAryOp { arg aSelector, anArgList;
		^this.class.new(value.perform(aSelector, *anArgList))
	}

	// double dispatch for mixed operations
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		^this.class.new(aNumber.perform(aSelector, value, adverb))
	}

	performBinaryOpOnSignal { arg aSelector, aSignal, adverb;
		^this.class.new(aSignal.perform(aSelector, value, adverb))
	}

	performBinaryOpOnComplex { arg aSelector, aComplex, adverb;
		^this.class.new(aComplex.perform(aSelector, value, adverb))
	}

	performBinaryOpOnSeqColl { arg aSelector, aSeqColl, adverb;
		^this.class.new(aSeqColl.perform(aSelector, value, adverb))
	}

	hash { arg obj;
		^this.instVarHash([\value])
	}

	== { arg obj;
		^this.compareObject(obj, [\value])
	}

	printOn { |stream|
		stream << this.class.name << "(";
		value !? { stream << value };
		stream << ")"
	}

	storeOn { |stream|
		stream << this.class.name << "(";
		value !? { stream <<< value };
		stream << ")"
	}

}
