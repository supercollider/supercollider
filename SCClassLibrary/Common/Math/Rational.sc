Rational : Number {
	var <>numerator, <>denominator;

	*new { arg numerator, denominator = 1;
		^super.new.initRational(numerator, denominator);
	}

	*newFromString { arg thisString;
		^thisString.asRational;
	}

	initRational { arg thisNumerator, thisDenominator;
		var factor;
		if(thisNumerator.isString) {
			^thisNumerator.asRational
		} {
			if(thisNumerator.isKindOf(Rational) or: thisDenominator.isKindOf(Rational)) {
				^(thisNumerator.asRational / thisDenominator.asRational)
			} {
				if(thisNumerator.frac == 0 and: thisDenominator.frac == 0) {
					if(thisDenominator == 0) {
						this.numerator_(0);
						this.denominator_(0);
					} {
						factor = gcd(thisNumerator.abs.asInteger, thisDenominator.abs.asInteger);
						(thisDenominator < 0).if({factor = factor.neg});
						(thisNumerator   < 0).if({factor = factor.neg});
						this.numerator_((((thisNumerator/factor).abs) * factor.sign).asInteger);
						this.denominator_((thisDenominator/factor).abs.asInteger);
					};
				} {
					^(thisNumerator / thisDenominator).asRational
				};
			};
		};
	}
	
	performBinaryOpOnSimpleNumber { arg aSelector, aNumber, adverb;
		^aNumber.asRational.perform(aSelector, this, adverb)
	}
	
	isRational { ^true }
	
	%/ { arg aNumber; ^Rational(this, aNumber) }
	
	asRational { ^this }
	
	as { arg aSimilarClass; ^aSimilarClass.new(this.numerator, this.denominator) }
	
	asArray { ^[this.numerator, this.denominator]}
	
	asFloat { ^(this.numerator / this.denominator)}
	
	asInteger { ^((this.numerator / this.denominator).asInteger) }
	
	asInt { ^this.asInteger }
	
	reciprocal { ^Rational(this.denominator, this.numerator) }
	
	printOn { arg stream;
		stream << "Rational( " << numerator << ", " << denominator << " )";
	}
	
	hash { ^numerator.hash << 2 bitXor: denominator.hash } 
	
	+ { arg aNumber, adverb;
		if(aNumber.isNumber) {
			^this.species.new(
				(this.numerator * aNumber.denominator) + (this.denominator * aNumber.numerator),
				this.denominator * aNumber.denominator)
		} {
			^aNumber.performBinaryOpOnRational('+', this, adverb)
		};
	}
	
	- { arg aNumber, adverb;
		if(aNumber.isKindOf(Rational)) {
			^this.species.new(
				(this.numerator * aNumber.denominator) - (this.denominator * aNumber.numerator),
				this.denominator * aNumber.denominator )
		}  {
			^aNumber.performBinaryOpOnRational('-', this, adverb)
		};	
	}
	
	* { arg aNumber, adverb;
		if(aNumber.isKindOf(Rational)) {
			^this.species.new(
				this.numerator * aNumber.numerator,
				this.denominator * aNumber.denominator)
		} {
			^aNumber.performBinaryOpOnRational('*', this, adverb)
		};
	}
	
	
	/ { arg aNumber, adverb;
		if(aNumber.isKindOf(Rational)) {
			^this.species.new(
				this.numerator * aNumber.denominator,
				this.denominator * aNumber.numerator )
		} {
			^aNumber.performBinaryOpOnRational('/', this, adverb)
		}
	}
	
	== { arg aNumber, adverb;
		if(aNumber.isKindOf(Rational)) {
			^((this.numerator * aNumber.denominator) == (this.denominator * aNumber.numerator))
		} {
			^aNumber.performBinaryOpOnRational('==', this, adverb)
		}	
	}
	
	!= { arg aNumber, adverb;
		if(aNumber.isKindOf(Rational)) {
			^((this.numerator * aNumber.denominator) != (this.denominator * aNumber.numerator))
		} {
			^aNumber.performBinaryOpOnRational('!=', this, adverb)
		}	
	}
	
	pow { arg aNumber;
		case
		{ aNumber == 0 } { ^(1.as(this.species)) }
		{ aNumber > 0  } { ^(this.species.new(this.numerator.pow(aNumber.asFloat), this.denominator.pow(aNumber.asFloat))) }
		{ aNumber < 0  } {
			if((this.numerator == 0).not) {
				^this.reciprocal.pow(aNumber.abs)
			} {
				"Numerator can't be 0".warn;
			};
		};
	}
	
	simplify { arg maxDenominator=20;
		^this.asFloat.asRational(maxDenominator)
	}
	
	< { arg aNumber;
		^(this.asFloat < aNumber.asFloat)
	}
	
	> { arg aNumber;
		^(this.asFloat > aNumber.asFloat)
	}
	
	<= { arg aNumber;
		^(this.asFloat <= aNumber.asFloat)
	}
	
	>= { arg aNumber;
		^(this.asFloat >= aNumber.asFloat)
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
	
	mod {
		if (this.isNegative) {
			^this.neg
		} {
			^this
		};
	}
	
	max { arg aNumber;
		var rationalArray, floatArray;
		rationalArray = [this, aNumber];
		floatArray = rationalArray.asFloat;
		^rationalArray.at(floatArray.indexOf(floatArray.max));
	}
	
	min { arg aNumber;
		var rationalArray, floatArray;
		rationalArray = [this, aNumber];
		floatArray = rationalArray.asFloat;
		^rationalArray.at(floatArray.indexOf(floatArray.min));
	}
	
	sqrt { ^this.pow(this.species.new(1,2)) }
	squared { ^this.pow(this.species.new(2,1)) }
	cubed { ^this.pow(this.species.new(3,1)) }	
	abs { ^this.species.new(this.numerator.abs.asInteger, this.denominator.abs.asInteger) }
}