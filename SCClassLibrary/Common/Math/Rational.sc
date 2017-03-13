Rational : Number {
	var <numerator, <denominator;

	*new {  arg numerator, denominator;
		^super.newCopyArgs(numerator, denominator).reduce
	}

	*newFrom { arg that; ^that.asRational }

	numerator_ { arg newNum; numerator = newNum; this.reduce }

	denominator_ { arg newDen; denominator = newDen; this.reduce }

	reduce {
		var d;
		if (denominator == 0) {"Rational has zero denominator".error; ^inf};
		d = this.factor;
		numerator   = (this.numerator/d).abs * d.sign;
		denominator = (this.denominator/d).abs;
	}

	reduceNestedRationals {
		^if(numerator.isRational or: denominator.isRational) {
			(numerator.asRational / denominator.asRational)
		};
	}

	factor {
		var d = gcd(this.numerator.asInteger, this.denominator.asInteger);
		if (denominator < 0) { d = d.neg };
		if (numerator   < 0) { d = d.neg };
		^d
	}

	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		^aNumber.asRational.perform(aSelector, this, adverb)
	}

	isRational { ^true }

	%/ { arg aNumber; ^Rational(this, aNumber) }

	asRational { ^this }

	asFloat { ^this.numerator / this.denominator }

	asInteger { ^(this.numerator / this.denominator).asInteger }

	asInt { ^this.asInteger }

	reciprocal { ^Rational(this.denominator, this.numerator) }

	// printOn { arg stream;
	//	stream << "Rational( " << numerator << ", " << denominator << " )";
	// }

	printOn { arg stream;
		stream <<  numerator << " %/ " << denominator;
	}

	hash { ^this.instVarHash }

	+ { arg aNumber, adverb;
		^this.class.new(
			(this.numerator * aNumber.denominator) + (this.denominator * aNumber.numerator),
			this.denominator * aNumber.denominator)
	}

	- { arg aNumber, adverb;
		^this.class.new(
			(this.numerator * aNumber.denominator) - (this.denominator * aNumber.numerator),
			this.denominator * aNumber.denominator )
	}

	* { arg aNumber, adverb;
		^this.class.new(
			this.numerator * aNumber.numerator,
			this.denominator * aNumber.denominator)
	}

	/ { arg aNumber, adverb;
		^this.class.new(
			this.numerator * aNumber.denominator,
			this.denominator * aNumber.numerator )
	}

	== { arg aNumber, adverb;
		^((this.numerator * aNumber.denominator) == (this.denominator * aNumber.numerator))
	}

	!= { arg aNumber, adverb;
		^((this.numerator * aNumber.denominator) != (this.denominator * aNumber.numerator))
	}

	pow { arg n;

        if (n.isInteger.not) {"Exponent is not Integer".error};

		^case
		{ n == 0 } { Rational(1,1) }

		// if n is a for non-negative integer
		{ n > 0 } {
			this.class.new(
				this.numerator.pow(n),
				this.denominator.pow(n)
			)
		}
		// if n is negative integer
		{ n < 0  } {
			// if numerator ≠ 0
			if((this.numerator == 0).not) {
				this.reciprocal.pow(n.abs)
			} {
				"Rational has zero denominator".error;
			}
		}
	}

	simplify { arg maxDenominator=20;
		^this.asFloat.asRational(maxDenominator)
	}

	< { arg aNumber;
		^(this.numerator * aNumber.denominator) < (this.denominator * aNumber.numerator)
	}

	> { arg aNumber;
		^(this.numerator * aNumber.denominator) > (this.denominator * aNumber.numerator)
	}

	<= { arg aNumber;
		^(this.numerator * aNumber.denominator) <= (this.denominator * aNumber.numerator)
	}

	>= { arg aNumber;
		^(this.numerator * aNumber.denominator) >= (this.denominator * aNumber.numerator)
	}

	isNegative { ^this.numerator.isNegative }

	isPositive { ^this.numerator.isPositive }

	neg { ^this.class.new(numerator.neg,denominator)}

	// sqrt { ^this.pow(this.class.new(1,2)) }

	squared { ^this.pow(2) }

	cubed { ^this.pow(3) }

	abs { ^this.class.new(numerator.abs, denominator) }

}

