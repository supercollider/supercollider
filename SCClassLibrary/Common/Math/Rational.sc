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

	pow { arg aNumber;
		^case
		{ aNumber == 0 }
		{ (1.as(this.species)) }


		{ aNumber > 0  }
		{ (this.species.new(this.numerator.pow(aNumber.asFloat), this.denominator.pow(aNumber.asFloat))) }

		{ aNumber < 0  }
		{
			if((this.numerator == 0).not) {
				this.reciprocal.pow(aNumber.abs)
			} {
				"Rational has zero denominator".error; inf
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

	isNegative {
		^this.numerator.isNegative
	}

	isPositive {
		^this.numerator.isPositive
	}

	neg {
		^(this * (-1))
	}

	sqrt { ^this.pow(this.species.new(1,2)) }

	squared { ^this.pow(this.species.new(2,1)) }

	cubed { ^this.pow(this.species.new(3,1)) }

	abs { ^this.class.new(numerator.abs, denominator) }

}
										