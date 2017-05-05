Rational : Number {
	var <numerator, <denominator;

	*new {  arg numerator=1.0, denominator=1.0;
		^super.newCopyArgs(numerator, denominator).reduce
	}

	reduce {
		var d;
		if (this.numerator.isKindOf(Number)) {
			if (this.numerator.isKindOf(Rational) || this.denominator.isKindOf(Rational)){
				^(numerator.asRational / denominator.asRational)
			};
			if (this.numerator.frac == 0 && this.denominator.frac == 0) {
				if (denominator == 0) {
					if (numerator == 0) {
						"Rational has zero denominator".error;^0/0
					}{
						"Rational has zero denominator".error;^inf
					}
				};
				d = this.factor;
				numerator = ((this.numerator/d).abs * d.sign).round;
				denominator = (this.denominator/d).abs.round;
				^Rational.fromReducedInts(numerator,denominator);
			} {
				^(this.numerator / this.denominator).asRational
			}
		} {
			if (this.numerator.isKindOf(String)) {
				^this.numerator.asRational
			}
		}
	}

	*fromReducedInts { arg numerator=1.0, denominator=1.0;
		^super.newCopyArgs(numerator, denominator);
	}

	factor {
		var d = gcd(this.numerator.asInteger, this.denominator.asInteger).abs;
		if (denominator < 0) { d = d.neg };
		if (numerator   < 0) { d = d.neg };
		^d
	}

	*newFrom { arg that; ^that.asRational }

	numerator_ { arg newNum=1.0; numerator = newNum; this.reduce }

	denominator_ { arg newDen=1.0; denominator = newDen; this.reduce }

	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		^aNumber.asRational.perform(aSelector, this, adverb)
	}

	isRational { ^true }

	%/ { arg aNumber; ^Rational(this, aNumber) }

	asRational { ^this }

	asFloat { ^(this.numerator / this.denominator).asFloat }

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

		/*if (n.isInteger.not) {"Exponent is not Integer".error};*/

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

	simplify { arg maxDenominator=20, fasterBetter=false;
		^this.asFloat.asRational(maxDenominator,fasterBetter)
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

	isNegative { ^this.numerator.isNegative}

	isPositive { ^this.numerator.isPositive }

	neg { ^(this * (-1)) }

	squared { ^this.pow(2) }

	cubed { ^this.pow(3) }

	abs { ^this.class.new(numerator.abs, denominator) }

}

