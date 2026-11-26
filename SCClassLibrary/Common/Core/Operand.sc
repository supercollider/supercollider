Operand : AbstractFunction {
	var <>value;

	*new { |value|
		^super.newCopyArgs(value.dereferenceOperand)
	}

	dereferenceOperand {
		^value
	}

	// MATH SUPPORT
	composeUnaryOp { |aSelector|
		^this.class.new(value.perform(aSelector))
	}
	composeBinaryOp { |aSelector, something, adverb|
		^this.class.new(value.perform(aSelector, something, adverb))
	}
	reverseComposeBinaryOp { |aSelector, something, adverb|
		^this.class.new(something.perform(aSelector, value, adverb))
	}
	composeNAryOp { |aSelector, anArgList|
		^this.class.new(value.perform(aSelector, *anArgList))
	}

	// double dispatch for mixed operations
	performBinaryOpOnSimpleNumber { |aSelector, aNumber, adverb|
		^this.class.new(aNumber.perform(aSelector, value, adverb))
	}

	performBinaryOpOnSignal { |aSelector, aSignal, adverb|
		^this.class.new(aSignal.perform(aSelector, value, adverb))
	}

	performBinaryOpOnComplex { |aSelector, aComplex, adverb|
		^this.class.new(aComplex.perform(aSelector, value, adverb))
	}

	performBinaryOpOnSeqColl { |aSelector, aSeqColl, adverb|
		^this.class.new(aSeqColl.perform(aSelector, value, adverb))
	}

	hash { |obj|
		^this.instVarHash([\value])
	}

	== { |obj|
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
